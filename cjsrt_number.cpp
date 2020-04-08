//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

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
                        return n.new_number(
                                pow(number,
                                    std::dynamic_pointer_cast<jsv_number>(op)->number));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
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
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return n.new_number(1.0);
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
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
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
                    default:
                        break;
                }
                break;
            case BINARY_MULTIPLY:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (number == 0.0 || s == 0.0)
                            return shared_from_this();
                        if (number == 1.0)
                            return op;
                        if (s == 1.0)
                            return shared_from_this();
                        return n.new_number(
                                number *
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(0.0);
                        std::stringstream ss;
                        ss << trim(s);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (number == 0.0 || d == 0.0)
                                return shared_from_this();
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
                               n.new_number(1.0);
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return n.new_number(1.0);
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (number == 0.0 || d == 0.0)
                                return shared_from_this();
                            if (number == 1.0)
                                return op;
                            if (d == 1.0)
                                return shared_from_this();
                            return n.new_number(pow(number, d));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return n.new_number(0);
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
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
                        return n.new_number(fmod(
                                number,
                                std::dynamic_pointer_cast<jsv_number>(op)->number));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return n.new_number(NAN);
                        std::stringstream ss;
                        ss << trim(s);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(NAN);
                            return n.new_number(fmod(number, d));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(fmod(number, 1.0)) :
                               n.new_number(NAN);
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return n.new_number(NAN);
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(NAN);
                            return n.new_number(fmod(number, d));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return n.new_number(NAN);
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
                    default:
                        break;
                }
                break;
            case BINARY_ADD: {
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        if (number == 0.0)
                            return op;
                        return n.new_number(
                                number +
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << number;
                        return n.new_string(
                                ss.str() +
                                std::dynamic_pointer_cast<jsv_string>(op)->str);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(
                                       number + 1.0) :
                               shared_from_this();
                    case r_regex: {
                        const auto &s = std::dynamic_pointer_cast<jsv_regex>(op);
                        std::stringstream ss;
                        ss << number << s;
                        return n.new_string(ss.str());
                    }
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        ss << number;
                        auto r = ss.str();
                        if (s->arr.empty())
                            return n.new_string(r);
                        ss.str("");
                        s->print(ss);
                        auto _s = ss.str();
                        _s.erase(_s.begin());
                        _s.pop_back();
                        return n.new_string(r + _s);
                    }
                    case r_object: {
                        std::stringstream ss;
                        ss << number << "[object Object]";
                        return n.new_string(ss.str());
                    }
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
                        std::stringstream ss;
                        ss << number << s;
                        return n.new_string(ss.str());
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
                    default:
                        break;
                }
            }
                break;
            case BINARY_SUBTRACT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        return n.new_number(
                                number -
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
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
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            return n.new_number(number - d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
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
                            return n.new_number(number > 0 ? INFINITY : -INFINITY);
                        return n.new_number(
                                number /
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(number > 0 ? INFINITY : -INFINITY);
                            return n.new_number(number / d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               shared_from_this() :
                               n.new_number(number > 0 ? INFINITY : -INFINITY);
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return n.new_number(number > 0 ? INFINITY : -INFINITY);
                            return n.new_number(number / d);
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return n.new_number(INFINITY);
                    }
                    case r_undefined: {
                        return n.new_number(NAN);
                    }
                    default:
                        break;
                }
                break;
            case BINARY_LSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a << c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a << c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a << 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a << c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return shared_from_this();
                    }
                    default:
                        break;
                }
                break;
            case BINARY_RSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = int(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a >> c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = int(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a >> c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = int(fix(number));
                        return n.new_number(double(int(a >> 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = int(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a >> c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return shared_from_this();
                    }
                    default:
                        break;
                }
                break;
            case BINARY_URSHIFT:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a >> c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a >> c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a >> 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a >> c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return shared_from_this();
                    }
                    default:
                        break;
                }
                break;
            case BINARY_AND:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a & c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a & c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a & 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a & c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return n.new_number(0.0);
                    }
                    case r_undefined: {
                        return n.new_number(0.0);
                    }
                    default:
                        break;
                }
                break;
            case BINARY_XOR:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a ^ c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a ^ c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a ^ 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a ^ c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return shared_from_this();
                    }
                    default:
                        break;
                }
                break;
            case BINARY_OR:
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        auto b = fix(s);
                        auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                        return n.new_number(double(int(a | c)));
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss.str(trim(s));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a | c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_boolean: {
                        if (!std::dynamic_pointer_cast<jsv_boolean>(op)->b)
                            return shared_from_this();
                        auto a = uint32_t(fix(number));
                        return n.new_number(double(int(a | 1U)));
                    }
                    case r_regex:
                        return n.new_number(NAN);
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        if (s->arr.empty())
                            return shared_from_this();
                        ss.str("");
                        s->print(ss);
                        ss.str(trim(ss.str()));
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            if (d == 0.0)
                                return shared_from_this();
                            auto a = uint32_t(fix(number));
                            auto b = fix(d);
                            auto c = b > 0 ? (uint32_t(b) % 32) : uint32_t(int(fmod(b, 32)) + 32);
                            return n.new_number(double(int(a | c)));
                        }
                        return n.new_number(NAN);
                    }
                    case r_object: {
                        return n.new_number(NAN);
                    }
                    case r_function: {
                        return n.new_number(NAN);
                    }
                    case r_null: {
                        return shared_from_this();
                    }
                    case r_undefined: {
                        return shared_from_this();
                    }
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

    void jsv_number::print(std::ostream &os) {
        os << number;
    }
}