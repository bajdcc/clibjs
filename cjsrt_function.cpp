//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include <cassert>
#include "cjsruntime.h"

namespace clib {

    jsv_function::jsv_function(const sym_code_t::ref &c, js_value_new &n) {
        code = std::make_shared<cjs_function_info>(std::move(c), n);
    }

    runtime_t jsv_function::get_type() {
        return r_function;
    }

    js_value::ref jsv_function::binary_op(js_value_new &n, int c, const js_value::ref &op) {
        switch (c) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_string:
                    case r_function:
                    case r_object:
                        return n.new_boolean(code->text < op->to_string(&n, 0));
                    case r_number:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_LESS_EQUAL:
                switch (op->get_type()) {
                    case r_string:
                    case r_function:
                    case r_object:
                        return n.new_boolean(code->text <= op->to_string(&n, 0));
                    case r_number:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_EQUAL:
                switch (op->get_type()) {
                    case r_string:
                    case r_object:
                        return n.new_boolean(code->text == op->to_string(&n, 0));
                    case r_function:
                        return n.new_boolean(shared_from_this() == op);
                    case r_number:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_NOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_op(n, COMPARE_EQUAL, op)));
            case COMPARE_GREATER:
                switch (op->get_type()) {
                    case r_string:
                    case r_function:
                    case r_object:
                        return n.new_boolean(code->text > op->to_string(&n, 0));
                    case r_number:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_GREATER_EQUAL:
                switch (op->get_type()) {
                    case r_string:
                    case r_function:
                    case r_object:
                        return n.new_boolean(code->text >= op->to_string(&n, 0));
                    case r_number:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_FEQUAL:
                switch (op->get_type()) {
                    case r_function:
                        return n.new_boolean(shared_from_this() == op);
                    case r_boolean:
                    case r_number:
                    case r_string:
                    case r_object:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_FNOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_op(n, COMPARE_FEQUAL, op)));
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(1.0);
                        return n.new_number(NAN);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(1.0);
                                return n.new_number(NAN);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(NAN) :
                               n.new_number(1.0);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_MULTIPLY:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_MODULO:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_ADD:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_string(to_string(&n, 0) + op->to_string(&n, 0));
                    default:
                        break;
                }
                break;
            case BINARY_SUBTRACT:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_FLOOR_DIVIDE:
                break;
            case BINARY_TRUE_DIVIDE:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_LSHIFT:
            case BINARY_RSHIFT:
            case BINARY_URSHIFT:
            case BINARY_AND:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(0.0);
                    default:
                        break;
                }
                break;
            case BINARY_XOR:
            case BINARY_OR:
                switch (op->get_type()) {
                    case r_number: {
                        auto d = fix(JS_NUM(op));
                        if (d == 0.0)d = 0.0;
                        return n.new_number(d);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2:
                                if (d == 0.0 || std::isinf(d))
                                    return n.new_number(0.0);
                                return n.new_number(d);
                            case 3:
                                return n.new_number(0.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(1.0) :
                               n.new_number(0.0);
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(0.0);
                    default:
                        break;
                }
                break;
            default:
                break;
        }
        return nullptr;
    }

    js_value::ref jsv_function::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE:
                return n.new_number(NAN);
            case UNARY_NEGATIVE:
                return n.new_number(NAN);
            case UNARY_NOT:
                return n.new_boolean(false);
            case UNARY_INVERT:
                return n.new_number(-1.0);
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("function");
            default:
                break;
        }
        return nullptr;
    }

    bool jsv_function::to_bool() const {
        return true;
    }

    void jsv_function::mark(int n) {
        jsv_object::mark(n);
        if (builtin)
            return;
        if (closure.lock()) {
            closure.lock()->mark(n);
        }
    }

    std::string jsv_function::to_string(js_value_new *n, int hint) const {
        if (builtin)
            return name;
        return code ? code->text : "builtin";
    }

    double jsv_function::to_number(js_value_new *n) const {
        return NAN;
    }

    jsv_function::ref jsv_function::clear2() {
        jsv_object::clear();
        code = nullptr;
        closure.reset();
        name.clear();
        return std::dynamic_pointer_cast<jsv_function>(shared_from_this());
    }

    cjs_function::cjs_function(const sym_code_t::ref &code, js_value_new &n) {
        reset(code, n);
    }

    cjs_function::cjs_function(cjs_function_info::ref code) {
        reset(std::move(code));
    }

    void cjs_function::store_name(const std::string &n, js_value::weak_ref obj) {
        envs.lock()->obj[n] = std::move(obj);
    }

    void cjs_function::store_fast(const std::string &n, js_value::weak_ref obj) {
        envs.lock()->obj[n] = std::move(obj);
    }

    void cjs_function::store_deref(const std::string &n, js_value::weak_ref obj) {
        auto c = closure.lock();
        if (!c)
            return;
        const auto &cc = JS_OBJ(c);
        auto f = cc.find(n);
        if (f == cc.end())
            return;
        auto var = f->second.lock();
        if (var->get_type() != r_object)
            return;
        auto &cc2 = JS_OBJ(var);
        cc2[n] = std::move(obj);
    }

    void cjs_function::clear() {
        info = nullptr;
        name = "UNKNOWN";
        pc = 0;
        stack.clear();
        ret_value.reset();
        _this.reset();
        envs.reset();
        closure.reset();
        rests.clear();
    }

    void cjs_function::reset(const sym_code_t::ref &code, js_value_new &n) {
        name = code->debugName;
        info = std::make_shared<cjs_function_info>(code, n);
    }

    void cjs_function::reset(cjs_function_info::ref code) {
        name = code->debugName;
        info = std::move(code);
    }

    cjs_function_info::cjs_function_info(const sym_code_t::ref &code, js_value_new &n) {
        arrow = code->arrow;
        debugName = std::move(code->debugName);
        simpleName = std::move(code->simpleName);
        fullName = std::move(code->fullName);
        args = std::move(code->args_str);
        std::copy(code->closure_str.begin(), code->closure_str.end(), std::back_inserter(closure));
        codes = std::move(code->codes);
        text = std::move(code->text);
        rest = code->rest;
        args_num = (int) args.size() - (rest ? 1 : 0);
        const auto &c = code->consts;
        std::copy(c.get_names_data().begin(),
                  c.get_names_data().end(),
                  std::back_inserter(names));
        std::copy(c.get_globals_data().begin(),
                  c.get_globals_data().end(),
                  std::back_inserter(globals));
        std::copy(c.get_derefs_data().begin(),
                  c.get_derefs_data().end(),
                  std::back_inserter(derefs));
        consts.resize(c.get_consts_data().size());
        for (size_t i = 0; i < c.get_consts_data().size(); i++) {
            consts[i] = load_const(c, i, n);
            consts[i]->attr |= js_value::at_const;
        }
    }

    js_value::ref cjs_function_info::load_const(const cjs_consts &c, int op, js_value_new &n) {
        auto t = c.get_type(op);
        switch (t) {
            case r_number:
                return n.new_number(*(double *) c.get_data(op));
            case r_string:
                return n.new_string(*(std::string *) c.get_data(op));
            case r_boolean:
                break;
            case r_object:
                break;
            case r_function: {
                auto f = n.new_function();
                auto code = ((sym_code_t::weak_ref *) c.get_data(op))->lock();
                f->code = std::make_shared<cjs_function_info>(code, n);
                return f;
            }
            default:
                break;
        }
        assert(!"invalid runtime type");
        return nullptr;
    }
}