//
// Project: clibjs
// Created by bajdcc
//

#include <cassert>
#include <iostream>
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

    jsv_string::jsv_string(std::string s) : str(std::move(s)) {

    }

    js_value::ref jsv_string::clone() const {
        return std::make_shared<jsv_string>(str);
    }

    runtime_t jsv_string::get_type() {
        return r_string;
    }

    jsv_boolean::jsv_boolean(bool flag) : b(flag) {

    }

    js_value::ref jsv_boolean::clone() const {
        return std::make_shared<jsv_boolean>(b);
    }

    runtime_t jsv_boolean::get_type() {
        return r_boolean;
    }

    jsv_regex::jsv_regex(std::string s) : re(std::move(s)) {

    }

    js_value::ref jsv_regex::clone() const {
        return std::make_shared<jsv_regex>(re);
    }

    runtime_t jsv_regex::get_type() {
        return r_regex;
    }

    js_value::ref jsv_array::clone() const {
        return nullptr;
    }

    runtime_t jsv_array::get_type() {
        return r_array;
    }

    js_value::ref jsv_object::clone() const {
        return nullptr;
    }

    runtime_t jsv_object::get_type() {
        return r_object;
    }

    js_value::ref jsv_function::clone() const {
        return nullptr;
    }

    runtime_t jsv_function::get_type() {
        return r_function;
    }

    cjs_function::cjs_function(sym_code_t::ref code, int pc) : code(code), pc(pc) {

    }

    void cjsruntime::eval(sym_code_t::ref code) {
        stack.clear();
        stack.push_back(std::make_shared<cjs_function>(code, 0));
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
                auto ret = run(code, c);
                if (ret != 0)
                    break;
            }
            auto ret = stack.back()->ret_value;
            stack.pop_back();
        }
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
                break;
            case BINARY_MULTIPLY:
                break;
            case BINARY_MODULO:
                break;
            case BINARY_ADD:
                break;
            case BINARY_SUBTRACT:
                break;
            case BINARY_SUBSCR:
                break;
            case BINARY_FLOOR_DIVIDE:
                break;
            case BINARY_TRUE_DIVIDE:
                break;
            case BINARY_INC:
                break;
            case BINARY_DEC:
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
            case STORE_NAME:
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
            case STORE_GLOBAL:
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
            case LOAD_GLOBAL:
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
            case MAKE_FUNCTION:
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
                break;
            default:
                break;
        }
        assert(!"invalid runtime type");
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
}