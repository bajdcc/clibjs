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
#include "cjsparser.h"

#define DUMP_STEP 1
#define DUMP_GC 1
#define GC_PERIOD 128
#define DUMP_PRINT_FILE_ENABLE 1
#define DUMP_PRINT_FILE_AUTO_CLEAR 1
#define DUMP_PRINT_FILE "debug_print.txt"

#define LOG_AST 0
#define LOG_FILE 0

namespace clib {

    void cjsruntime::init(void *p) {
        pjs = p;
        // proto
        permanents._proto_root = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_root->obj["__type__"] = new_string("root");
        permanents._proto_root->__proto__.reset();
        permanents._proto_object = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_object->obj["__type__"] = new_string("object");
        permanents._proto_object->__proto__ = permanents._proto_root;
        permanents._proto_object_hasOwnProperty = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
        permanents._proto_object_hasOwnProperty->name = "hasOwnProperty";
        permanents._proto_object_hasOwnProperty->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            if (args.empty()) {
                func->stack.push_back(js.new_boolean(false));
                return 0;
            }
            auto f = _this.lock();
            if (f->get_type() != r_object) {
                func->stack.push_back(js.new_boolean(false));
                return 0;
            }
            const auto &obj = JS_OBJ(f);
            func->stack.push_back(js.new_boolean(obj.find(args.front().lock()->to_string()) != obj.end()));
            return 0;
        };
        permanents._proto_object->obj.insert({permanents._proto_object_hasOwnProperty->name, permanents._proto_object_hasOwnProperty});
        permanents._proto_boolean = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_boolean->obj["__type__"] = new_string("boolean");
        permanents._proto_function = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_function->obj["__type__"] = new_string("function");
        permanents._proto_function_call = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
        permanents._proto_function_call->name = "call";
        permanents._proto_function_call->builtin = [](auto &func, auto &_this, auto &__args, auto &js, auto attr) {
            auto f = _this.lock();
            assert(f->get_type() == r_function);
            auto fun = JS_FUN(f);
            auto __this = __args.empty() ? _this : __args.front();
            auto args = __args.size() > 1 ?
                        std::vector<js_value::weak_ref>(__args.begin() + 1, __args.end()) :
                        std::vector<js_value::weak_ref>();
            if (fun->builtin) {
                return fun->builtin(func, __this, args, js, 0);
            }
            auto new_stack = js.new_func(fun->code);
            new_stack->_this = __this;
            new_stack->name = fun->name;
            auto env = new_stack->envs.lock();
            if (!fun->code->arrow && func->name.front() != '<')
                env->obj[func->name] = f;
            auto arg = js.new_object();
            env->obj["arguments"] = arg;
            size_t i = 0;
            for (; i < args.size(); i++) {
                std::stringstream ss;
                ss << i;
                arg->obj[ss.str()] = args.at(i);
                if (i < fun->code->args.size())
                    env->obj[fun->code->args.at(i)] = args.at(i);
            }
            for (; i < fun->code->args.size(); i++) {
                env->obj[fun->code->args.at(i)] = js.new_undefined();
            }
            arg->obj["length"] = js.new_number(args.size());
            if (fun->closure.lock())
                new_stack->closure = fun->closure;
            func->pc++;
            return 1;
        };
        permanents._proto_function->obj.insert({permanents._proto_function_call->name, permanents._proto_function_call});
        permanents._proto_function_apply = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
        permanents._proto_function_apply->name = "apply";
        permanents._proto_function_apply->builtin = [](auto &func, auto &_this, auto &__args, auto &js, auto attr) {
            auto f = _this.lock();
            assert(f->get_type() == r_function);
            auto fun = JS_FUN(f);
            auto __this = __args.empty() ? _this : __args.front();
            auto args = __args.size() > 1 ?
                        to_array(__args[1].lock()) :
                        std::vector<js_value::weak_ref>();
            if (fun->builtin) {
                return fun->builtin(func, __this, args, js, 0);
            }
            auto new_stack = js.new_func(fun->code);
            new_stack->_this = __this;
            new_stack->name = fun->name;
            auto env = new_stack->envs.lock();
            if (!fun->code->arrow && func->name.front() != '<')
                env->obj[func->name] = f;
            auto arg = js.new_object();
            env->obj["arguments"] = arg;
            size_t i = 0;
            for (; i < args.size(); i++) {
                std::stringstream ss;
                ss << i;
                arg->obj[ss.str()] = args.at(i);
                if (i < fun->code->args.size())
                    env->obj[fun->code->args.at(i)] = args.at(i);
            }
            for (; i < fun->code->args.size(); i++) {
                env->obj[fun->code->args.at(i)] = js.new_undefined();
            }
            arg->obj["length"] = js.new_number(args.size());
            if (fun->closure.lock())
                new_stack->closure = fun->closure;
            func->pc++;
            return 1;
        };
        permanents._proto_function->obj.insert({permanents._proto_function_apply->name, permanents._proto_function_apply});
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
        permanents._debug_print = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
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
        permanents._debug_dump = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
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
        permanents.console_log = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
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
        // sys
        permanents.sys = _new_object(js_value::at_const | js_value::at_readonly);
        permanents.sys_exec_file = _new_function(nullptr, js_value::at_const | js_value::at_readonly);
        permanents.sys_exec_file->name = "exec_file";
        permanents.sys_exec_file->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            if (args.empty() || args.front().lock()->get_type() != r_string) {
                func->stack.push_back(js.new_undefined());
                return 0;
            }
            std::ifstream file(args.front().lock()->to_string());
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                auto str = buffer.str();
                func->pc++;
                js.exec(str);
                return 3;
            } else {
                func->stack.push_back(js.new_undefined());
            }
            return 0;
        };
        permanents.sys->obj.insert({permanents.sys_exec_file->name, permanents.sys_exec_file});
        global_env.insert({"sys", permanents.sys});
        // number
        permanents.f_number = _new_function(permanents._proto_number, js_value::at_const | js_value::at_readonly);
        permanents.f_number->name = "Number";
        permanents.f_number->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_number(0.0);
            } else {
                auto n = args.front().lock();
                double d = 0.0;
                if (to_number(n, d)) {
                    pri = js.new_number(d);
                } else {
                    pri = js.new_number(NAN);
                }
            }
            if (attr & jsv_function::at_new_function) {
                pri = js.new_object_box(pri);
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_number->name, permanents.f_number});
        // boolean
        permanents.f_boolean = _new_function(permanents._proto_boolean, js_value::at_const | js_value::at_readonly);
        permanents.f_boolean->name = "Boolean";
        permanents.f_boolean->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                pri = js.new_object_box(pri);
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_boolean->name, permanents.f_boolean});
        // object
        permanents.f_object = _new_function(permanents._proto_object, js_value::at_const | js_value::at_readonly);
        permanents.f_object->name = "Object";
        permanents.f_object->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                pri = js.new_object_box(pri);
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_object->name, permanents.f_object});
        // string
        permanents.f_string = _new_function(permanents._proto_string, js_value::at_const | js_value::at_readonly);
        permanents.f_string->name = "String";
        permanents.f_string->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_string("");
            } else {
                pri = js.new_string(args.front().lock()->to_string());
            }
            if (attr & jsv_function::at_new_function) {
                pri = js.new_object_box(pri);
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_string->name, permanents.f_string});
        // function
        permanents.f_function = _new_function(permanents._proto_function, js_value::at_const | js_value::at_readonly);
        permanents.f_function->name = "Function";
        permanents.f_function->builtin = [](auto &func, auto &_this, auto &args, auto &js, auto attr) {
            js_value::ref pri;
            if (args.empty()) {
                pri = js.new_boolean(false);
            } else {
                pri = js.new_boolean(args.front().lock()->to_bool());
            }
            if (attr & jsv_function::at_new_function) {
                pri = js.new_object_box(pri);
            }
            func->stack.push_back(pri);
            return 0;
        };
        global_env.insert({permanents.f_function->name, permanents.f_function});
        // array
        permanents._proto_array = _new_object(js_value::at_const | js_value::at_readonly);
        permanents._proto_array->obj["__type__"] = new_string("array");
        permanents.f_array = _new_function(permanents._proto_array, js_value::at_const | js_value::at_readonly);
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