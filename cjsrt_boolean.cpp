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

    js_value::ref jsv_boolean::clone() const {
        return std::make_shared<jsv_boolean>(b);
    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    js_value::ref jsv_boolean::binary_op(js_value_new &n, int code, js_value::ref op) {
        return b ? binary_true(n, code, std::move(op)) : binary_false(n, code, std::move(op));
    }

    js_value::ref jsv_boolean::binary_true(js_value_new &n, int code, js_value::ref op) {
        switch (code) {
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_number(1.0);
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
                            return n.new_number(1.0);
                        }
                        return n.new_number(NAN);
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
                            return n.new_number(d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(NAN);
                        if (s == 1.0 || s == -1.0)
                            return n.new_number(0.0);
                        return n.new_number(1.0);
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
                            if (d == 1.0)
                                return n.new_number(0.0);
                            return n.new_number(1.0);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        return n.new_number(std::dynamic_pointer_cast<jsv_number>(op)->number + 1.0);
                    case r_string:
                        return n.new_string(_str_t + std::dynamic_pointer_cast<jsv_string>(op)->str);
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(2.0) :
                               n.new_number(1.0);
                    case r_object:
                        return n.new_string(_str_t + jsv_object::_str);
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        return n.new_number(1.0 - s);
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
                            return n.new_number(1.0 - d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(INFINITY);
                        return n.new_number(1.0 / s);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(INFINITY);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_number(INFINITY);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(INFINITY);
                            return n.new_number(1.0 / d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1U << c)));
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
                            auto a = fix(d);
                            auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                            return n.new_number(double(int(1U << c)));
                        }
                        return n.new_number(1.0);
                    }
                    case r_boolean:
                        return n.new_number(
                                std::dynamic_pointer_cast<jsv_boolean>(op)->b ? 2.0 : 1.0);
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(1.0);
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1 >> c)));
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
                            auto a = fix(d);
                            auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                            return n.new_number(double(int(1 >> c)));
                        }
                        return n.new_number(1.0);
                    }
                    case r_boolean:
                        return n.new_number(
                                std::dynamic_pointer_cast<jsv_boolean>(op)->b ? 0.0 : 1.0);
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(1.0);
                        auto a = fix(s);
                        auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                        return n.new_number(double(int(1U >> c)));
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
                            auto a = fix(d);
                            auto c = a > 0 ? (uint32_t(a) % 32) : uint32_t(int(fmod(a, 32)) + 32);
                            return n.new_number(double(int(1U >> c)));
                        }
                        return n.new_number(1.0);
                    }
                    case r_boolean:
                        return n.new_number(
                                std::dynamic_pointer_cast<jsv_boolean>(op)->b ? 0.0 : 1.0);
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return op;
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U & a)));
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
                                return n.new_number(0.0);
                            auto a = uint32_t(fix(d));
                            return n.new_number(double(int(1U & a)));
                        }
                        return n.new_number(0.0);
                    }
                    case r_boolean:
                        return n.new_number(
                                std::dynamic_pointer_cast<jsv_boolean>(op)->b ? 1.0 : 0.0);
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
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U ^ a)));
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
                            auto a = uint32_t(fix(d));
                            return n.new_number(double(int(1U ^ a)));
                        }
                        return n.new_number(1.0);
                    }
                    case r_boolean:
                        return n.new_number(
                                std::dynamic_pointer_cast<jsv_boolean>(op)->b ? 0.0 : 1.0);
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        auto a = uint32_t(fix(s));
                        return n.new_number(double(int(1U | a)));
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
                            auto a = uint32_t(fix(d));
                            return n.new_number(double(int(1U | a)));
                        }
                        return n.new_number(1.0);
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

    js_value::ref jsv_boolean::binary_false(js_value_new &n, int code, js_value::ref op) {
        switch (code) {
            case BINARY_POWER:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(1.0);
                        if (s > 0.0)
                            return n.new_number(0.0);
                        else
                            return n.new_number(INFINITY);
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
                            if (d > 0.0)
                                return n.new_number(0.0);
                            return n.new_number(0.0);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                    case r_number:{
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        return n.new_number(s >= 0.0 ? 0.0 : -0.0);
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
                            return n.new_number(0.0);
                        }
                        return n.new_number(NAN);
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(0.0);
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
                            return n.new_number(0.0);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        return op;
                    case r_string:
                        return n.new_string(_str_f + std::dynamic_pointer_cast<jsv_string>(op)->str);
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(1.0) :
                               n.new_number(0.0);
                    case r_object:
                        return n.new_string(_str_f + jsv_object::_str);
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return op;
                        return n.new_number(0.0 - s);
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
                                return n.new_number(0.0);
                            return n.new_number(0.0 - d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(s >= 0.0 ? 0.0 : -0.0);
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
                            return n.new_number(0.0);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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
                    case r_number:
                        return op;
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
                                return n.new_number(0.0);
                            return n.new_number(d);
                        }
                        return n.new_number(0.0);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
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

    void jsv_boolean::print(std::ostream &os) {
        os << std::boolalpha << b;
    }
}