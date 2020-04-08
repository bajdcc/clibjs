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

    js_value::ref jsv_array::clone() const {
        return nullptr;
    }

    runtime_t jsv_array::get_type() {
        return r_array;
    }

    js_value::ref jsv_array::binary_op(js_value_new &n, int code, js_value::ref op) {
        return nullptr;
    }

    bool jsv_array::to_bool() const {
        return true;
    }

    void jsv_array::mark(int n) {
        marked = n;
        for (auto &s : arr)
            s.lock()->mark(n);
    }

    void jsv_array::print(std::ostream &os) {
        os << "[";
        std::transform(arr.begin(), arr.end(),
                       std::ostream_iterator<std::string>(os, ","),
                       [](auto x) {
                           std::stringstream ss;
                           x.lock()->print(ss);
                           return ss.str();
                       });
        os << "]";
    }

    jsv_array::ref jsv_array::clear() {
        arr.clear();
        return std::dynamic_pointer_cast<jsv_array>(shared_from_this());
    }
}