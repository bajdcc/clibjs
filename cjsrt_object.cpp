//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iterator>
#include "cjsruntime.h"

namespace clib {

    std::string jsv_object::_str = "[object Object]";

    runtime_t jsv_object::get_type() {
        return r_object;
    }

    js_value::ref jsv_object::binary_op(js_value_new &n, int code, const js_value::ref &op) {
        switch (code) {
            case COMPARE_LESS:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(false);
                    case r_string:
                        return n.new_boolean(to_string(&n, 0) < op->to_string(&n, 0));
                    case r_boolean:
                        return n.new_boolean(false);
                    case r_object:
                    case r_function:
                        return n.new_boolean(to_string(&n, 0) < op->to_string(&n, 0));
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
                        return n.new_boolean(false);
                    case r_string:
                        return n.new_boolean(to_string(&n, 0) <= op->to_string(&n, 0));
                    case r_boolean:
                        return n.new_boolean(false);
                    case r_object:
                    case r_function:
                        return n.new_boolean(to_string(&n, 0) <= op->to_string(&n, 0));
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
                    case r_string:
                    case r_boolean:
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    case r_object:
                        return n.new_boolean(shared_from_this() == op);
                    case r_function:
                        return n.new_boolean(to_string(&n, 0) == op->to_string(&n, 0));
                    default:
                        break;
                }
                break;
            case COMPARE_NOT_EQUAL:
                return n.new_boolean(!JS_BOOL(binary_op(n, COMPARE_EQUAL, op)));
            case COMPARE_GREATER:
                switch (op->get_type()) {
                    case r_number:
                        return n.new_boolean(false);
                    case r_string:
                        return n.new_boolean(to_string(&n, 0) > op->to_string(&n, 0));
                    case r_boolean:
                        return n.new_boolean(false);
                    case r_object:
                    case r_function:
                        return n.new_boolean(to_string(&n, 0) > op->to_string(&n, 0));
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
                        return n.new_boolean(false);
                    case r_string:
                        return n.new_boolean(to_string(&n, 0) >= op->to_string(&n, 0));
                    case r_boolean:
                        return n.new_boolean(false);
                    case r_object:
                    case r_function:
                        return n.new_boolean(to_string(&n, 0) >= op->to_string(&n, 0));
                    case r_null:
                    case r_undefined:
                        return n.new_boolean(false);
                    default:
                        break;
                }
                break;
            case COMPARE_FEQUAL:
                switch (op->get_type()) {
                    case r_object:
                        return n.new_boolean(shared_from_this() == op);
                    case r_null:
                    case r_boolean:
                    case r_number:
                    case r_string:
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
                        return n.new_number(NAN);
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
                                return n.new_number(NAN);
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
                    case r_string:
                    case r_boolean:
                    case r_object:
                    case r_function:
                    case r_null:
                    case r_undefined:
                        return n.new_string(to_string(&n, 0) + op->to_string(&n, 0));
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
                    case r_number: {
                        auto d = fix(JS_NUM(op));
                        if (d == 0.0)d = 0.0;
                        return n.new_number(d);
                    }
                    case r_string: {
                        double d;
                        switch (JS_STR2NUM(op, d)) {
                            case 0:
                            case 1:
                                return n.new_number(0.0);
                            case 2:
                                if (d == 0.0 || std::isinf(d))
                                    return n.new_number(0.0);
                                return n.new_number(d);
                            case 3:
                                return n.new_number(0.0);
                            default:
                                break;
                        }
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

    js_value::ref jsv_object::unary_op(js_value_new &n, int code) {
        switch (code) {
            case UNARY_POSITIVE:
                return n.new_number(to_number(&n));
            case UNARY_NEGATIVE:
                return n.new_number(-to_number(&n));
            case UNARY_NOT:
                return n.new_boolean(false);
            case UNARY_INVERT:
                return n.new_number(-1.0);
            case UNARY_NEW:
                break;
            case UNARY_DELETE:
                break;
            case UNARY_TYPEOF:
                return n.new_string("object");
            default:
                break;
        }
        return nullptr;
    }

    bool jsv_object::to_bool() const {
        return true;
    }

    void jsv_object::mark(int n) {
        marked = n;
        for (const auto &s : obj) {
            if ((s.second.lock()->marked > 0 ? 1 : 0) != (n > 0 ? 1 : 0))
                s.second.lock()->mark(n);
        }
        for (const auto &s : special) {
            if ((s.second.lock()->marked > 0 ? 1 : 0) != (n > 0 ? 1 : 0))
                s.second.lock()->mark(n);
        }
    }

    std::string jsv_object::to_string(js_value_new *n, int hint) const {
        if (!n) {
            return _str;
        }
        if (!special.empty()) {
            auto f = special.find("PrimitiveValue");
            if (f != special.end()) {
                return f->second.lock()->to_string(n, 0);
            }
        }
        if (__proto__.lock()) {
            auto proto = __proto__.lock();
            if (!proto) {
                return _str;
            }
            auto p = proto;
            while (p) {
                if (p->get_type() != r_object) {
                    break;
                }
                const auto &o = JS_OBJ(p);
                auto f = o.find("toString");
                if (f != o.end()) {
                    auto fun = f->second.lock();
                    if (fun->get_type() == r_function) {
                        std::vector<js_value::weak_ref> args;
                        args.push_back(n->new_number(hint));
                        js_value::weak_ref _this = std::const_pointer_cast<js_value>(shared_from_this());
                        return n->fast_api(JS_FUN(fun), _this, args, 0)->to_string(n, 0);
                    }
                }
                p = p->__proto__.lock();
            }
            const auto &p2 = JS_OBJ(proto);
            auto f = p2.find("__type__");
            if (f != p2.end() && f->second.lock()) {
                std::stringstream ss;
                ss << "[object " << f->second.lock()->to_string(n, 0) << "]";
                return ss.str();
            }
        }
        return _str;
    }

    double jsv_object::to_number(js_value_new *n) const {
        auto s = to_string(n, 0);
        auto d = 0.0;
        switch (jsv_string::to_number(s, d)) {
            case 0:
            case 1:
                return 0;
            case 2:
                return d;
            case 3:
                return NAN;
            default:
                break;
        }
    }

    jsv_object::ref jsv_object::clear() {
        obj.clear();
        special.clear();
        return std::dynamic_pointer_cast<jsv_object>(shared_from_this());
    }
}