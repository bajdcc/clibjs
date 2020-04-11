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

    std::string jsv_undefined::_str = "undefined";

    js_value::ref jsv_undefined::clone() const {
        return nullptr;
    }

    runtime_t jsv_undefined::get_type() {
        return r_undefined;
    }

    js_value::ref jsv_undefined::binary_op(js_value_new &n, int code, js_value::ref op) {
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
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_MODULO:
                switch (op->get_type()) {
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_number(NAN);
                    default:
                        break;
                }
                break;
            case BINARY_ADD:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_number(NAN);
                    case r_string:
                        return n.new_string(_str + std::dynamic_pointer_cast<jsv_string>(op)->str);
                    case r_boolean:
                        return n.new_number(NAN);
                    case r_object:
                        return n.new_string(_str + jsv_object::_str);
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
                        return n.new_string(_str + s);
                    }
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
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
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
                    case r_number:
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
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

    bool jsv_undefined::to_bool() const {
        return false;
    }

    void jsv_undefined::mark(int n) {
    }

    void jsv_undefined::print(std::ostream &os) {
        os << _str;
    }
}