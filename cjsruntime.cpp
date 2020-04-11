//
// Project: clibjs
// Created by bajdcc
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <fstream>
#include "cjsruntime.h"
#include "cjsast.h"

#define DUMP_STEP 0
#define DUMP_GC 0
#define DUMP_PRINT_FILE_ENABLE 1
#define DUMP_PRINT_FILE_AUTO_CLEAR 1
#define DUMP_PRINT_FILE "debug_print.txt"

namespace clib {

    std::string trim(std::string s) {
        if (s.empty()) {
            return s;
        }
        s.erase(0, s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
        return s;
    }

    double fix(const double &d) {
        if (d == 0) {
            return d;
        }
        if (d >= 0)return floor(d);
        return ceil(d);
    }

    cjsruntime::cjsruntime() {
        permanents._null = std::make_shared<jsv_null>();
        permanents._undefined = std::make_shared<jsv_undefined>();
        permanents._true = std::make_shared<jsv_boolean>(true);
        permanents._false = std::make_shared<jsv_boolean>(false);
        permanents.__nan = std::make_shared<jsv_number>(NAN);
        permanents._inf = std::make_shared<jsv_number>(INFINITY);
        permanents._minus_inf = std::make_shared<jsv_number>(-INFINITY);
        permanents._zero = std::make_shared<jsv_number>(0);
        permanents._minus_zero = std::make_shared<jsv_number>(-0.0);
        permanents._one = std::make_shared<jsv_number>(1);
        permanents._minus_one = std::make_shared<jsv_number>(-1);
        permanents._empty = std::make_shared<jsv_string>("");
        permanents._debug_print = std::make_shared<jsv_function>();
#if DUMP_PRINT_FILE_ENABLE && DUMP_PRINT_FILE_AUTO_CLEAR
        permanents._debug_print->name = "debug_print";
        {
            std::ofstream ofs(DUMP_PRINT_FILE);
        }
#endif
        permanents._debug_print->builtin = [](auto &func, auto &args, auto &js) {
#if DUMP_PRINT_FILE_ENABLE
            std::ofstream ofs(DUMP_PRINT_FILE, std::ios::app);
            for (size_t i = 0; i < args.size(); i++) {
                args[i].lock()->print(ofs);
                if (i + 1 < args.size())
                    ofs << " ";
            }
            ofs << std::endl;
#endif
            func->stack.push_back(js.new_undefined());
        };
        global_env.insert({permanents._debug_print->name, permanents._debug_print});
    }

    void cjsruntime::eval(cjs_code_result::ref code) {
        stack.clear();
        stack.push_back(std::make_shared<cjs_function>(code->code));
        current_stack = stack.back();
        std::swap(global_env, current_stack->envs);
        code = nullptr;
        auto r = 0;
        decltype(stack.back()->ret_value) ret;
        while (!stack.empty()) {
            const auto &codes = current_stack->info->codes;
            const auto &pc = current_stack->pc;
            while (true) {
                if (pc >= (int) codes.size())
                    break;
                const auto &c = codes.at(pc);
                if (pc + 1 == (int) codes.size() && c.code == POP_TOP) {
                    r = 2;
                    break;
                }
#if DUMP_STEP
                dump_step(c);
#endif
                r = run(code->code, c);
#if DUMP_STEP
                dump_step2(c);
#endif
                gc();
                if (r != 0)
                    break;
            }
            if (r == 1) {
                current_stack = stack.back();
                continue;
            }
            if (r == 2) {
                current_stack->ret_value = pop();
            }
            ret = stack.back()->ret_value;
            if (stack.size() > 1) {
                stack.pop_back();
                current_stack = stack.back();
                push(ret);
            } else {
                global_env.clear();
                std::swap(global_env, current_stack->envs);
                stack.pop_back();
            }
        }
        assert(ret.lock());
        ret.lock()->print(std::cout);
        std::cout << std::endl;
    }

    int cjsruntime::run(const sym_code_t::ref &fun, const cjs_code &code) {
        switch (code.code) {
            case LOAD_EMPTY: {
                js_value::ref v;
                push(v);
            }
                break;
            case LOAD_NULL:
                push(permanents._null);
                break;
            case LOAD_UNDEFINED:
                push(permanents._undefined);
                break;
            case LOAD_TRUE:
                push(permanents._true);
                break;
            case LOAD_FALSE:
                push(permanents._false);
                break;
            case LOAD_ZERO:
                push(code.op1 == 0 ? permanents._zero : permanents._minus_zero);
                break;
            case POP_TOP:
                pop();
                break;
            case ROT_TWO:
                break;
            case ROT_THREE:
                break;
            case DUP_TOP:
                push(top());
                break;
            case DUP_TOP_TWO:
                break;
            case ROT_FOUR:
                break;
            case NOP:
                break;
            case UNARY_POSITIVE:
                break;
            case UNARY_NEGATIVE:
                break;
            case UNARY_NOT:
                break;
            case UNARY_INVERT:
                break;
            case BINARY_MATRIX_MULTIPLY:
                break;
            case INPLACE_MATRIX_MULTIPLY:
                break;
            case BINARY_POWER:
            case BINARY_MULTIPLY:
            case BINARY_MODULO:
            case BINARY_ADD:
            case BINARY_SUBTRACT:
            case BINARY_FLOOR_DIVIDE:
            case BINARY_TRUE_DIVIDE:
            case BINARY_LSHIFT:
            case BINARY_RSHIFT:
            case BINARY_URSHIFT:
            case BINARY_AND:
            case BINARY_XOR:
            case BINARY_OR: {
                auto op2 = pop().lock();
                auto op1 = pop().lock();
                auto result = op1->binary_op(*this, code.code, op2);
                assert(result);
                push(result);
            }
                break;
            case BINARY_SUBSCR:
                break;
            case BINARY_INC:
            case BINARY_DEC: {
                auto op1 = pop();
            }
                break;
            case INPLACE_FLOOR_DIVIDE:
                break;
            case INPLACE_TRUE_DIVIDE:
                break;
            case RERAISE:
                break;
            case WITH_EXCEPT_START:
                break;
            case GET_AITER:
                break;
            case GET_ANEXT:
                break;
            case BEFORE_ASYNC_WITH:
                break;
            case END_ASYNC_FOR:
                break;
            case INPLACE_ADD:
                break;
            case INPLACE_SUBTRACT:
                break;
            case INPLACE_MULTIPLY:
                break;
            case INPLACE_MODULO:
                break;
            case STORE_SUBSCR:
                break;
            case DELETE_SUBSCR:
                break;
            case INPLACE_POWER:
                break;
            case GET_ITER:
                break;
            case GET_YIELD_FROM_ITER:
                break;
            case PRINT_EXPR:
                break;
            case LOAD_BUILD_CLASS:
                break;
            case YIELD_FROM:
                break;
            case GET_AWAITABLE:
                break;
            case LOAD_ASSERTION_ERROR:
                break;
            case INPLACE_LSHIFT:
                break;
            case INPLACE_RSHIFT:
                break;
            case INPLACE_AND:
                break;
            case INPLACE_XOR:
                break;
            case INPLACE_OR:
                break;
            case LIST_TO_TUPLE:
                break;
            case RETURN_VALUE:
                return 2;
            case IMPORT_STAR:
                break;
            case SETUP_ANNOTATIONS:
                break;
            case YIELD_VALUE:
                break;
            case POP_BLOCK:
                break;
            case POP_EXCEPT:
                break;
            case HAVE_ARGUMENT:
                break;
            case STORE_NAME: {
                auto obj = top();
                auto id = code.op1;
                auto name = current_stack->info->names.at(id);
                current_stack->store_name(name, obj);
            }
                break;
            case DELETE_NAME:
                break;
            case UNPACK_SEQUENCE:
                break;
            case FOR_ITER:
                break;
            case UNPACK_EX:
                break;
            case STORE_ATTR:
                break;
            case DELETE_ATTR:
                break;
            case STORE_GLOBAL: {
                auto obj = top();
                auto id = code.op1;
                auto name = current_stack->info->globals.at(id);
                stack.front()->store_name(name, obj);
            }
                break;
            case DELETE_GLOBAL:
                break;
            case LOAD_CONST: {
                auto op = code.op1;
                auto var = load_const(op);
                push(var);
            }
                break;
            case LOAD_NAME: {
                auto op = code.op1;
                auto var = load_name(op);
                push(var);
            }
                break;
            case BUILD_TUPLE:
                break;
            case BUILD_LIST: {
                auto n = code.op1;
                assert(current_stack->stack.size() >= n);
                auto obj = new_object();
                std::stringstream ss;
                for (auto i = 0; i < n; i++) {
                    auto v = pop().lock();
                    if (v) {
                        ss.str("");
                        ss << i;
                        obj->obj.insert({ss.str(), v});
                    }
                }
                obj->obj.insert({"length", new_number(n)});
                push(obj);
            }
                break;
            case BUILD_SET:
                break;
            case BUILD_MAP: {
                auto n = code.op1;
                assert(current_stack->stack.size() >= n * 2);
                auto obj = new_object();
                for (auto i = 0; i < n; i++) {
                    auto v = pop();
                    auto k = pop().lock();
                    if (k->get_type() == r_string) {
                        obj->obj.insert({std::dynamic_pointer_cast<jsv_string>(k)->str, v});
                    } else if (k->get_type() == r_number) {
                        std::stringstream ss;
                        ss << std::dynamic_pointer_cast<jsv_number>(k)->number;
                        obj->obj.insert({ss.str(), v});
                    } else {
                        continue;
                    }
                }
                push(obj);
            }
                break;
            case LOAD_ATTR:
                break;
            case COMPARE_OP:
                break;
            case IMPORT_NAME:
                break;
            case IMPORT_FROM:
                break;
            case JUMP_FORWARD: {
                auto jmp = code.op1;
                current_stack->pc += jmp;
            }
                break;
            case JUMP_IF_FALSE_OR_POP: {
                auto jmp = code.op1;
                const auto &t = top();
                if (!t.lock()->to_bool()) {
                    current_stack->pc = jmp;
                    return 0;
                } else {
                    pop();
                }
            }
                break;
            case JUMP_IF_TRUE_OR_POP: {
                auto jmp = code.op1;
                const auto &t = top();
                if (t.lock()->to_bool()) {
                    current_stack->pc = jmp;
                    return 0;
                } else {
                    pop();
                }
            }
                break;
            case JUMP_ABSOLUTE: {
                auto jmp = code.op1;
                current_stack->pc = jmp;
                return 0;
            }
            case POP_JUMP_IF_FALSE: {
                auto jmp = code.op1;
                auto t = pop();
                if (!t.lock()->to_bool()) {
                    current_stack->pc = jmp;
                    return 0;
                }
            }
                break;
            case POP_JUMP_IF_TRUE: {
                auto jmp = code.op1;
                auto t = pop();
                if (t.lock()->to_bool()) {
                    current_stack->pc = jmp;
                    return 0;
                }
            }
                break;
            case LOAD_GLOBAL: {
                auto op = code.op1;
                auto var = load_global(op);
                push(var);
            }
                break;
            case IS_OP:
                break;
            case CONTAINS_OP:
                break;
            case JUMP_IF_NOT_EXC_MATCH:
                break;
            case SETUP_FINALLY:
                break;
            case LOAD_FAST: {
                auto op = code.op1;
                auto var = load_fast(op);
                push(var);
            }
                break;
            case STORE_FAST: {
                auto obj = top();
                auto id = code.op1;
                auto name = current_stack->info->names.at(id);
                current_stack->store_fast(name, obj);
            }
                break;
            case DELETE_FAST:
                break;
            case RAISE_VARARGS:
                break;
            case CALL_FUNCTION: {
                auto n = code.op1;
                assert((int) current_stack->stack.size() > n);
                std::vector<js_value::weak_ref> args(n);
                while (n-- > 0) {
                    args[n] = pop();
                }
                auto f = pop();
                assert(f.lock()->get_type() == r_function);
                auto func = std::dynamic_pointer_cast<jsv_function>(f.lock());
                if (func->builtin) {
                    func->builtin(current_stack, args, *this);
                    break;
                }
                stack.push_back(std::make_shared<cjs_function>(func->code));
                auto new_stack = stack.back();
                new_stack->name = func->name;
                for (auto i = 0; i < code.op1; i++) {
                    new_stack->envs.insert({func->code->args.at(i), args.at(i)});
                }
                if (func->closure.lock())
                    new_stack->closure = func->closure.lock()->obj;
                current_stack->pc++;
                return 1;
            }
            case MAKE_FUNCTION: {
                auto op = (uint32_t) code.op1;
                if (op & 8U) {
                    auto name = pop();
                    auto f = pop();
                    auto closure = pop();
                    assert(f.lock()->get_type() == r_function);
                    assert(name.lock()->get_type() == r_string);
                    assert(closure.lock()->get_type() == r_object);
                    auto func = std::dynamic_pointer_cast<jsv_function>(f.lock());
                    func->name = std::dynamic_pointer_cast<jsv_string>(name.lock())->str;
                    func->closure = std::dynamic_pointer_cast<jsv_object>(closure.lock());
                    push(f);
                } else {
                    auto name = pop();
                    auto f = top();
                    assert(f.lock()->get_type() == r_function);
                    assert(name.lock()->get_type() == r_string);
                    std::dynamic_pointer_cast<jsv_function>(f.lock())->name =
                            std::dynamic_pointer_cast<jsv_string>(name.lock())->str;
                }
            }
                break;
            case BUILD_SLICE:
                break;
            case LOAD_CLOSURE: {
                auto op = code.op1;
                auto var = current_stack->info->names.at(op);
                push(new_string(var));
                push(load_closure(var));
            }
                break;
            case LOAD_DEREF: {
                auto op = code.op1;
                auto var = current_stack->info->derefs.at(op);
                push(load_deref(var));
            }
                break;
            case STORE_DEREF:
                break;
            case DELETE_DEREF:
                break;
            case CALL_FUNCTION_KW:
                break;
            case CALL_FUNCTION_EX:
                break;
            case SETUP_WITH:
                break;
            case EXTENDED_ARG:
                break;
            case LIST_APPEND:
                break;
            case SET_ADD:
                break;
            case MAP_ADD:
                break;
            case LOAD_CLASSDEREF:
                break;
            case SETUP_ASYNC_WITH:
                break;
            case FORMAT_VALUE:
                break;
            case BUILD_CONST_KEY_MAP:
                break;
            case BUILD_STRING:
                break;
            case LOAD_METHOD:
                break;
            case CALL_METHOD:
                break;
            case LIST_EXTEND:
                break;
            case SET_UPDATE:
                break;
            case DICT_MERGE:
                break;
            case DICT_UPDATE:
                break;
            default:
                assert(!"invalid opcode");
                return 1;
        }
        current_stack->pc++;
        return 0;
    }

    js_value::ref cjsruntime::load_const(int op) {
        auto v = current_stack->info->consts.at(op);
        if (v)
            return register_value(v);
        assert(!"invalid runtime type");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_fast(int op) {
        auto name = current_stack->info->names.at(op);
        auto L = current_stack->envs.find(name);
        if (L != current_stack->envs.end()) {
            return L->second.lock();
        }
        assert(!"cannot find value by name");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_name(int op) {
        auto name = current_stack->info->names.at(op);
        for (auto i = stack.rbegin(); i != stack.rend(); i++) {
            auto L = (*i)->envs.find(name);
            if (L != (*i)->envs.end()) {
                return L->second.lock();
            }
        }
        assert(!"cannot find value by name");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_global(int op) {
        auto g = current_stack->info->globals.at(op);
        auto G = stack.front()->envs.find(g);
        if (G != stack.front()->envs.end()) {
            return G->second.lock();
        }
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_closure(const std::string &name) {
        for (auto i = stack.rbegin() + 1; i != stack.rend(); i++) {
            auto L = (*i)->envs.find(name);
            if (L != (*i)->envs.end()) {
                return L->second.lock();
            }
        }
        assert(!"cannot load closure value by name");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_deref(const std::string &name) {
        auto f = current_stack->closure.find(name);
        if (f != current_stack->closure.end()) {
            return f->second.lock();
        }
        assert(!"cannot load deref by name");
        return permanents._undefined;
    }

    void cjsruntime::push(js_value::weak_ref value) {
        current_stack->stack.push_back(std::move(value));
    }

    const js_value::weak_ref &cjsruntime::top() const {
        assert(!current_stack->stack.empty());
        return current_stack->stack.back();
    }

    js_value::weak_ref cjsruntime::pop() {
        assert(!current_stack->stack.empty());
        auto p = current_stack->stack.back();
        current_stack->stack.pop_back();
        return p;
    }

    js_value::ref cjsruntime::register_value(const js_value::ref &value) {
        if (value)
            objs.push_back(value);
        return value;
    }

    void cjsruntime::dump_step(const cjs_code &c) const {
        fprintf(stdout, "R [%04d] %s\n", current_stack->pc, c.desc.c_str());
    }

    void cjsruntime::dump_step2(const cjs_code &c) const {
        if (!stack.empty() && !stack.front()->stack.empty())
            std::cout << std::setfill('=') << std::setw(60) << "" << std::endl;
        for (auto s = stack.rbegin(); s != stack.rend(); s++) {
            fprintf(stdout, "**** Stack [%p] \"%.100s\"\n", s->get(), (*s)->name.c_str());
            const auto &st = (*s)->stack;
            auto sti = (int) st.size();
            for (auto s2 = st.rbegin(); s2 != st.rend(); s2++) {
                fprintf(stdout, "%4d | [%p] ", sti--, s2->lock().get());
                print(s2->lock(), 0, std::cout);
            }
            const auto &env = (*s)->envs;
            if (!env.empty()) {
                std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
                for (const auto &e : env) {
                    fprintf(stdout, " Env | [%p] \"%.100s\" ", e.second.lock().get(), e.first.c_str());
                    print(e.second.lock(), 0, std::cout);
                }
            }
            std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
        }
    }

    void cjsruntime::dump_step3() const {
        if (objs.empty())
            return;
        for (const auto &s : objs) {
            fprintf(stdout, " GC  | [%p] Mark: %d, ", s.get(), s->marked);
            print(s, 0, std::cout);
        }
    }

    jsv_number::ref cjsruntime::new_number(double n) {
        if (std::isnan(n)) {
            return permanents.__nan;
        }
        if (std::isinf(n)) {
            return std::signbit(n) == 0 ? permanents._inf : permanents._minus_inf;
        }
        if (n == 0.0) {
            return std::signbit(n) == 0 ? permanents._zero : permanents._minus_zero;
        }
        if (n == 1.0) {
            return permanents._one;
        }
        if (n == -1.0) {
            return permanents._minus_one;
        }
        if (reuse.reuse_numbers.empty()) {
            auto s = std::make_shared<jsv_number>(n);
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_numbers.back();
        register_value(r);
        r->number = n;
        reuse.reuse_numbers.pop_back();
        return std::move(r);
    }

    jsv_string::ref cjsruntime::new_string(const std::string &s) {
        if (s.empty())
            return permanents._empty;
        if (reuse.reuse_strings.empty()) {
            auto t = std::make_shared<jsv_string>(s);
            register_value(t);
            return t;
        }
        auto r = reuse.reuse_strings.back();
        register_value(r);
        r->str = s;
        reuse.reuse_strings.pop_back();
        return std::move(r);
    }

    jsv_boolean::ref cjsruntime::new_boolean(bool b) {
        if (b) return permanents._true;
        return permanents._false;
    }

    jsv_object::ref cjsruntime::new_object() {
        if (reuse.reuse_objects.empty()) {
            auto s = std::make_shared<jsv_object>();
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_objects.back();
        register_value(r);
        reuse.reuse_objects.pop_back();
        return std::move(r);
    }

    jsv_function::ref cjsruntime::new_function() {
        if (reuse.reuse_functions.empty()) {
            auto s = std::make_shared<jsv_function>();
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_functions.back();
        register_value(r);
        reuse.reuse_functions.pop_back();
        return std::move(r);
    }

    jsv_null::ref cjsruntime::new_null() {
        return permanents._null;
    }

    jsv_undefined::ref cjsruntime::new_undefined() {
        return permanents._undefined;
    }

    void cjsruntime::reuse_value(const js_value::ref &v) {
        if (!v)
            return;
        if (v->marked == 4)
            return;
        switch (v->get_type()) {
            case r_number:
                reuse.reuse_numbers.push_back(
                        std::dynamic_pointer_cast<jsv_number>(v));
                break;
            case r_string:
                reuse.reuse_strings.push_back(
                        std::dynamic_pointer_cast<jsv_string>(v)->clear());
                break;
            case r_boolean:
                reuse.reuse_booleans.push_back(
                        std::dynamic_pointer_cast<jsv_boolean>(v));
                break;
            case r_object:
                reuse.reuse_objects.push_back(
                        std::dynamic_pointer_cast<jsv_object>(v)->clear());
                break;
            case r_function:
                reuse.reuse_functions.push_back(
                        std::dynamic_pointer_cast<jsv_function>(v)->clear());
                break;
            default:
                break;
        }
    }

    void cjsruntime::gc() {
        std::for_each(objs.begin(), objs.end(), [](auto &x) { x->mark(0); });
        for (const auto &s : stack) {
            const auto &st = s->stack;
            const auto &env = s->envs;
            const auto &closure = s->closure;
            for (const auto &s2 : st) {
                if (s2.lock())
                    s2.lock()->mark(1);
            }
            for (const auto &s2 : env) {
                s2.second.lock()->mark(2);
            }
            for (const auto &s2 : closure) {
                s2.second.lock()->mark(2);
            }
        }
#if DUMP_STEP
        dump_step3();
#endif
        for (auto i = objs.begin(); i != objs.end();) {
            if ((*i)->marked == 0) {
                if (!((*i)->attr & js_value::at_const)) {
#if DUMP_GC
                    fprintf(stdout, " GC  | [%p] Reuse, ", (*i).get());
                    print(*i, 0, std::cout);
#endif
                    reuse_value(*i);
                }
                i = objs.erase(i);
            } else
                i++;
        }
#if DUMP_STEP
        std::cout << std::setfill('#') << std::setw(60) << "" << std::endl;
#endif
    }

    void cjsruntime::print(const js_value::ref &value, int level, std::ostream &os) {
        if (value == nullptr) {
            os << "undefined" << std::endl;
            return;
        }
        auto type = value->get_type();
        os << std::setfill(' ') << std::setw(level) << "";
        switch (type) {
            case r_number: {
                auto n = std::dynamic_pointer_cast<jsv_number>(value);
                os << "number: " << std::fixed << n->number << std::endl;
            }
                break;
            case r_string: {
                auto n = std::dynamic_pointer_cast<jsv_string>(value);
                os << "string: " << n->str << std::endl;
            }
                break;
            case r_boolean: {
                auto n = std::dynamic_pointer_cast<jsv_boolean>(value);
                os << "boolean: " << std::boolalpha << n->b << std::endl;
            }
                break;
            case r_object: {
                auto n = std::dynamic_pointer_cast<jsv_object>(value);
                os << "object: " << std::endl;
                for (const auto &s : n->obj) {
                    os << std::setfill(' ') << std::setw(level) << "";
                    os << s.first << ": " << std::endl;
                    print(s.second.lock(), level + 1, os);
                }
            }
                break;
            case r_function: {
                auto n = std::dynamic_pointer_cast<jsv_function>(value);
                if (n->builtin)
                    os << "function: builtin " << n->name << std::endl;
                else
                    os << "function: " << n->code->fullname << std::endl;
            }
                break;
            case r_null: {
                os << "null" << std::endl;
            }
                break;
            case r_undefined: {
                os << "undefined" << std::endl;
            }
                break;
            default:
                break;
        }
    }
}