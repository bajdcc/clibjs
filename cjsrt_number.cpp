//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include <cfenv>
#include <cstring>
#include <cassert>
#include "cjsruntime.h"

#define MAX_SAFE_INTEGER ((int64_t)((1ULL << 53U) - 1))

namespace clib {

    jsv_number::jsv_number(double n) : number(n) {

    }

    runtime_t jsv_number::get_type() {
        return r_number;
    }

    js_value::ref jsv_number::binary_op(js_value_new &n, int code, const js_value::ref &op) {
        switch (code) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(number < JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(number < 0.0);
                            case 2:
                                return n.new_boolean(number < d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(number < (JS_BOOL(op) ? 1.0 : 0.0));
                    case r_null:
                        return n.new_boolean(number < 0.0);
                    case r_object:
                    case r_function:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_LESS_EQUAL:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(number <= JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(number <= 0.0);
                            case 2:
                                return n.new_boolean(number <= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(number <= (JS_BOOL(op) ? 1.0 : 0.0));
                    case r_null:
                        return n.new_boolean(number <= 0.0);
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
                        return n.new_boolean(number == JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(number == 0.0);
                            case 2:
                                return n.new_boolean(number == d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(number == (JS_BOOL(op) ? 1.0 : 0.0));
                    case r_null:
                        return n.new_boolean(false);
                    case r_object:
                    case r_function:
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
                    case r_number:
                        return n.new_boolean(number > JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(number > 0.0);
                            case 2:
                                return n.new_boolean(number > d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(number > (JS_BOOL(op) ? 1.0 : 0.0));
                    case r_null:
                        return n.new_boolean(number > 0.0);
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
                        return n.new_boolean(number >= JS_NUM(op));
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_boolean(number >= 0.0);
                            case 2:
                                return n.new_boolean(number >= d);
                            case 3:
                                return n.new_boolean(false);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return n.new_boolean(number >= (JS_BOOL(op) ? 1.0 : 0.0));
                    case r_null:
                        return n.new_boolean(number >= 0.0);
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
                    case r_number:
                        return n.new_boolean(number == JS_NUM(op));
                    case r_null:
                    case r_boolean:
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
                        if (s == 1.0)
                            return shared_from_this();
                        if ((number == 1.0 || number == -1.0) && std::isinf(s))
                            return n.new_number(NAN);
                        return n.new_number(pow(number, s));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(1.0);
                            case 2:
                                if ((number == 1.0 || number == -1.0) && std::isinf(d))
                                    return n.new_number(NAN);
                                if (d == 0.0)
                                    return n.new_number(1.0);
                                if (d == 1.0)
                                    return shared_from_this();
                                return n.new_number(pow(number, d));
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               shared_from_this() :
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
                        if (number == 0.0 && (std::isinf(s) || std::isnan(s)))
                            return n.new_number(NAN);
                        if (std::isnan(number))
                            return n.new_number(NAN);
                        return n.new_number(number * s);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                if (std::isinf(number) || std::isnan(number)) {
                                    return n.new_number(NAN);
                                }
                                return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                            case 2:
                                if (number == 0.0)
                                    return std::isinf(d) ? n.new_number(NAN) : shared_from_this();
                                if (number == 1.0)
                                    return op;
                                if (d == 0.0)
                                    return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                                if (d == 1.0)
                                    return shared_from_this();
                                return n.new_number(number * d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        if ((std::isinf(number) || std::isnan(number)) &&
                            !JS_BOOL(op)) {
                            return n.new_number(NAN);
                        }
                        return JS_BOOL(op) ?
                               shared_from_this() :
                               n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        if (std::isinf(number) || std::isnan(number)) {
                            return n.new_number(NAN);
                        }
                        return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
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
                        if (std::isinf(number)) {
                            return n.new_number(NAN);
                        }
                        if (s == 0.0)
                            return n.new_number(NAN);
                        if (std::isinf(s))
                            return shared_from_this();
                        if (number == 0.0)
                            return std::isnan(s) ? n.new_number(NAN) : shared_from_this();
                        return n.new_number(fmod(number, s));
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
                                if (std::isinf(number) && std::isinf(d))
                                    return n.new_number(NAN);
                                if (number == 0.0 || std::isinf(d))
                                    return shared_from_this();
                                return n.new_number(fmod(number, d));
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        if (JS_BOOL(op)) {
                            if (number == 0.0)
                                return n.new_number(number);
                            return n.new_number(fmod(number, 1.0));
                        } else {
                            return n.new_number(NAN);
                        }
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
                        return n.new_number(number + s);
                    }
                    case r_string:
                        return n.new_string(to_string() + op->to_string());
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(number + 1.0) :
                               (number == 0.0 ? n.new_number(0.0) : shared_from_this());
                    case r_object:
                        return n.new_string(to_string() + op->to_string());
                    case r_function:
                        return n.new_string(to_string() + op->to_string());
                    case r_null:
                        return number == 0.0 ?
                               n.new_number(0.0) : n.new_number(number);
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
                        return n.new_number(number - s);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return shared_from_this();
                            case 2:
                                if (d == 0.0)
                                    return shared_from_this();
                                return n.new_number(number - d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        return JS_BOOL(op) ?
                               n.new_number(
                                       number - 1.0) :
                               shared_from_this();
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        return shared_from_this();
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
                        if (s == 0.0 && number == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(number / s);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                if (std::isnan(number))
                                    return shared_from_this();
                                return number == 0.0 ? n.new_number(NAN) :
                                       n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
                            case 2:
                                if (d == 0.0)
                                    return number == 0.0 ? n.new_number(NAN) :
                                           n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
                                return n.new_number(number / d);
                            case 3:
                                return n.new_number(NAN);
                            default:
                                break;
                        }
                    }
                    case r_boolean:
                        if (std::isnan(number) && !JS_BOOL(op))
                            return n.new_number(NAN);
                        if (std::isinf(number))
                            return shared_from_this();
                        return JS_BOOL(op) ?
                               shared_from_this() :
                               (number == 0.0 ? n.new_number(NAN) :
                                n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY));
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
                        if (std::isnan(number))
                            return shared_from_this();
                        return number == 0.0 ? n.new_number(NAN) :
                               n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
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
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a << c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            case 2: {
                                if (d == 0.0)
                                    return number == 0.0 ? n.new_number(0.0) : shared_from_this();
                                auto a = uint32_t(fix(number));
                                auto b = fix(d);
                                auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                                return n.new_number(double(int(a << c)));
                            }
                            case 3:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        if (!JS_BOOL(op))
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a << 1U)));
                    }
                    case r_object:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_function:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_null:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_undefined:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    default:
                        break;
                }
                break;
            case BINARY_RSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = int(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a >> c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            case 2: {
                                if (d == 0.0)
                                    return number == 0.0 ? n.new_number(0.0) : shared_from_this();
                                auto a = int(fix(number));
                                auto b = fix(d);
                                auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                                return n.new_number(double(int(a >> c)));
                            }
                            case 3:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        if (!JS_BOOL(op))
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = int(fix(number));
                        return n.new_number(double(int(a >> 1U)));
                    }
                    case r_object:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_function:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_null:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_undefined:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    default:
                        break;
                }
                break;
            case BINARY_URSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : uint32_t(fix(number)));
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(uint32_t(a >> c)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(double(uint32_t(fix(number))));
                            case 2: {
                                auto a = uint32_t(fix(number));
                                if (d == 0.0)
                                    return n.new_number(double(a));
                                auto b = fix(double(d));
                                auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                                return n.new_number(double(uint32_t(a >> c)));
                            }
                            case 3:
                                return n.new_number(double(uint32_t(fix(number))));
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        auto a = uint32_t(fix(number));
                        if (!JS_BOOL(op))
                            return n.new_number(double(a));
                        return n.new_number(double(uint32_t(a >> 1U)));
                    }
                    case r_object:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(uint32_t(fix(number)));
                    case r_function:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(uint32_t(fix(number)));
                    case r_null:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(uint32_t(fix(number)));
                    case r_undefined:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(uint32_t(fix(number)));
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
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a & b)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2: {
                                if (d == 0.0)
                                    return shared_from_this();
                                auto a = uint32_t(fix(number));
                                auto b = uint32_t(fix(d));
                                return n.new_number(double(int(a & b)));
                            }
                            case 3:
                                return n.new_number(0.0);
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        if (!JS_BOOL(op))
                            return n.new_number(0.0);
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a & 1U)));
                    }
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
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a ^ b)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            case 2: {
                                if (d == 0.0)
                                    return n.new_number(0.0);
                                auto a = uint32_t(fix(number));
                                auto b = uint32_t(fix(d));
                                return n.new_number(double(int(a ^ b)));
                            }
                            case 3:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        if (!JS_BOOL(op))
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a ^ 1U)));
                    }
                    case r_object:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_function:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_null:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_undefined:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    default:
                        break;
                }
                break;
            case BINARY_OR:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a | b)));
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            case 2: {
                                if (d == 0.0)
                                    return shared_from_this();
                                auto a = uint32_t(fix(number));
                                auto b = uint32_t(fix(d));
                                return n.new_number(double(int(a | b)));
                            }
                            case 3:
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                            default:
                                break;
                        }
                    }
                    case r_boolean: {
                        if (!JS_BOOL(op))
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a | 1U)));
                    }
                    case r_object:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_function:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_null:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_undefined:
                        return fix(number) == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    default:
                        break;
                }
                break;
            default:
                break;
        }
        return nullptr;
    }

    js_value::ref jsv_number::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE:
                return shared_from_this();
            case UNARY_NEGATIVE:
                return n.new_number(-number);
            case UNARY_NOT:
                return n.new_boolean(number == 0.0 || std::isnan(number));
            case UNARY_INVERT:
                return n.new_number((double) (int) ~(uint32_t) fix(number));
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("number");
            default:
                break;
        }
        return nullptr;
    }

    bool jsv_number::to_bool() const {
        if (std::isnan(number)) return false;
        return number != 0.0;
    }

    void jsv_number::mark(int n) {
        marked = n;
    }

    /* 2 <= base <= 36 */
    static char *i64toa(char *buf_end, int64_t n, unsigned int base) {
        auto q = buf_end;
        uint64_t digit;
        int is_neg;

        is_neg = 0;
        if (n < 0) {
            is_neg = 1;
            n = -n;
        }
        *--q = '\0';
        do {
            digit = ((uint64_t) n) % base;
            n = (uint64_t) n / base;
            if (digit < 10)
                digit += '0';
            else
                digit += 'a' - 10;
            *--q = digit;
        } while (n != 0);
        if (is_neg)
            *--q = '-';
        return q;
    }

    /* buf1 contains the printf result */
    static void js_ecvt1(double d, int n_digits, int *decpt, int *sign, char *buf,
                         int rounding_mode, char *buf1, int buf1_size) {
        if (rounding_mode != FE_TONEAREST)
            fesetround(rounding_mode);
        snprintf(buf1, buf1_size, "%+.*e", n_digits - 1, d);
        if (rounding_mode != FE_TONEAREST)
            fesetround(FE_TONEAREST);
        *sign = (buf1[0] == '-');
        /* mantissa */
        buf[0] = buf1[1];
        if (n_digits > 1)
            memcpy(buf + 1, buf1 + 3, n_digits - 1);
        buf[n_digits] = '\0';
        /* exponent */
        *decpt = atoi(buf1 + n_digits + 2 + (n_digits > 1)) + 1;
    }

/* maximum buffer size for js_dtoa */
#define JS_DTOA_BUF_SIZE 128

/* needed because ecvt usually limits the number of digits to
   17. Return the number of digits. */
    static int js_ecvt(double d, int n_digits, int *decpt, int *sign, char *buf, bool is_fixed) {
        int rounding_mode;
        char buf_tmp[JS_DTOA_BUF_SIZE];

        if (!is_fixed) {
            /* find the minimum amount of digits (XXX: inefficient but simple) */
            auto n_digits_min = 1;
            auto n_digits_max = 17;
            while (n_digits_min < n_digits_max) {
                n_digits = (n_digits_min + n_digits_max) / 2;
                js_ecvt1(d, n_digits, decpt, sign, buf, FE_TONEAREST,
                         buf_tmp, sizeof(buf_tmp));
                if (strtod(buf_tmp, nullptr) == d) {
                    /* no need to keep the trailing zeros */
                    while (n_digits >= 2 && buf[n_digits - 1] == '0')
                        n_digits--;
                    n_digits_max = n_digits;
                } else {
                    n_digits_min = n_digits + 1;
                }
            }
            n_digits = n_digits_max;
            rounding_mode = FE_TONEAREST;
        } else {
            rounding_mode = FE_TONEAREST;
            char buf1[JS_DTOA_BUF_SIZE], buf2[JS_DTOA_BUF_SIZE];
            int decpt1, sign1, decpt2, sign2;
            /* The JS rounding is specified as round to nearest ties away
               from zero (RNDNA), but in printf the "ties" case is not
               specified (for example it is RNDN for glibc, RNDNA for
               Windows), so we must round manually. */
            js_ecvt1(d, n_digits + 1, &decpt1, &sign1, buf1, FE_TONEAREST,
                     buf_tmp, sizeof(buf_tmp));
            /* XXX: could use 2 digits to reduce the average running time */
            if (buf1[n_digits] == '5') {
                js_ecvt1(d, n_digits + 1, &decpt1, &sign1, buf1, FE_DOWNWARD,
                         buf_tmp, sizeof(buf_tmp));
                js_ecvt1(d, n_digits + 1, &decpt2, &sign2, buf2, FE_UPWARD,
                         buf_tmp, sizeof(buf_tmp));
                if (memcmp(buf1, buf2, n_digits + 1) == 0 && decpt1 == decpt2) {
                    /* exact result: round away from zero */
                    if (sign1)
                        rounding_mode = FE_DOWNWARD;
                    else
                        rounding_mode = FE_UPWARD;
                }
            }
        }
        js_ecvt1(d, n_digits, decpt, sign, buf, rounding_mode,
                 buf_tmp, sizeof(buf_tmp));
        return n_digits;
    }

    static int js_fcvt1(char *buf, int buf_size, double d, int n_digits,
                        int rounding_mode) {
        int n;
        if (rounding_mode != FE_TONEAREST)
            fesetround(rounding_mode);
        n = snprintf(buf, buf_size, "%.*f", n_digits, d);
        if (rounding_mode != FE_TONEAREST)
            fesetround(FE_TONEAREST);
        assert(n < buf_size);
        return n;
    }

    static void js_fcvt(char *buf, int buf_size, double d, int n_digits) {
        int rounding_mode;
        rounding_mode = FE_TONEAREST;
        int n1, n2;
        char buf1[JS_DTOA_BUF_SIZE];
        char buf2[JS_DTOA_BUF_SIZE];

        /* The JS rounding is specified as round to nearest ties away from
           zero (RNDNA), but in printf the "ties" case is not specified
           (for example it is RNDN for glibc, RNDNA for Windows), so we
           must round manually. */
        n1 = js_fcvt1(buf1, sizeof(buf1), d, n_digits + 1, FE_TONEAREST);
        rounding_mode = FE_TONEAREST;
        /* XXX: could use 2 digits to reduce the average running time */
        if (buf1[n1 - 1] == '5') {
            n1 = js_fcvt1(buf1, sizeof(buf1), d, n_digits + 1, FE_DOWNWARD);
            n2 = js_fcvt1(buf2, sizeof(buf2), d, n_digits + 1, FE_UPWARD);
            if (n1 == n2 && memcmp(buf1, buf2, n1) == 0) {
                /* exact result: round away from zero */
                if (buf1[0] == '-')
                    rounding_mode = FE_DOWNWARD;
                else
                    rounding_mode = FE_UPWARD;
            }
        }
        js_fcvt1(buf, buf_size, d, n_digits, rounding_mode);
    }

    // Refer: quickjs
    void jsv_number::print(std::ostream &os) {
        os << number_to_string(number);
    }

    std::string jsv_number::to_string() const {
        if (number == 0.0)
            return "0";
        return number_to_string(number);
    }

    std::string jsv_number::number_to_string(double number) {
        if (std::isnan(number)) {
            return "NaN";
        }
        if (std::isinf(number)) {
            return ((std::signbit(number) == 0) ? "Infinity" : "-Infinity");
        }
        if (number == 0) {
            return ((std::signbit(number) == 0) ? "0" : "-0");
        }
        auto i64 = (int64_t) number;
        char buf1[70];
        if (number != i64 || i64 > MAX_SAFE_INTEGER || i64 < -MAX_SAFE_INTEGER) {
            int sign, decpt, k, n, i, p, n_max;
            n_max = 21;
            /* the number has k digits (k >= 1) */
            k = js_ecvt(number, 0, &decpt, &sign, buf1, false);
            n = decpt; /* d=10^(n-k)*(buf1) i.e. d= < x.yyyy 10^(n-1) */
            char buf[JS_DTOA_BUF_SIZE];
            auto q = buf;
            if (sign)
                *q++ = '-';
            if (n >= 1 && n <= n_max) {
                if (k <= n) {
                    memcpy(q, buf1, k);
                    q += k;
                    for (i = 0; i < (n - k); i++)
                        *q++ = '0';
                    *q = '\0';
                } else {
                    /* k > n */
                    memcpy(q, buf1, n);
                    q += n;
                    *q++ = '.';
                    for (i = 0; i < (k - n); i++)
                        *q++ = buf1[n + i];
                    *q = '\0';
                }
            } else if (n >= -5 && n <= 0) {
                *q++ = '0';
                *q++ = '.';
                for (i = 0; i < -n; i++)
                    *q++ = '0';
                memcpy(q, buf1, k);
                q += k;
                *q = '\0';
            } else {
                force_exp:
                /* exponential notation */
                *q++ = buf1[0];
                if (k > 1) {
                    *q++ = '.';
                    for (i = 1; i < k; i++)
                        *q++ = buf1[i];
                }
                *q++ = 'e';
                p = n - 1;
                if (p >= 0)
                    *q++ = '+';
                sprintf(q, "%d", p);
            }
            return buf;
        } else {
            return i64toa(buf1 + sizeof(buf1), i64, 10);
        }
    }
}