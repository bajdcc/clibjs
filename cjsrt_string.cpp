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

    js_value::ref jsv_string::clone() const {
        return std::make_shared<jsv_string>(str);
    }

    runtime_t jsv_string::get_type() {
        return r_string;
    }

    js_value::ref jsv_string::binary_op(js_value_new &n, int code, js_value::ref op) {
        if (code == BINARY_ADD) {
            return n.new_string(to_string() + op->to_string());
        }
        if (!calc_number)
            calc();
        if (number_state != 2) {
            switch (code) {
                case BINARY_POWER:
                    switch (op->get_type()) {
                        case r_number: {
                            const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                            if (s == 0.0)
                                return n.new_number(1.0);
                            if (s == 1.0)
                                return n.new_number(number);
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
                                    return n.new_number(number);
                                return n.new_number(pow(number, d));
                            }
                            return n.new_number(NAN);
                        }
                        case r_boolean:
                            return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                                    return n.new_number(number);
                                if (d == 0.0)
                                    return n.new_number(std::signbit(number) == 0 ? 0.0 : -0.0);
                                if (number == 1.0)
                                    return op;
                                if (d == 1.0)
                                    return n.new_number(number);
                                return n.new_number(number * d);
                            }
                            return n.new_number(NAN);
                        }
                        case r_boolean:
                            return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                                   n.new_number(number) :
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
                                return n.new_number(number);
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
                                    return n.new_number(number);
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
                case BINARY_SUBTRACT:
                    switch (op->get_type()) {
                        case r_number: {
                            const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                            return n.new_number(number - s);
                        }
                        case r_string: {
                            const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                            if (s.empty())
                                return n.new_number(number);
                            std::stringstream ss;
                            ss << trim(s);
                            if (ss.str().empty())
                                return n.new_number(number);
                            double d;
                            ss >> d;
                            if (ss.eof() && !ss.fail()) {
                                if (d == 0.0)
                                    return n.new_number(number);
                                return n.new_number(number - d);
                            }
                            return n.new_number(NAN);
                        }
                        case r_boolean:
                            return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(number);
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
                                    return number == 0.0 ? n.new_number(0.0) : n.new_number(number);
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
                                    return n.new_number(number);
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
                                    return n.new_number(number);
                                auto a = uint32_t(fix(number));
                                auto b = uint32_t(fix(d));
                                return n.new_number(double(int(a | b)));
                            }
                            return n.new_number(fix(number) == 0.0 ? 0.0 : fix(number));
                        }
                        case r_boolean: {
                            if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
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
                case BINARY_POWER:
                    switch (op->get_type()) {
                        case r_number: {
                            const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                            if (s == 0.0)
                                return n.new_number(1.0);
                            return n.new_number(NAN);
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
                                return n.new_number(NAN);
                            }
                            return n.new_number(NAN);
                        }
                        case r_boolean:
                            return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                            const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                            return n.new_number(fix(s) == 0.0 ? 0.0 : fix(s));
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
                                return n.new_number(fix(d) == 0.0 ? 0.0 : fix(d));
                            }
                            return n.new_number(0.0);
                        }
                        case r_boolean:
                            return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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

    void jsv_string::calc() {
        // state=0  empty
        // state=1  normal
        // state=2  parse error
        calc_number = true;
        if (str.empty()) {
            return;
        }
        std::stringstream ss;
        ss << trim(str);
        if (ss.str().empty())
            return;
        ss >> number;
        if (ss.eof() && !ss.fail()) {
            number_state = 1;
        } else {
            number_state = 2;
        }
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
}