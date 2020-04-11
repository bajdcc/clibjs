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

    js_value::ref jsv_number::clone() const {
        return std::make_shared<jsv_number>(number);
    }

    runtime_t jsv_number::get_type() {
        return r_number;
    }

    js_value::ref jsv_number::binary_op(js_value_new &n, int code, js_value::ref op) {
        switch (code) {
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(1.0);
                        if (s == 1.0)
                            return shared_from_this();
                        return n.new_number(pow(number, s));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(1.0);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(1.0);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(1.0);
                            if (d == 1.0)
                                return shared_from_this();
                            return n.new_number(pow(number, d));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        return n.new_number(number * s);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (number == 0.0)
                                return shared_from_this();
                            if (d == 0.0)
                                return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                            if (number == 1.0)
                                return op;
                            if (d == 1.0)
                                return shared_from_this();
                            return n.new_number(number * d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               shared_from_this() :
                               n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                    case r_object:
                        return n.new_number(NAN);
                    case r_function:
                        return n.new_number(NAN);
                    case r_null:
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(NAN);
                        if (number == 0.0)
                            return shared_from_this();
                        return n.new_number(fmod(number, s));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(NAN);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(NAN);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(NAN);
                            if (number == 0.0)
                                return shared_from_this();
                            return n.new_number(fmod(number, d));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
            case BINARY_ADD:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        return n.new_number(number + s);
                    }
                    case r_string:
                        return n.new_string(to_string() + op->to_string());
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(number + 1.0) :
                               (number == 0.0 ? n.new_number(0.0) : shared_from_this());
                    case r_object:
                        return n.new_string(to_string() + op->to_string());
                    case r_function:
                        return n.new_string(to_string() + op->to_string());
                    case r_null:
                        return number == 0.0 ?
                               n.new_number(0.0) : n.new_number(fix(number));
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_SUBTRACT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        return n.new_number(number - s);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return shared_from_this();
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            return n.new_number(number - d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0 && number == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(number / s);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return number == 0.0 ? n.new_number(NAN) :
                                   n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return number == 0.0 ? n.new_number(NAN) :
                                   n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return number == 0.0 ? n.new_number(NAN) :
                                       n.new_number(std::signbit(number) == 0 ? INFINITY : -INFINITY);
                            return n.new_number(number / d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               shared_from_this() :
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a << c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return number == 0.0 ? n.new_number(0.0) : shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a << c)));
                        }
                        return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = int(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a >> c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return number == 0.0 ? n.new_number(0.0) : shared_from_this();
                            auto a = int(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a >> c)));
                        }
                        return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(uint32_t(a >> c)));
                    }
                    case r_string: {
                        double d = 0.0;
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        std::stringstream ss;
                        if (!s.empty()) {
                            ss << trim(s);
                            if (!ss.str().empty()) {
                                ss >> d;
                                if (!(ss.eof() && !ss.fail())) {
                                    d = 0.0;
                                }
                            }
                        }
                        auto a = uint32_t(fix(number));
                        if (d == 0.0)
                            return n.new_number(double(a));
                        auto b = fix(d);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(uint32_t(a >> c)));
                    }
                    case r_boolean: {
                        auto a = uint32_t(fix(number));
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(0.0);
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a & b)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(0.0);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(0.0);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = uint32_t(fix(d));
                            return n.new_number(double(int(a & b)));
                        }
                        return n.new_number(0.0);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(number == 0.0 ? number : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a ^ b)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(0.0);
                            auto a = uint32_t(fix(number));
                            auto b = uint32_t(fix(d));
                            return n.new_number(double(int(a ^ b)));
                        }
                        return number == 0.0 ? n.new_number(0.0) : n.new_number(fix(number));
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return number == 0 ?
                                   n.new_number(0.0) : shared_from_this();
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        auto a = uint32_t(fix(number));
                        auto b = uint32_t(fix(s));
                        return n.new_number(double(int(a | b)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = uint32_t(fix(d));
                            return n.new_number(double(int(a | b)));
                        }
                        return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return number == 0 ?
                                   n.new_number(0.0) : shared_from_this();
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