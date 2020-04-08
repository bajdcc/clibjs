//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include "cjsruntime.h"

namespace clib {

    jsv_regex::jsv_regex(std::string s) : re(std::move(s)) {

    }

    js_value::ref jsv_regex::clone() const {
        return std::make_shared<jsv_regex>(re);
    }

    runtime_t jsv_regex::get_type() {
        return r_regex;
    }

    js_value::ref jsv_regex::binary_op(js_value_new &n, int code, js_value::ref op) {
        return nullptr;
    }

    bool jsv_regex::to_bool() const {
        return true;
    }

    void jsv_regex::mark(int n) {
        marked = n;
    }

    void jsv_regex::print(std::ostream &os) {
        os << re;
    }

    jsv_regex::ref jsv_regex::clear() {
        re.clear();
        return std::dynamic_pointer_cast<jsv_regex>(shared_from_this());
    }
}