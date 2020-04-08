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
        switch (code) {
            case BINARY_POWER:
                break;
            case BINARY_MULTIPLY:
                break;
            case BINARY_MODULO:
                break;
            case BINARY_ADD: {
                switch (op->get_type()) {
                    case r_number: {
                        const auto &s = std::dynamic_pointer_cast<jsv_number>(op)->number;
                        std::stringstream ss(str);
                        ss << s;
                        return n.new_string(ss.str());
                    }
                    case r_string: {
                        if (str.empty())
                            return op;
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        return n.new_string(str + s);
                    }
                    case r_boolean: {
                        const auto &b = std::dynamic_pointer_cast<jsv_boolean>(op)->b;
                        return n.new_string(str + (b ? "true" : "false"));
                    }
                    case r_regex:
                        break;
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        if (s->arr.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        s->print(ss);
                        auto _s = ss.str();
                        _s.erase(_s.begin());
                        _s.pop_back();
                        return n.new_string(str + _s);
                    }
                    case r_object: {
                        return n.new_string(str + "[object Object]");
                    }
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
                        std::stringstream ss(str);
                        ss << s;
                        return n.new_string(ss.str());
                    }
                    default:
                        break;
                }
            }
                break;
            case BINARY_SUBTRACT:
                break;
            case BINARY_FLOOR_DIVIDE:
                break;
            case BINARY_TRUE_DIVIDE:
                break;
            case BINARY_LSHIFT:
                break;
            case BINARY_RSHIFT:
                break;
            case BINARY_URSHIFT:
                break;
            case BINARY_AND:
                break;
            case BINARY_XOR:
                break;
            case BINARY_OR:
                break;
            default:
                break;
        }
        return nullptr;
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
}