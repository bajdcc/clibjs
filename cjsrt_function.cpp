//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <cmath>
#include <sstream>
#include <cassert>
#include "cjsruntime.h"

namespace clib {

    jsv_function::jsv_function(sym_code_t::ref c) {
        code = std::make_shared<cjs_function_info>(std::move(c));
    }

    js_value::ref jsv_function::clone() const {
        return nullptr;
    }

    runtime_t jsv_function::get_type() {
        return r_function;
    }

    js_value::ref jsv_function::binary_op(js_value_new &n, int code, js_value::ref op) {
        return nullptr;
    }

    bool jsv_function::to_bool() const {
        return true;
    }

    void jsv_function::mark(int n) {
        if (builtin)
            return;
        marked = n;
        if (closure.lock()) {
            closure.lock()->mark(n);
        }
    }

    void jsv_function::print(std::ostream &os) {
        os << code->text;
    }

    jsv_function::ref jsv_function::clear() {
        code = nullptr;
        closure.reset();
        name.clear();
        return std::dynamic_pointer_cast<jsv_function>(shared_from_this());
    }

    cjs_function::cjs_function(sym_code_t::ref code) {
        info = std::make_shared<cjs_function_info>(std::move(code));
    }

    cjs_function::cjs_function(cjs_function_info::ref
                               code) : info(std::move(code)) {

    }

    void cjs_function::store_name(const std::string &n, js_value::weak_ref obj) {
        envs.insert({n, std::move(obj)});
    }

    void cjs_function::store_fast(const std::string &n, js_value::weak_ref obj) {
        envs.insert({n, std::move(obj)});
    }

    cjs_function_info::cjs_function_info(
            const sym_code_t::ref &code) {
        fullname = std::move(code->fullname);
        args = std::move(code->args_str);
        closure = std::move(code->closure_str);
        codes = std::move(code->codes);
        text = std::move(code->text);
        const auto &c = code->consts;
        std::copy(c.get_names_data().begin(),
                  c.get_names_data().end(),
                  std::back_inserter(names));
        std::copy(c.get_globals_data().begin(),
                  c.get_globals_data().end(),
                  std::back_inserter(globals));
        std::copy(c.get_derefs_data().begin(),
                  c.get_derefs_data().end(),
                  std::back_inserter(derefs));
        consts.resize(c.get_consts_data().size());
        for (size_t i = 0; i < c.get_consts_data().size(); i++) {
            consts[i] = load_const(c, i);
            consts[i]->attr |= js_value::at_const;
        }
    }

    js_value::ref cjs_function_info::load_const(const cjs_consts &c, int op) {
        auto t = c.get_type(op);
        switch (t) {
            case r_number:
                return std::make_shared<jsv_number>(*(double *) c.get_data(op));
            case r_string:
                return std::make_shared<jsv_string>(*(std::string *) c.get_data(op));
            case r_boolean:
                break;
            case r_object:
                break;
            case r_function:
                return std::make_shared<jsv_function>(((
                        sym_code_t::weak_ref *) c.get_data(op))->lock());
            default:
                break;
        }
        assert(!"invalid runtime type");
        return nullptr;
    }
}