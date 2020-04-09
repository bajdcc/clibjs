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

    js_value::ref jsv_null::clone() const {
        return nullptr;
    }

    runtime_t jsv_null::get_type() {
        return r_null;
    }

    js_value::ref jsv_null::binary_op(js_value_new &n, int code, js_value::ref op) {
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
}