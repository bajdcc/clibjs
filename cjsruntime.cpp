//
// Project: clibjs
// Created by bajdcc
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include "cjsruntime.h"
#include "cjsast.h"

#define DUMP_STEP 1

namespace clib {

    jsv_number::jsv_number(double n) : number(n) {

    }

    js_value::ref jsv_number::clone() const {
        return std::make_shared<jsv_number>(number);
    }

    runtime_t jsv_number::get_type() {
        return r_number;
    }

    js_value::ref jsv_number::binary_op(js_value_new &n, int code, js_value::ref op) {
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
                        if (s == 0.0)
                            return shared_from_this();
                        return n.new_number(
                                number +
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    }
                    case r_string: {
                        const auto &s = std::dynamic_pointer_cast<jsv_string>(op)->str;
                        if (s.empty())
                            return shared_from_this();
                        std::stringstream ss;
                        ss << number;
                        return n.new_string(
                                ss.str() +
                                std::dynamic_pointer_cast<jsv_string>(op)->str);
                    }
                    case r_boolean:
                        return std::dynamic_pointer_cast<jsv_boolean>(op)->b ?
                               n.new_number(
                                       number + 1.0) :
                               shared_from_this();
                    case r_regex:
                        break;
                    case r_array: {
                        const auto &s = std::dynamic_pointer_cast<jsv_array>(op);
                        std::stringstream ss;
                        ss << number;
                        auto r = ss.str();
                        if (s->arr.empty())
                            return n.new_string(r);
                        ss.str("");
                        s->print(ss);
                        auto _s = ss.str();
                        _s.erase(_s.begin());
                        _s.pop_back();
                        return n.new_string(r + _s);
                    }
                    case r_object: {
                        std::stringstream ss;
                        ss << number << "[object Object]";
                        return n.new_string(ss.str());
                    }
                    case r_function: {
                        const auto &s = std::dynamic_pointer_cast<jsv_function>(op)->code->text;
                        std::stringstream ss;
                        ss << number << s;
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

    bool jsv_number::to_bool() const {
        return number != 0.0;
    }

    void jsv_number::mark(int n) {
        marked = n;
    }

    void jsv_number::print(std::ostream &os) {
        os << number;
    }

    jsv_string::jsv_string(std::string s) : str(std::move(s)) {

    }

    js_value::ref jsv_string::clone() const {
        return std::make_shared<jsv_string>(str);
    }

    runtime_t jsv_string::get_type() {
        return r_string;
    }

    js_value::ref jsv_string::binary_op(js_value_new &n, int code, js_value::ref
    op) {
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
        return true;
    }

    void jsv_string::mark(int n) {
        marked = n;
    }

    void jsv_string::print(std::ostream &os) {
        os << str;
    }

    jsv_boolean::jsv_boolean(bool
                             flag) : b(flag) {

    }

    js_value::ref jsv_boolean::clone() const {
        return std::make_shared<jsv_boolean>(b);
    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    js_value::ref jsv_boolean::binary_op(js_value_new &n, int code, js_value::ref
    op) {
        return nullptr;
    }

    bool jsv_boolean::to_bool() const {
        return b;
    }

    void jsv_boolean::mark(int n) {
        marked = n;
    }

    void jsv_boolean::print(std::ostream &os) {
        os << std::boolalpha << b;
    }

    jsv_regex::jsv_regex(std::string
                         s) : re(std::move(s)) {

    }

    js_value::ref jsv_regex::clone() const {
        return std::make_shared<jsv_regex>(re);
    }

    runtime_t jsv_regex::get_type() {
        return r_regex;
    }

    js_value::ref jsv_regex::binary_op(js_value_new &n, int code, js_value::ref
    op) {
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

    js_value::ref jsv_array::clone() const {
        return nullptr;
    }

    runtime_t jsv_array::get_type() {
        return r_array;
    }

    js_value::ref jsv_array::binary_op(js_value_new &n, int code, js_value::ref
    op) {
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

    js_value::ref jsv_object::clone() const {
        return nullptr;
    }

    runtime_t jsv_object::get_type() {
        return r_object;
    }

    js_value::ref jsv_object::binary_op(js_value_new &n, int code, js_value::ref
    op) {
        return nullptr;
    }

    bool jsv_object::to_bool() const {
        return true;
    }

    void jsv_object::mark(int n) {
        marked = n;
    }

    void jsv_object::print(std::ostream &os) {
        os << "[object Object]";
    }

    jsv_object::ref jsv_object::clear() {
        obj.clear();
        return std::dynamic_pointer_cast<jsv_object>(shared_from_this());
    }

    jsv_function::jsv_function(sym_code_t::ref
                               c) {
        code = std::make_shared<cjs_function_info>(std::move(c));
    }

    js_value::ref jsv_function::clone() const {
        return nullptr;
    }

    runtime_t jsv_function::get_type() {
        return r_function;
    }

    js_value::ref jsv_function::binary_op(js_value_new &n, int code, js_value::ref
    op) {
        return nullptr;
    }

    bool jsv_function::to_bool() const {
        return true;
    }

    void jsv_function::mark(int n) {
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

    cjs_function::cjs_function(sym_code_t::ref
                               code) {
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
            case r_regex:
                return std::make_shared<jsv_regex>(*(std::string *) c.get_data(op));
            case r_array:
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

    cjsruntime::cjsruntime() {
        permanents._true = std::make_shared<jsv_boolean>(true);
        permanents._false = std::make_shared<jsv_boolean>(false);
    }

    void cjsruntime::eval(cjs_code_result::ref code) {
        stack.clear();
        stack.push_back(std::make_shared<cjs_function>(code->code));
        current_stack = stack.back();
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
            stack.pop_back();
            if (!stack.empty()) {
                current_stack = stack.back();
                push(ret);
            }
        }
        print(ret.lock(), 0, std::cout);
    }

    int cjsruntime::run(const sym_code_t::ref &fun, const cjs_code &code) {
        switch (code.code) {
            case LOAD_NULL:
                break;
            case LOAD_UNDEFINED:
                break;
            case LOAD_TRUE:
                push(permanents._true);
                break;
            case LOAD_FALSE:
                push(permanents._false);
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
                if (!(result == op1 || result == op2))
                    register_value(result);
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
                auto arr = std::make_shared<jsv_array>();
                for (auto i = 0; i < n; i++) {
                    arr->arr.push_back(pop());
                }
                push(register_value(std::move(arr)));
            }
                break;
            case BUILD_SET:
                break;
            case BUILD_MAP: {
                auto n = code.op1;
                assert(current_stack->stack.size() >= n * 2);
                auto obj = std::make_shared<jsv_object>();
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
                push(register_value(std::move(obj)));
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
            case JUMP_FORWARD:
                break;
            case JUMP_IF_FALSE_OR_POP:
                break;
            case JUMP_IF_TRUE_OR_POP:
                break;
            case JUMP_ABSOLUTE: {
                auto jmp = code.op1;
                current_stack->pc = jmp;
                return 0;
            }
            case POP_JUMP_IF_FALSE: {
                auto jmp = code.op1;
                auto t = pop();
                if (!(t.lock() && t.lock()->to_bool())) {
                    current_stack->pc = jmp;
                    return 0;
                }
            }
                break;
            case POP_JUMP_IF_TRUE:
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
                push(register_value(std::make_shared<jsv_string>(var)));
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
        return nullptr;
    }

    js_value::ref cjsruntime::load_fast(int op) {
        auto name = current_stack->info->names.at(op);
        auto L = current_stack->envs.find(name);
        if (L != current_stack->envs.end()) {
            return L->second.lock();
        }
        assert(!"cannot find value by name");
        return nullptr;
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
        return nullptr;
    }

    js_value::ref cjsruntime::load_global(int op) {
        auto g = current_stack->info->globals.at(op);
        auto G = stack.front()->envs.find(g);
        if (G != stack.front()->envs.end()) {
            return G->second.lock();
        }
        return nullptr;
    }

    js_value::ref cjsruntime::load_closure(const std::string &name) {
        for (auto i = stack.rbegin() + 1; i != stack.rend(); i++) {
            auto L = (*i)->envs.find(name);
            if (L != (*i)->envs.end()) {
                return L->second.lock();
            }
        }
        assert(!"cannot load closure value by name");
        return nullptr;
    }

    js_value::ref cjsruntime::load_deref(const std::string &name) {
        auto f = current_stack->closure.find(name);
        if (f != current_stack->closure.end()) {
            return f->second.lock();
        }
        assert(!"cannot load deref by name");
        return nullptr;
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
        std::cout << std::setfill('#') << std::setw(60) << "" << std::endl;
    }

    jsv_number::ref cjsruntime::new_number(double n) {
        if (reuse.reuse_numbers.empty()) {
            return std::make_shared<jsv_number>(n);
        }
        auto r = reuse.reuse_numbers.back();
        r->number = n;
        reuse.reuse_numbers.pop_back();
        return std::move(r);
    }

    jsv_string::ref cjsruntime::new_string(const std::string &s) {
        if (reuse.reuse_strings.empty()) {
            return std::make_shared<jsv_string>(s);
        }
        auto r = reuse.reuse_strings.back();
        r->str = s;
        reuse.reuse_strings.pop_back();
        return std::move(r);
    }

    jsv_boolean::ref cjsruntime::new_boolean(bool b) {
        if (b) return permanents._true;
        return permanents._false;
    }

    jsv_regex::ref cjsruntime::new_regex(const std::string &s) {
        if (reuse.reuse_regexes.empty()) {
            return std::make_shared<jsv_regex>(s);
        }
        auto r = reuse.reuse_regexes.back();
        r->re = s;
        reuse.reuse_regexes.pop_back();
        return std::move(r);
    }

    jsv_array::ref cjsruntime::new_array() {
        if (reuse.reuse_arrays.empty()) {
            return std::make_shared<jsv_array>();
        }
        auto r = reuse.reuse_arrays.back();
        reuse.reuse_arrays.pop_back();
        return std::move(r);
    }

    jsv_object::ref cjsruntime::new_object() {
        if (reuse.reuse_objects.empty()) {
            return std::make_shared<jsv_object>();
        }
        auto r = reuse.reuse_objects.back();
        reuse.reuse_objects.pop_back();
        return std::move(r);
    }

    jsv_function::ref cjsruntime::new_function() {
        if (reuse.reuse_functions.empty()) {
            return std::make_shared<jsv_function>();
        }
        auto r = reuse.reuse_functions.back();
        reuse.reuse_functions.pop_back();
        return std::move(r);
    }

    void cjsruntime::reuse_value(const js_value::ref &v) {
        if (!v)
            return;
        switch (v->get_type()) {
            case r_number:
                reuse.reuse_numbers.push_back(
                        std::dynamic_pointer_cast<jsv_number>(v));
                break;
            case r_string:
                reuse.reuse_strings.push_back(
                        std::dynamic_pointer_cast<jsv_string>(v));
                break;
            case r_boolean:
                reuse.reuse_booleans.push_back(
                        std::dynamic_pointer_cast<jsv_boolean>(v));
                break;
            case r_regex:
                reuse.reuse_regexes.push_back(
                        std::dynamic_pointer_cast<jsv_regex>(v)->clear());
                break;
            case r_array:
                reuse.reuse_arrays.push_back(
                        std::dynamic_pointer_cast<jsv_array>(v)->clear());
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
                if (s2.second.lock())
                    s2.second.lock()->mark(2);
            }
            for (const auto &s2 : closure) {
                if (s2.second.lock())
                    s2.second.lock()->mark(2);
            }
        }
#if DUMP_STEP
        dump_step3();
#endif
        for (auto i = objs.begin(); i != objs.end();) {
            if ((*i)->marked == 0) {
                reuse_value(*i);
                i = objs.erase(i);
            } else
                i++;
        }
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
                os << "number: " << n->number << std::endl;
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
            case r_regex: {
                auto n = std::dynamic_pointer_cast<jsv_regex>(value);
                os << "regex: " << n->re << std::endl;
            }
                break;
            case r_array: {
                auto n = std::dynamic_pointer_cast<jsv_array>(value);
                os << "array: " << std::endl;
                for (const auto &s : n->arr) {
                    print(s.lock(), level + 1, os);
                }
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
                os << "function: " << n->code->fullname << std::endl;
            }
                break;
            default:
                break;
        }
    }
}