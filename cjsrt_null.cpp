//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "cjsruntime.h"

namespace clib {

    std::string jsv_null::_str = "null";

    runtime_t jsv_null::get_type() {
        return r_null;
    }

    js_value::ref jsv_null::binary_op(js_value_new &n, int code, const js_value::ref &op) {
        switch (code) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 < JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(false);
                            case 2:
                                return n.new_boolean(0.0 < d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(JS_BOOL(op));
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_LESS_EQUAL:
                switch (op->get_type()) {
                    case r_number: {
                        auto d = JS_NUM(op);
                        if (std::isnan(d))
                            return n.new_boolean(false);
                        return n.new_boolean(0.0 <= JS_NUM(op));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(true);
                            case 2:
                                if (std::isnan(d))
                                    return n.new_boolean(false);
                                return n.new_boolean(0.0 <= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                    case r_null:
                        return n.new_boolean(true);
                    case r_object:
                    case r_function:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                        return n.new_boolean(false);
                    case r_undefined:
                    case r_null:
                        return n.new_boolean(true);
                    default:
                        break;
                }
                break;
            case COMPARE_NOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_op(n, COMPARE_EQUAL, op)));
            case COMPARE_GREATER:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 > JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(false);
                            case 2:
                                return n.new_boolean(0.0 > d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_GREATER_EQUAL:
                switch (op->get_type()) {
                    case r_number: {
                        auto d = JS_NUM(op);
                        if (std::isnan(d))
                            return n.new_boolean(false);
                        return n.new_boolean(0.0 >= JS_NUM(op));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(true);
                            case 2:
                                if (std::isnan(d))
                                    return n.new_boolean(false);
                                return n.new_boolean(0.0 >= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(!JS_BOOL(op));
                    case r_null:
                        return n.new_boolean(true);
                    case r_object:
                    case r_function:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_FEQUAL:
                switch (op->get_type()) {
                    case r_null:
                        return n.new_boolean(true);
                    case r_boolean:
                    case r_number:
                    case r_string:
                    case r_object:
                    case r_function:
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
                        if (std::isnan(s))
                            return n.new_number(NAN);
                        if (s > 0.0)
                            return n.new_number(0.0);
                        else
                            return n.new_number(INFINITY);
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
                                if (std::isinf(d) && std::signbit(d) != 0)
                                    return n.new_number(INFINITY);
                                return n.new_number(0.0);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(0.0) :
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
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return op;
                        if (std::isinf(s) || std::isnan(s))
                            return n.new_number(NAN);
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2:
                                if (std::isinf(d))
                                    return n.new_number(NAN);
                                return n.new_number(0.0);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(0.0);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(0.0);
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_MODULO:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0 || std::isnan(s))
                            return n.new_number(NAN);
                        return n.new_number(0.0);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(NAN);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(NAN);
                                return n.new_number(0.0);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(0.0) :
                               n.new_number(NAN);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(NAN);
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_ADD:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(0.0);
                        return op;
                    }
                    case r_string:
                        return n.new_string(_str + JS_STR(op));
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(1.0) :
                               n.new_number(0.0);
                    case r_object:
                        return n.new_string(_str + op->to_string(&n, 0));
                    case r_function:
                        return n.new_string(_str + op->to_string(&n, 0));
                    case r_null:
                        return n.new_number(0.0);
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_SUBTRACT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(0.0);
                        return n.new_number(0.0 - s);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(0.0);
                                return n.new_number(0.0 - d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(-1.0) :
                               n.new_number(0.0);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(0.0);
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
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0 || std::isnan(s))
                            return n.new_number(NAN);
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(NAN);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(NAN);
                                return n.new_number(std::signbit(d) ? -0.0 : 0.0);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(0.0) :
                               n.new_number(NAN);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(NAN);
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

    js_value::ref jsv_null::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE:
                return n.new_number(0.0);
            case UNARY_NEGATIVE:
                return n.new_number(-0.0);
            case UNARY_NOT:
                return n.new_boolean(true);
            case UNARY_INVERT:
                return n.new_number(-1.0);
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("object");
            default:
                break;
        }
        return nullptr;
    }

    bool jsv_null::to_bool() const {
        return false;
    }

    void jsv_null::mark(int n) {
    }

    std::string jsv_null::to_string(js_value_new *n, int hint) const {
        return _str;
    }

    double jsv_null::to_number(js_value_new *n) const {
        return 0;
    }
}