//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

namespace clib {

    std::string jsv_object::_str = "[object Object]";

    js_value::ref jsv_object::clone() const {
        return nullptr;
    }

    runtime_t jsv_object::get_type() {
        return r_object;
    }

    js_value::ref jsv_object::binary_op(js_value_new &n, int code, js_value::ref op) {
        return nullptr;
    }

    bool jsv_object::to_bool() const {
        return true;
    }

    void jsv_object::mark(int n) {
        marked = n;
    }

    void jsv_object::print(std::ostream &os) {
        os << _str;
    }

    std::string jsv_object::to_string() const {
        return _str;
    }

    jsv_object::ref jsv_object::clear() {
        obj.clear();
        return std::dynamic_pointer_cast<jsv_object>(shared_from_this());
    }
}