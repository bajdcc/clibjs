//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

namespace clib {

    jsv_boolean::jsv_boolean(bool flag) : b(flag) {

    }

    js_value::ref jsv_boolean::clone() const {
        return std::make_shared<jsv_boolean>(b);
    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    js_value::ref jsv_boolean::binary_op(js_value_new &n, int code, js_value::ref op) {
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