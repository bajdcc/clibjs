//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

namespace clib {

    std::string jsv_boolean::_str_t = "true";

    std::string jsv_boolean::_str_f = "false";

    jsv_boolean::jsv_boolean(bool flag) : b(flag) {

    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    js_value::ref jsv_boolean::binary_op(js_value_new &n, int code, const js_value::ref &op) {
        return b ? binary_true(n, code, std::move(op)) : binary_false(n, code, std::move(op));
    }

    js_value::ref jsv_boolean::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE:
                return n.new_number(b ? 1.0 : 0.0);
            case UNARY_NEGATIVE:
                return n.new_number(b ? -1.0 : -0.0);
            case UNARY_NOT:
                return n.new_boolean(!b);
            case UNARY_INVERT:
                return n.new_number(b ? -2.0 : -1.0);
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("boolean");
            default:
                break;
        }
        return nullptr;
    }

    js_value::ref jsv_boolean::binary_true(js_value_new &n, int code, const js_value::ref &op) {
        switch (code) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(1.0 < JS_NUM(op));
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(false);
                            case 2:
                                return n.new_boolean(1.0 < d);
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
            case COMPARE_LESS_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(1.0 <= JS_NUM(op));
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(false);
                            case 2:
                                return n.new_boolean(1.0 <= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return op;
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(1.0 == JS_NUM(op));
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(false);
                            case 2:
                                return n.new_boolean(1.0 == d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return op;
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_NOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_true(n, COMPARE_EQUAL, op)));
            case COMPARE_GREATER:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(1.0 > JS_NUM(op));
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return shared_from_this();
                            case 2:
                                return n.new_boolean(1.0 > d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(!JS_BOOL(op));
                    case r_null:
                        return shared_from_this();
                    case r_object:
                    case r_function:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_GREATER_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(1.0 >= JS_NUM(op));
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return shared_from_this();
                            case 2:
                                return n.new_boolean(1.0 >= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                    case r_null:
                        return shared_from_this();
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
                    case r_boolean:
                        return op;
                    case r_number:
                    case r_string:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_FNOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_true(n, COMPARE_FEQUAL, op)));
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number: {
                        auto s = JS_NUM(op);
                        if (std::isnan(s) || std::isinf(s))
                            return n.new_number(NAN);
                        return n.new_number(1.0);
                    }
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                            case 2:
                                if (std::isinf(d))
                                    return n.new_number(NAN);
                                return n.new_number(1.0);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(1.0);
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
                        return op;
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2:
                                return n.new_number(d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(1.0) :
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
            case BINARY_MODULO:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0 || std::isnan(s))
                            return n.new_number(NAN);
                        if (std::isinf(s))
                            return n.new_number(1.0);
                        if (s == 1.0 || s == -1.0)
                            return n.new_number(0.0);
                        return n.new_number(fmod(1.0, s));
                    }
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(NAN);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(NAN);
                                if (d == 1.0)
                                    return n.new_number(0.0);
                                return n.new_number(1.0);
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
                    case r_number:
                        return n.new_number(JS_NUM(op) + 1.0);
                    case r_string:
                        return n.new_string(_str_t + JS_STR(op));
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(2.0) :
                               n.new_number(1.0);
                    case r_object:
                        return n.new_string(_str_t + jsv_object::_str);
                    case r_function: {
                        const auto &s = JS_STRF(op);
                        return n.new_string(_str_t + s);
                    }
                    case r_null:
                        return n.new_number(1.0);
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
                        return n.new_number(1.0 - s);
                    }
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2:
                                return n.new_number(1.0 - d);
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
            case BINARY_FLOOR_DIVIDE:
                break;
            case BINARY_TRUE_DIVIDE:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(std::signbit(s) == 0 ? INFINITY : -INFINITY);
                        return n.new_number(1.0 / s);
                    }
                    case r_string: {
                        double d = 0.0;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(INFINITY);
                            case 2:
                                if (d == 0.0)
                                    return n.new_number(INFINITY);
                                return n.new_number(1.0 / d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(1.0) :
                               n.new_number(INFINITY);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return n.new_number(INFINITY);
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_LSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1U << c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2: {
                                if (d == 0.0)
                                    return n.new_number(1.0);
                                auto a = fix(d);
                                auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                                return n.new_number(double(int(1U << c)));
                            }
                            case 3:
                                return n.new_number(1.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(
                                JS_BOOL(op) ? 2.0 : 1.0);
                    case r_object:
                        return n.new_number(1.0);
                    case r_function:
                        return n.new_number(1.0);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(1.0);
                    default:
                        break;
                }
                break;
            case BINARY_RSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(1.0);
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1 >> c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2: {
                                if (d == 0.0)
                                    return n.new_number(1.0);
                                auto a = fix(d);
                                auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                                return n.new_number(double(int(1 >> c)));
                            }
                            case 3:
                                return n.new_number(1.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(
                                JS_BOOL(op) ? 0.0 : 1.0);
                    case r_object:
                        return n.new_number(1.0);
                    case r_function:
                        return n.new_number(1.0);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(1.0);
                    default:
                        break;
                }
                break;
            case BINARY_URSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(1.0);
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1U >> c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2: {
                                auto a = fix(d);
                                auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                                return n.new_number(double(int(1U >> c)));
                            }
                            case 3:
                                return n.new_number(1.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(
                                JS_BOOL(op) ? 0.0 : 1.0);
                    case r_object:
                        return n.new_number(1.0);
                    case r_function:
                        return n.new_number(1.0);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(1.0);
                    default:
                        break;
                }
                break;
            case BINARY_AND:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(0.0);
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U & a)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2: {
                                if (d == 0.0)
                                    return n.new_number(0.0);
                                auto a = uint32_t(fix(d));
                                return n.new_number(double(int(1U & a)));
                            }
                            case 3:
                                return n.new_number(0.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(
                                JS_BOOL(op) ? 1.0 : 0.0);
                    case r_object:
                        return n.new_number(0.0);
                    case r_function:
                        return n.new_number(0.0);
                    case r_null:
                        return n.new_number(0.0);
                    case r_undefined:
                        return n.new_number(0.0);
                    default:
                        break;
                }
                break;
            case BINARY_XOR:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U ^ a)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2: {
                                if (d == 0.0)
                                    return n.new_number(1.0);
                                auto a = uint32_t(fix(d));
                                return n.new_number(double(int(1U ^ a)));
                            }
                            case 3:
                                return n.new_number(1.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(
                                JS_BOOL(op) ? 0.0 : 1.0);
                    case r_object:
                        return n.new_number(1.0);
                    case r_function:
                        return n.new_number(1.0);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(1.0);
                    default:
                        break;
                }
                break;
            case BINARY_OR:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U | a)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2: {
                                auto a = uint32_t(fix(d));
                                return n.new_number(double(int(1U | a)));
                            }
                            case 3:
                                return n.new_number(1.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_number(1.0);
                    case r_object:
                        return n.new_number(1.0);
                    case r_function:
                        return n.new_number(1.0);
                    case r_null:
                        return n.new_number(1.0);
                    case r_undefined:
                        return n.new_number(1.0);
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    js_value::ref jsv_boolean::binary_false(js_value_new &n, int code, const js_value::ref &op) {
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
                                return shared_from_this();
                            case 2:
                                return n.new_boolean(0.0 < d);
                            case 3:
                                return shared_from_this();
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return op;
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_LESS_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 <= JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(true);
                            case 2:
                                return n.new_boolean(0.0 <= d);
                            case 3:
                                return shared_from_this();
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
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 == JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(true);
                            case 2:
                                return n.new_boolean(0.0 == d);
                            case 3:
                                return shared_from_this();
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(!JS_BOOL(op));
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_NOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_false(n, COMPARE_EQUAL, op)));
            case COMPARE_GREATER:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 > JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return shared_from_this();
                            case 2:
                                return n.new_boolean(0.0 > d);
                            case 3:
                                return shared_from_this();
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_GREATER_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 >= JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(true);
                            case 2:
                                return n.new_boolean(0.0 >= d);
                            case 3:
                                return shared_from_this();
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
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_FEQUAL:
                switch (op->get_type()) {
                    case r_boolean:
                        return n.new_boolean(!JS_BOOL(op));
                    case r_number:
                    case r_string:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return shared_from_this();
                    default:
                        break;
                }
                break;
            case COMPARE_FNOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_false(n, COMPARE_FEQUAL, op)));
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (std::isnan(s))
                            return n.new_number(NAN);
                        if (std::isinf(s))
                            return n.new_number(std::signbit(s) == 0 ? 0.0 : INFINITY);
                        if (s == 0.0)
                            return n.new_number(1.0);
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
                                if (d > 0.0)
                                    return n.new_number(0.0);
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
                                if (std::isinf(d) || std::isnan(d))
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
                        return n.new_string(_str_f + JS_STR(op));
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(1.0) :
                               n.new_number(0.0);
                    case r_object:
                        return n.new_string(_str_f + jsv_object::_str);
                    case r_function: {
                        const auto &s = JS_STRF(op);
                        return n.new_string(_str_f + s);
                    }
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
                                return n.new_number(std::signbit(d) == 0 ? 0.0 : -0.0);
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
                        const auto &s = JS_NUM(op);
                        auto d = fix(s);
                        if (d == 0.0)
                            return n.new_number(0.0);
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

    bool jsv_boolean::to_bool() const {
        return b;
    }

    void jsv_boolean::mark(int n) {
    }

    void jsv_boolean::print(std::ostream &os) const {
        os << std::boolalpha << b;
    }

    std::string jsv_boolean::to_string() const {
        return b ? _str_t : _str_f;
    }
}