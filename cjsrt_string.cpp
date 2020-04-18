//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

namespace clib {

    jsv_string::jsv_string(std::string s) : str(std::move(s)) {

    }

    runtime_t jsv_string::get_type() {
        return r_string;
    }

    js_value::ref jsv_string::binary_op(js_value_new &n, int code, const js_value::ref &op) {
        if (code == BINARY_ADD) {
            return n.new_string(to_string() + op->to_string());
        }
        if (op->get_type() == r_string) {
            switch (code) {
                case COMPARE_LESS:
                    return n.new_boolean(str < JS_STR(op));
                case COMPARE_LESS_EQUAL:
                    return n.new_boolean(str <= JS_STR(op));
                case COMPARE_EQUAL:
                    return n.new_boolean(str == JS_STR(op));
                case COMPARE_NOT_EQUAL:
                    return n.new_boolean(str != JS_STR(op));
                case COMPARE_GREATER:
                    return n.new_boolean(str > JS_STR(op));
                case COMPARE_GREATER_EQUAL:
                    return n.new_boolean(str >= JS_STR(op));
                case COMPARE_FEQUAL:
                    return n.new_boolean(str == JS_STR(op));
                case COMPARE_FNOT_EQUAL:
                    return n.new_boolean(str != JS_STR(op));
                default:
                    break;
            }
        }
        if (!calc_number)
            calc();
        if (number_state != 3) {
            switch (code) {
                case COMPARE_LESS:
                    switch (op->get_type()) {
                        case r_number:
                            return n.new_boolean(number < JS_NUM(op));
                        case r_boolean:
                            return n.new_boolean(number < (JS_BOOL(op) ? 1.0 : 0.0));
                        case r_object:
                        case r_function:
                            return n.new_boolean(str < op->to_string());
                        case r_null:
                            return n.new_boolean(number < 0.0);
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
                        case r_boolean:
                            return n.new_boolean(number <= (JS_BOOL(op) ? 1.0 : 0.0));
                        case r_object:
                        case r_function:
                            return n.new_boolean(str <= op->to_string());
                        case r_null:
                            return n.new_boolean(number <= 0.0);
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
                        case r_boolean:
                            return n.new_boolean(number == (JS_BOOL(op) ? 1.0 : 0.0));
                        case r_null:
                            return n.new_boolean(false);
                        case r_object:
                        case r_function:
                            return n.new_boolean(str == op->to_string());
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
                        case r_boolean:
                            return n.new_boolean(number > (JS_BOOL(op) ? 1.0 : 0.0));
                        case r_object:
                        case r_function:
                            return n.new_boolean(str > op->to_string());
                        case r_null:
                            return n.new_boolean(number > 0.0);
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
                        case r_boolean:
                            return n.new_boolean(number >= (JS_BOOL(op) ? 1.0 : 0.0));
                        case r_object:
                        case r_function:
                            return n.new_boolean(str >= op->to_string());
                        case r_null:
                            return n.new_boolean(number >= 0.0);
                        case r_undefined:
                            return n.new_boolean(false);
                        default:
                            break;
                    }
                    break;
                case COMPARE_FEQUAL:
                    switch (op->get_type()) {
                        case r_null:
                        case r_boolean:
                        case r_number:
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
                                return n.new_number(number);
                            return n.new_number(pow(number, s));
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
                                    if (d == 1.0)
                                        return n.new_number(number);
                                    return n.new_number(pow(number, d));
                                case 3:
                                    return n.new_number(NAN);
                                default:
                                    break;
                            }
                        }
                        case r_boolean:
                            return JS_BOOL(op) ?
                                   n.new_number(number) :
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
                            return n.new_number(number * s);
                        }
                        case r_string: {
                            double d;
                            switch (JS_STR2NUM(op, d)) {
                                case 0:
                                case 1:
                                    if (std::isinf(number)) {
                                        return n.new_number(NAN);
                                    }
                                    return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                                case 2:
                                    if (std::isinf(number) && std::isinf(d))
                                        return n.new_number(number * d);
                                    if (number == 0.0)
                                        return n.new_number(std::isinf(d) ? NAN : number);
                                    if (number == 1.0)
                                        return op;
                                    if (d == 0.0)
                                        return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                                    return n.new_number(number * d);
                                case 3:
                                    return n.new_number(NAN);
                                default:
                                    break;
                            }
                        }
                        case r_boolean:
                            if (std::isinf(number) && !JS_BOOL(op)) {
                                return n.new_number(NAN);
                            }
                            return JS_BOOL(op) ?
                                   n.new_number(number) :
                                   n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                        case r_object:
                            return n.new_number(NAN);
                        case r_function:
                            return n.new_number(NAN);
                        case r_null:
                            if (std::isinf(number)) {
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
                            if (std::isinf(number) || std::isnan(s))
                                return n.new_number(NAN);
                            if (s == 0.0)
                                return n.new_number(NAN);
                            if (number == 0.0 || std::isinf(s))
                                return n.new_number(number);
                            return n.new_number(fmod(number, s));
                        }
                        case r_string: {
                            double d;
                            switch (JS_STR2NUM(op, d)) {
                                case 0:
                                case 1:
                                    if (std::isinf(d))
                                        return n.new_number(0.0);
                                    return n.new_number(NAN);
                                case 2:
                                    if (number == 0.0)
                                        return std::isinf(d) ? n.new_number(0.0) : n.new_number(number);
                                    if (std::isinf(d))
                                        return std::isinf(number) ? n.new_number(NAN) : n.new_number(number);
                                    if (d == 0.0)
                                        return n.new_number(NAN);
                                    return n.new_number(fmod(number, d));
                                case 3:
                                    return n.new_number(NAN);
                                default:
                                    break;
                            }
                        }
                        case r_boolean:
                            if (std::isinf(number) && JS_BOOL(op)) {
                                return n.new_number(NAN);
                            }
                            return JS_BOOL(op) ?
                                   n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0) :
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
                                    return n.new_number(number);
                                case 2:
                                    if (d == 0.0)
                                        return n.new_number(number);
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
                                   n.new_number(number);
                        case r_object:
                            return n.new_number(NAN);
                        case r_function:
                            return n.new_number(NAN);
                        case r_null:
                            return n.new_number(number);
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
                            return JS_BOOL(op) ?
                                   n.new_number(number) :
                                   (number == 0.0 ? n.new_number(NAN) :
                                    n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY));
                        case r_object:
                            return n.new_number(NAN);
                        case r_function:
                            return n.new_number(NAN);
                        case r_null:
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
                                        return number == 0.0 ? n.new_number(0.0) : n.new_number(number);
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
                                return number == 0 ?
                                       n.new_number(0.0) : n.new_number(fix(number));
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
                                        return number == 0.0 ? n.new_number(0.0) : n.new_number(number);
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
                                return number == 0 ?
                                       n.new_number(0.0) : n.new_number(fix(number));
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
                                return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
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
                                    auto b = fix(d);
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
                                        return n.new_number(number);
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
                                return n.new_number(number == 0.0 ? number : fix(number));
                            auto a = uint32_t(fix(number));
                            auto b = uint32_t(fix(s));
                            return n.new_number(double(int(a ^ b)));
                        }
                        case r_string: {
                            double d;
                            switch (JS_STR2NUM(op, d)) {
                                case 0:
                                case 1:
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                                case 2: {
                                    if (d == 0.0)
                                        return n.new_number(0.0);
                                    auto a = uint32_t(fix(number));
                                    auto b = uint32_t(fix(d));
                                    return n.new_number(double(int(a ^ b)));
                                }
                                case 3:
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                                default:
                                    break;
                            }
                        }
                        case r_boolean: {
                            if (std::isinf(number))
                                return n.new_number(JS_BOOL(op));
                            if (!JS_BOOL(op))
                                return number == 0 ?
                                       n.new_number(0.0) : n.new_number(number);
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
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                                case 2: {
                                    if (d == 0.0)
                                        return n.new_number(number);
                                    auto a = uint32_t(fix(number));
                                    auto b = uint32_t(fix(d));
                                    return n.new_number(double(int(a | b)));
                                }
                                case 3:
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                                default:
                                    break;
                            }
                        }
                        case r_boolean: {
                            if (std::isinf(number))
                                return n.new_number(JS_BOOL(op));
                            if (!JS_BOOL(op))
                                return number == 0 ?
                                       n.new_number(0.0) : n.new_number(number);
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
        } else {
            switch (code) {
                case COMPARE_LESS:
                    switch (op->get_type()) {
                        case r_object:
                        case r_function:
                            return n.new_boolean(str < op->to_string());
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
                        case r_object:
                        case r_function:
                            return n.new_boolean(str <= op->to_string());
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
                        case r_object:
                        case r_function:
                            return n.new_boolean(str == op->to_string());
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
                        case r_object:
                        case r_function:
                            return n.new_boolean(str > op->to_string());
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
                        case r_object:
                        case r_function:
                            return n.new_boolean(str > op->to_string());
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
                        case r_boolean:
                        case r_number:
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
                                case 2: {
                                    if (d == 0.0)
                                        return n.new_number(1.0);
                                    return n.new_number(NAN);
                                }
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
                    return n.new_number(NAN);
                case BINARY_MODULO:
                    return n.new_number(NAN);
                case BINARY_SUBTRACT:
                    return n.new_number(NAN);
                case BINARY_FLOOR_DIVIDE:
                    break;
                case BINARY_TRUE_DIVIDE:
                    return n.new_number(NAN);
                case BINARY_LSHIFT:
                    return n.new_number(0.0);
                case BINARY_RSHIFT:
                    return n.new_number(0.0);
                case BINARY_URSHIFT:
                    return n.new_number(0.0);
                case BINARY_AND:
                    return n.new_number(0.0);
                case BINARY_XOR:
                case BINARY_OR:
                    switch (op->get_type()) {
                        case r_number: {
                            const auto &s = JS_NUM(op);
                            if (s == 0.0 || std::isnan(s) || std::isinf(s))
                                return n.new_number(0.0);
                            return n.new_number(fix(s) == 0.0 ? 0.0 : fix(s));
                        }
                        case r_string: {
                            double d;
                            switch (JS_STR2NUM(op, d)) {
                                case 0:
                                case 1:
                                    return n.new_number(0.0);
                                case 2:
                                    return n.new_number(fix(d) == 0.0 ? 0.0 : fix(d));
                                case 3:
                                    return n.new_number(0.0);
                                default:
                                    break;
                            }
                        }
                        case r_boolean:
                            return JS_BOOL(op) ?
                                   n.new_number(1.0) : n.new_number(0.0);
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
                default:
                    break;
            }
        }
        return nullptr;
    }

    js_value::ref jsv_string::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE: {
                if (!calc_number)
                    calc();
                if (number_state <= 1)
                    return n.new_number(0.0);
                if (number_state == 2)
                    return n.new_number(number);
                return n.new_number(NAN);
            }
            case UNARY_NEGATIVE: {
                if (!calc_number)
                    calc();
                if (number_state <= 1)
                    return n.new_number(-0.0);
                if (number_state == 2)
                    return n.new_number(-number);
                return n.new_number(NAN);
            }
            case UNARY_NOT:
                return n.new_boolean(str.empty());
            case UNARY_INVERT: {
                if (!calc_number)
                    calc();
                if (number_state == 2)
                    return n.new_number((double) (int) ~(uint32_t) fix(number));
                return n.new_number(-1.0);
            }
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("string");
            default:
                break;
        }
        return nullptr;
    }

    void jsv_string::calc() {
        number_state = to_number(number);
    }

    bool jsv_string::to_bool() const {
        return !str.empty();
    }

    void jsv_string::mark(int n) {
        marked = n;
    }

    void jsv_string::print(std::ostream &os) {
        os << str;
    }

    std::string jsv_string::to_string() const {
        return str;
    }

    jsv_string::ref jsv_string::clear() {
        if (calc_number) {
            number = 0.0;
            number_state = 0;
            calc_number = false;
        }
        return std::dynamic_pointer_cast<jsv_string>(shared_from_this());
    }

    int jsv_string::to_number(double &d) const {
    }

    int jsv_string::to_number(const std::string &s, double &d) {
        // 0: empty
        // 1: trim -> empty
        // 2: number
        // 3: error
        if (s.empty())
            return 0;
        auto t = trim(s);
        if (t == "Infinity") {
            d = INFINITY;
            return 2;
        }
        if (t == "-Infinity") {
            d = -INFINITY;
            return 2;
        }
        std::stringstream ss;
        ss << t;
        if (ss.str().empty())
            return 1;
        ss >> d;
        if (ss.eof() && !ss.fail()) {
            return 2;
        }
        return 3;
    }
}