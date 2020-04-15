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
                            return n.new_boolean(0.0 < d);
                        }
                        return n.new_boolean(false);
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
                        return n.new_boolean(false);
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
                            return n.new_boolean(0.0 > d);
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
                        return n.new_boolean(false);
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
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return op;
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
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
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        if (s == 0.0)
                            return n.new_number(0.0);
                        return op;
                    }
                    case r_string:
                        return n.new_string(_str + std::dynamic_pointer_cast<jsv_string>(op)->str);
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(1.0) :
                               n.new_number(0.0);
                    case r_object:
                        return n.new_string(_str + op->to_string());
                    case r_function:
                        return n.new_string(_str + op->to_string());
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
                            return n.new_number(0.0);
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
                        return n.new_number(std::signbit(s) == 0 ? 0.0 : -0.0);
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
                    case r_number: {
                        auto d = fix(std::dynamic_pointer_cast<jsv_number>(op)->number);
                        if (d == 0.0)d = 0.0;
                        return n.new_number(d);
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

    js_value::ref jsv_null::unary_op(js_value_new &n, int code) {
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
                return n.new_string("null");
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

    void jsv_null::print(std::ostream &os) {
        os << _str;
    }

    std::string jsv_null::to_string() const {
        return _str;
    }
}