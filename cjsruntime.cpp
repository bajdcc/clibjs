//
// Project: clibjs
// Created by bajdcc
//

#include <cassert>
#include <iostream>
#include <iomanip>
#include "cjsruntime.h"

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

    js_value::ref jsv_number::binary_op(int code, js_value::ref op) {
        switch (code) {
            case BINARY_POWER:
                break;
            case BINARY_MULTIPLY:
                break;
            case BINARY_MODULO:
                break;
            case BINARY_ADD: {
                switch (op->get_type()) {
                    case r_number:
                        return std::make_shared<jsv_number>(
                                number +
                                std::dynamic_pointer_cast<jsv_number>(op)->number);
                    case r_string:
                        break;
                    case r_boolean:
                        break;
                    case r_regex:
                        break;
                    case r_array:
                        break;
                    case r_object:
                        break;
                    case r_function:
                        break;
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

    void jsv_number::mark(int n) {
        marked = n;
    }

    jsv_string::jsv_string(std::string s) : str(std::move(s)) {

    }

    js_value::ref jsv_string::clone() const {
        return std::make_shared<jsv_string>(str);
    }

    runtime_t jsv_string::get_type() {
        return r_string;
    }

    js_value::ref jsv_string::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_string::mark(int n) {
        marked = n;
    }

    jsv_boolean::jsv_boolean(bool flag) : b(flag) {

    }

    js_value::ref jsv_boolean::clone() const {
        return std::make_shared<jsv_boolean>(b);
    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    js_value::ref jsv_boolean::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_boolean::mark(int n) {
        marked = n;
    }

    jsv_regex::jsv_regex(std::string s) : re(std::move(s)) {

    }

    js_value::ref jsv_regex::clone() const {
        return std::make_shared<jsv_regex>(re);
    }

    runtime_t jsv_regex::get_type() {
        return r_regex;
    }

    js_value::ref jsv_regex::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_regex::mark(int n) {
        marked = n;
    }

    js_value::ref jsv_array::clone() const {
        return nullptr;
    }

    runtime_t jsv_array::get_type() {
        return r_array;
    }

    js_value::ref jsv_array::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_array::mark(int n) {
        marked = n;
    }

    js_value::ref jsv_object::clone() const {
        return nullptr;
    }

    runtime_t jsv_object::get_type() {
        return r_object;
    }

    js_value::ref jsv_object::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_object::mark(int n) {
        marked = n;
    }

    jsv_function::jsv_function(sym_code_t::weak_ref c) : code(std::move(c)) {

    }

    js_value::ref jsv_function::clone() const {
        return nullptr;
    }

    runtime_t jsv_function::get_type() {
        return r_function;
    }

    cjs_function::cjs_function(sym_code_t::ref code, int pc) : code(std::move(code)), pc(pc) {

    }

    void cjs_function::store_name(const std::string &name, js_value::weak_ref obj) {
        envs.insert({name, obj});
    }

    js_value::ref jsv_function::binary_op(int code, js_value::ref op) {
        return nullptr;
    }

    void jsv_function::mark(int n) {
        marked = n;
    }

    void cjsruntime::eval(sym_code_t::ref code) {
        stack.clear();
        stack.push_back(std::make_shared<cjs_function>(code, 0));
        decltype(stack.back()->ret_value) ret;
        while (!stack.empty()) {
            current_stack = stack.back();
            const auto &fun = stack.back()->code;
            const auto &codes = fun->codes;
            const auto &pc = current_stack->pc;
            while (true) {
                if (pc >= (int) codes.size())
                    break;
                const auto &c = codes.at(pc);
#if DUMP_STEP
                dump_step(c);
#endif
                auto r = run(code, c);
#if DUMP_STEP
                dump_step2(c);
#endif
                if (r != 0)
                    break;
            }
            ret = stack.back()->ret_value;
            stack.pop_back();
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
                break;
            case LOAD_FALSE:
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
                auto op2 = pop();
                auto op1 = pop();
                push(register_value(op1.lock()->binary_op(code.code, op2.lock())));
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
                break;
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
                auto name = fun->consts.get_name(id);
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
                auto name = fun->consts.get_global(id);
                stack.front()->store_name(name, obj);
            }
                break;
            case DELETE_GLOBAL:
                break;
            case LOAD_CONST: {
                auto op = code.op1;
                auto var = load_const(fun, op);
                push(var);
            }
                break;
            case LOAD_NAME:
                break;
            case BUILD_TUPLE:
                break;
            case BUILD_LIST:
                break;
            case BUILD_SET:
                break;
            case BUILD_MAP:
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
            case JUMP_ABSOLUTE:
                break;
            case POP_JUMP_IF_FALSE:
                break;
            case POP_JUMP_IF_TRUE:
                break;
            case LOAD_GLOBAL: {
                auto op = code.op1;
                auto var = load_global(fun, op);
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
            case LOAD_FAST:
                break;
            case STORE_FAST:
                break;
            case DELETE_FAST:
                break;
            case RAISE_VARARGS:
                break;
            case CALL_FUNCTION:
                break;
            case MAKE_FUNCTION: {
                auto name = pop();
                auto f = top();
                assert(f.lock()->get_type() == r_function);
                assert(name.lock()->get_type() == r_string);
                std::dynamic_pointer_cast<jsv_function>(f.lock())->name =
                        std::dynamic_pointer_cast<jsv_string>(name.lock());
            }
                break;
            case BUILD_SLICE:
                break;
            case LOAD_CLOSURE:
                break;
            case LOAD_DEREF:
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
                return 1;
        }
        current_stack->pc++;
        return 0;
    }

    js_value::ref cjsruntime::load_const(const sym_code_t::ref &fun, int op) {
        auto t = fun->consts.get_type(op);
        switch (t) {
            case r_number:
                return register_value(std::make_shared<jsv_number>(
                        *(double *) fun->consts.get_data(op)));
            case r_string:
                return register_value(std::make_shared<jsv_string>(
                        *(std::string *) fun->consts.get_data(op)));
            case r_boolean:
                break;
            case r_regex:
                return register_value(std::make_shared<jsv_regex>(
                        *(std::string *) fun->consts.get_data(op)));
            case r_array:
                break;
            case r_object:
                break;
            case r_function:
                return register_value(std::make_shared<jsv_function>(
                        *(sym_code_t::weak_ref *) fun->consts.get_data(op)));
            default:
                break;
        }
        assert(!"invalid runtime type");
        return nullptr;
    }

    js_value::ref cjsruntime::load_global(const sym_code_t::ref &fun, int op) {
        auto t = fun->consts.get_type(op);
        auto g = fun->consts.get_global(op);
        auto G = stack.front()->envs.find(g);
        if (G != stack.front()->envs.end()) {
            return G->second.lock();
        }
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

    void cjsruntime::dump_step(const cjs_code &c) {
        fprintf(stdout, "R [%04d] %s\n", current_stack->pc, c.desc.c_str());
    }

    void cjsruntime::dump_step2(const cjs_code &c) {
        if (!stack.empty() && !stack.front()->stack.empty())
            std::cout << std::setfill('=') << std::setw(60) << "" << std::endl;
        for (auto s = stack.rbegin(); s != stack.rend(); s++) {
            const auto &st = (*s)->stack;
            auto sti = (int) st.size();
            for (auto s2 = st.rbegin(); s2 != st.rend(); s2++) {
                fprintf(stdout, "%4d | [%p] ", sti--, s2->lock().get());
                print(s2->lock(), 0, std::cout);
            }
            const auto &env = (*s)->envs;
            for (const auto &e : env) {
                fprintf(stdout, " Env | [%p] \"%.100s\" ", e.second.lock().get(), e.first.c_str());
                print(e.second.lock(), 0, std::cout);
            }
            std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
        }
    }

    void cjsruntime::print(const js_value::ref &value, int level, std::ostream &os) {
        if (value == nullptr)
            return;
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
                os << "boolean: " << n->b << std::endl;
            }
                break;
            case r_regex: {
                auto n = std::dynamic_pointer_cast<jsv_regex>(value);
                os << "regex: " << n->re << std::endl;
            }
                break;
            case r_array:
                break;
            case r_object:
                break;
            case r_function:
                break;
            default:
                break;
        }
    }
}