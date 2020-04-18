//
// Project: clibjs
// Created by bajdcc
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <fstream>
#include "cjsruntime.h"
#include "cjsast.h"

#define DUMP_STEP 1
#define DUMP_GC 1
#define GC_PERIOD 128
#define DUMP_PRINT_FILE_ENABLE 1
#define DUMP_PRINT_FILE_AUTO_CLEAR 1
#define DUMP_PRINT_FILE "debug_print.txt"

namespace clib {

    void cjsruntime::init() {
        // proto
        permanents._proto_root = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_root->obj["__type__"] = new_string("root");
        permanents._proto_root->__proto__.reset();
        permanents._proto_object = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_object->obj["__type__"] = new_string("object");
        permanents._proto_object->__proto__ = permanents._proto_root;
        permanents._proto_boolean = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_boolean->obj["__type__"] = new_string("boolean");
        permanents._proto_function = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_function->obj["__type__"] = new_string("function");
        permanents._proto_function_call = _new_function(js_value::at_const | js_value::at_readonly);
        permanents._proto_function_call->name = "call";
        permanents._proto_function_call->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            auto f = _this.lock();
            assert(f->get_type() == r_function);
            auto fun = JS_FUN(f);
            if (fun->builtin) {
                if (args.empty())
                    return fun->builtin(func, _this, args, js, 0);
                typename std::remove_reference<decltype(args)>::type
                        _args(args.begin() + 1, args.end());
                return fun->builtin(func, args.front(), _args, js, 0);
            }
            auto new_stack = js.new_func(fun->code);
            new_stack->_this = args.empty() ? _this : args.front();
            new_stack->name = fun->name;
            auto env = new_stack->envs.lock();
            if (!fun->code->arrow && fun->name.front() != '<')
                env->obj[fun->name] = f;
            if (!args.empty())
                for (size_t i = 0; i < fun->code->args.size(); i++) {
                    env->obj[fun->code->args.at(i)] = args.at(i + 1);
                }
            if (fun->closure.lock())
                new_stack->closure = fun->closure;
            func->pc++;
            return 1;
        };
        permanents._proto_function->obj.insert({permanents._proto_function_call->name, permanents._proto_function_call});
        permanents._proto_number = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_number->obj["__type__"] = new_string("number");
        permanents._proto_string = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_string->obj["__type__"] = new_string("string");
        // const value
        permanents._null = _new_null(js_value::at_const | js_value::at_readonly);
        permanents._undefined = _new_undefined(js_value::at_const | js_value::at_readonly);
        permanents._true = _new_boolean(true, js_value::at_const | js_value::at_readonly);
        permanents._false = _new_boolean(false, js_value::at_const | js_value::at_readonly);
        permanents.__nan = _new_number(NAN, js_value::at_const | js_value::at_readonly);
        permanents._inf = _new_number(INFINITY, js_value::at_const | js_value::at_readonly);
        permanents._minus_inf = _new_number(-INFINITY, js_value::at_const);
        permanents._zero = _new_number(0.0, js_value::at_const);
        permanents._minus_zero = _new_number(-0.0, js_value::at_const);
        permanents._one = _new_number(1.0, js_value::at_const);
        permanents._minus_one = _new_number(-1.0, js_value::at_const);
        permanents._empty = _new_string("");
#if DUMP_PRINT_FILE_ENABLE
        permanents._debug_print = _new_function(js_value::at_const | js_value::at_readonly);
#if DUMP_PRINT_FILE_AUTO_CLEAR
        permanents._debug_print->name = "debug_print";
        {
            std::ofstream ofs(DUMP_PRINT_FILE);
        }
#endif
#endif
#if DUMP_PRINT_FILE_ENABLE
        permanents._debug_print->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            std::ofstream ofs(DUMP_PRINT_FILE, std::ios::app);
            for (size_t i = 0; i < args.size(); i++) {
                args[i].lock()->print(ofs);
                if (i + 1 < args.size())
                    ofs << " ";
            }
            ofs << std::endl;
            func->stack.push_back(js.new_undefined());
            return 0;
        };
        global_env.insert({permanents._debug_print->name, permanents._debug_print});
#endif
        global_env.insert({"NaN", permanents.__nan});
        global_env.insert({"Infinity", permanents._inf});
        // debug
        permanents._debug_dump = _new_function(js_value::at_const | js_value::at_readonly);
        permanents._debug_dump->name = "debug_dump";
        permanents._debug_dump->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            char buf[128];
            auto t = _this.lock();
            assert(t);
            auto type = t->unary_op(js, UNARY_TYPEOF)->to_string();
            auto proto = t->__proto__.lock();
            auto p = std::string("none");
            if (proto->get_type() == r_object) {
                auto o = JS_OBJ(proto);
                auto of = o.find("__type__");
                if (of != o.end()) {
                    p = of->second.lock()->to_string();
                }
            }
            snprintf(buf, sizeof(buf),
                     "Str: %s, Type: %s, Proto: %s, Ptr: %p",
                     t->to_string().c_str(), type.c_str(), p.c_str(), t.get());
            func->stack.push_back(js.new_string(buf));
            return 0;
        };
        permanents._proto_root->obj.insert({permanents._debug_dump->name, permanents._debug_dump});
        // console
        permanents.console = _new_object(js_value::at_const | js_value::at_readonly);
        permanents.console_log = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.console_log->name = "log";
        permanents.console_log->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            std::stringstream ss;
            for (size_t i = 0; i < args.size(); i++) {
                args[i].lock()->print(ss);
                if (i + 1 < args.size())
                    ss << " ";
            }
            ss << std::endl;
            auto s = ss.str();
            std::cout << ss.str();
            func->stack.push_back(js.new_undefined());
            return 0;
        };
        permanents.console->obj.insert({permanents.console_log->name, permanents.console_log});
        global_env.insert({"console", permanents.console});
        // number
        permanents.f_number = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_number->name = "Number";
        permanents.f_number->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_number(0.0);
            } else {
                auto n = args.front().lock();
                double d = 0.0;
                if (js.to_number(n, d)) {
                    pri = js.new_number(d);
                } else {
                    pri = js.new_number(NAN);
                }
            }
            if (attr & jsv_function::at_new_function) {
                auto new_pri = js.new_object();
                new_pri->special.insert({"PrimitiveValue", pri});
                pri = new_pri;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_number->name, permanents.f_number});
        // boolean
        permanents.f_boolean = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_boolean->name = "Boolean";
        permanents.f_boolean->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                auto new_pri = js.new_object();
                new_pri->special.insert({"PrimitiveValue", pri});
                pri = new_pri;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_boolean->name, permanents.f_boolean});
        // object
        permanents.f_object = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_object->name = "Object";
        permanents.f_object->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                auto new_pri = js.new_object();
                new_pri->special.insert({"PrimitiveValue", pri});
                pri = new_pri;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_object->name, permanents.f_object});
        // string
        permanents.f_string = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_string->name = "String";
        permanents.f_string->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_string("");
            } else {
                pri = js.new_string(args.front().lock()->to_string());
            }
            if (attr & jsv_function::at_new_function) {
                auto new_pri = js.new_object();
                new_pri->special.insert({"PrimitiveValue", pri});
                pri = new_pri;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_string->name, permanents.f_string});
        // function
        permanents.f_function = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_function->name = "Function";
        permanents.f_function->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                auto new_pri = js.new_object();
                new_pri->special.insert({"PrimitiveValue", pri});
                pri = new_pri;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_function->name, permanents.f_function});
        // array
        permanents._proto_array = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_array->obj["__type__"] = new_string("array");
        // slice
        permanents.f_array_slice = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_array_slice->name = "slice";
        permanents.f_array_slice->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            auto f = _this.lock();
            if (f->get_type() != r_object) {
                func->stack.push_back(js.new_array());
                return 0;
            }
            auto obj = JS_OBJ(f);
            auto l = obj.find("length");
            if (l == obj.end()) {
                func->stack.push_back(js.new_array());
                return 0;
            }
            auto len = l->second.lock();
            auto length = 0;
            double d = 0.0;
            if (js.to_number(len, d)) {
                if (!(std::isinf(d) && std::isnan(d)))
                    length = std::floor(d);
            }
            auto slice = 0;
            if (!args.empty()) {
                auto a = args.front().lock();
                d = 0.0;
                if (js.to_number(a, d)) {
                    if (!(std::isinf(d) && std::isnan(d)))
                        slice = std::floor(d);
                }
            }
            if (slice >= length) {
                func->stack.push_back(js.new_array());
                return 0;
            }
            auto arr = js.new_array();
            for (auto i = slice; i < length; i++) {
                std::stringstream ss;
                ss << i;
                auto ff = obj.find(ss.str());
                if (ff != obj.end()) {
                    arr->obj[ss.str()] = ff->second;
                }
            }
            arr->obj["length"] = js.new_number(length - slice);
            func->stack.push_back(arr);
            return 0;
        };
        // concat
        permanents.f_array_concat = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_array_concat->name = "concat";
        permanents.f_array_concat->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            auto f = _this.lock();
            auto i = 0;
            auto arr = js.new_array();
            auto &_objs = arr->obj;
            std::stringstream ss;
            typename std::remove_reference< typeof args>::type _args(args.size() + 1);
            _args[0] = f;
            std::copy(args.begin(), args.end(), _args.begin() + 1);
            auto first = true;
            for (const auto &a : _args) {
                if (a.lock()->get_type() != r_object) {
                    if (first) {
                        auto o = js.new_object();
                        o->special.insert({"PrimitiveValue", f});
                        ss.str("");
                        ss << i++;
                        _objs[ss.str()] = o;
                    } else {
                        ss.str("");
                        ss << i++;
                        _objs[ss.str()] = a.lock();
                    }
                } else if (a.lock()->__proto__.lock() == arr->__proto__.lock()) {
                    auto ao = JS_OBJ(a.lock());
                    auto of = ao.find("length");
                    if (of != ao.end()) {
                        auto len = of->second.lock();
                        double d = 0.0;
                        if (js.to_number(len, d)) {
                            if (!(std::isinf(d) && std::isnan(d)) && d > 0) {
                                auto l = std::floor(d);
                                auto j = 0;
                                for (const auto &s : ao) {
                                    if (jsv_string::to_number(s.first, d) == 2 && d >= 0 && d < l) {
                                        auto dd = std::floor(d);
                                        if (d == dd) {
                                            ss.str("");
                                            ss << (i + j);
                                            _objs[ss.str()] = s.second;
                                        }
                                    }
                                }
                                i += l;
                            }
                        }
                    }
                } else {
                    ss.str("");
                    ss << i++;
                    _objs[ss.str()] = a.lock();
                }
                if (first)
                    first = false;
            }
            arr->obj["length"] = js.new_number(i);
            func->stack.push_back(arr);
            return 0;
        };
        permanents._proto_array->obj.insert({permanents.f_array_concat->name, permanents.f_array_concat});
        permanents.f_array = _new_function(js_value::at_const | js_value::at_readonly);
        permanents.f_array->name = "Array";
        permanents.f_array->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_array();
            } else {
                auto arr = js.new_array();
                if (args.size() == 1 && args.front().lock()->get_type() == r_number) {
                    arr->obj["length"] = args.front().lock();
                } else {
                    auto i = 0;
                    for (const auto &s : args) {
                        std::stringstream ss;
                        ss << i++;
                        arr->obj.insert({ss.str(), s});
                    }
                    arr->obj["length"] = js.new_number(i);
                }
                pri = arr;
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_array->name, permanents.f_array});
    }
}