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
                break;
            case UNARY_NEGATIVE:
                break;
            case UNARY_NOT:
                break;
            case UNARY_INVERT:
                break;
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(false);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(false);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(1.0 < d);
                        }
                        return n.new_boolean(false);
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(false);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(false);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(1.0 <= d);
                        }
                        return n.new_boolean(false);
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(false);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(false);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(1.0 == d);
                        }
                        return n.new_boolean(false);
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return shared_from_this();
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(1.0 > d);
                        }
                        return n.new_boolean(false);
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return shared_from_this();
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(1.0 >= d);
                        }
                        return n.new_boolean(false);
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
                    case r_number:
                        return n.new_number(1.0);
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        if (s == 0.0)
                            return n.new_number(NAN);
                        if (s == 1.0 || s == -1.0)
                            return n.new_number(0.0);
                        return n.new_number(fmod(1.0, s));
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_NUM(op);
                        return n.new_number(1.0 - s);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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

    js_value::ref jsv_boolean::binary_false(js_value_new &n, int code, const js_value::ref &op) {
        switch (code) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(0.0 < JS_NUM(op));
                    case r_string: {
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return shared_from_this();
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(0.0 < d);
                        }
                        return shared_from_this();
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(true);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(true);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(0.0 <= d);
                        }
                        return shared_from_this();
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(true);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(true);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(0.0 == d);
                        }
                        return shared_from_this();
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return shared_from_this();
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(0.0 > d);
                        }
                        return shared_from_this();
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
                        const auto &s = JS_STR(op);
                        if (s.empty())
                            return n.new_boolean(true);
                        std::stringstream ss;
                        ss << trim(s);
                        if (ss.str().empty())
                            return n.new_boolean(true);
                        double d;
                        ss >> d;
                        if (ss.eof() && !ss.fail()) {
                            return n.new_boolean(0.0 >= d);
                        }
                        return shared_from_this();
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
                        if (s == 0.0)
                            return n.new_number(1.0);
                        if (s > 0.0)
                            return n.new_number(0.0);
                        else
                            return n.new_number(INFINITY);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(0.0);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_NUM(op);
                        if (s == 0.0)
                            return n.new_number(0.0);
                        return n.new_number(0.0 - s);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        if (s == 0.0)
                            return n.new_number(NAN);
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
                    }
                    case r_string: {
                        const auto &s = JS_STR(op);
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
                        const auto &s = JS_STR(op);
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

    void jsv_boolean::print(std::ostream &os) {
        os << std::boolalpha << b;
    }

    std::string jsv_boolean::to_string() const {
        return b ? _str_t : _str_f;
    }
}