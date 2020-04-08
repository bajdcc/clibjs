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

    js_value::ref jsv_undefined::clone() const {
        return nullptr;
    }

    runtime_t jsv_undefined::get_type() {
        return r_undefined;
    }

    js_value::ref jsv_undefined::binary_op(js_value_new &n, int code, js_value::ref op) {
        return nullptr;
    }

    bool jsv_undefined::to_bool() const {
        return false;
    }

    void jsv_undefined::mark(int n) {
    }

    void jsv_undefined::print(std::ostream &os) {
        os << "undefined";
    }
}