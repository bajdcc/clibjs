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
#include "cjs.h"

#define DUMP_STEP 0
#define DUMP_ENV 1
#define DUMP_CLOSURE 0
#define DUMP_GC 0
#define SHOW_EXTRA 1
#define GC_PERIOD 128

namespace clib {

    std::string trim(std::string s) {
        if (s.empty()) {
            return s;
        }
        s.erase(0, s.find_first_not_of(' '));
        s.erase(s.find_last_not_of(' ') + 1);
        return s;
    }

    double fix(const double &d) {
        if (std::isinf(d) || std::isnan(d))
            return 0.0;
        if (d == 0) {
            return d;
        }
        if (d >= 0)return floor(d);
        return ceil(d);
    }

    void cjsruntime::eval(cjs_code_result::ref code, bool top) {
        if (code->code->codes.empty()) {
            std::cout << "Compile failed." << std::endl;
            if (!top)
                push(new_undefined());
            return;
        }
        if (top) {
            stack.clear();
            auto top_stack = std::make_shared<cjs_function>(code->code, *this);
            top_stack->envs = new_object();
            top_stack->_this = top_stack->envs;
            stack.push_back(top_stack);
            current_stack = stack.back();
            current_stack->envs = permanents.global_env;
            current_stack->_this = permanents.global_env;
        } else {
            auto exec_stack = std::make_shared<cjs_function>(code->code, *this);
            exec_stack->envs = new_object();
            exec_stack->_this = stack.front()->envs;
            stack.push_back(exec_stack);
            current_stack = stack.back();
            return;
        }
        code = nullptr;
        auto r = 0;
        auto gc_period = 0;
        decltype(stack.back()->ret_value) ret;
        while (!stack.empty()) {
            const auto &codes = current_stack->info->codes;
            const auto &pc = current_stack->pc;
            while (true) {
                if (pc >= (int) codes.size()) {
                    r = 4;
                    break;
                }
                const auto &c = codes.at(pc);
                if (pc + 1 == (int) codes.size() && c.code == POP_TOP) {
                    r = 2;
                    break;
                }
#if DUMP_STEP
                dump_step(c);
#endif
                r = run(code->code, c);
#if DUMP_STEP && SHOW_EXTRA
                dump_step2(c);
#endif
                if (gc_period++ >= GC_PERIOD) {
                    gc_period = 0;
                    gc();
                }
                if (r != 0)
                    break;
            }
            if (r == 1) {
                current_stack = stack.back();
                continue;
            }
            if (r == 2) {
                if (!current_stack->ret_value.lock())
                    current_stack->ret_value =
                            current_stack->stack.empty() ? new_undefined() : pop();
            }
            if (r == 3) {
                continue;
            }
            if (r == 4) {
                current_stack->ret_value = new_undefined();
            }
            ret = stack.back()->ret_value;
            if (stack.size() > 1) {
                delete_stack(current_stack);
                stack.pop_back();
                current_stack = stack.back();
                push(ret);
            } else {
                delete_stack(current_stack);
                stack.pop_back();
            }
        }
        assert(ret.lock());
        ret.lock()->print(std::cout);
        std::cout << std::endl;
    }

    void cjsruntime::set_readonly(bool flag) {
        readonly = flag;
    }

    int cjsruntime::run(const sym_code_t::ref &fun, const cjs_code &code) {
        switch (code.code) {
            case LOAD_EMPTY: {
                js_value::ref v;
                push(v);
            }
                break;
            case LOAD_NULL:
                push(new_null());
                break;
            case LOAD_UNDEFINED:
                push(new_undefined());
                break;
            case LOAD_TRUE:
                push(new_boolean(true));
                break;
            case LOAD_FALSE:
                push(new_boolean(false));
                break;
            case LOAD_ZERO:
                push(code.op1 == 0 ? permanents._zero : permanents._minus_zero);
                break;
            case LOAD_THIS: {
                auto failed = true;
                for (auto i = stack.rbegin(); i != stack.rend(); i++) {
                    if (!(*i)->info->arrow) {
                        push((*i)->_this);
                        failed = false;
                        break;
                    }
                }
                if (failed)
                    push(new_undefined());
            }
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
            case INSTANCE_OF: {
                auto op2 = pop().lock();
                auto op1 = pop().lock();
                if (op1->get_type() != r_object) {
                    push(new_boolean(false));
                    break;
                }
                if (op2->get_type() != r_function) {
                    push(new_boolean(false));
                    break;
                }
                auto f = JS_OBJ(op2);
                auto ff = f.find("prototype");
                if (ff == f.end()) {
                    push(new_boolean(false));
                    break;
                }
                auto proto = op1->__proto__.lock();
                if (!proto) {
                    push(new_boolean(false));
                    break;
                }
                auto p = proto;
                auto failed = true;
                while (p) {
                    assert(p->get_type() == r_object);
                    if (p == ff->second.lock()) {
                        failed = false;
                        break;
                    }
                    p = p->__proto__.lock();
                }
                push(new_boolean(!failed));
            }
                break;
            case UNARY_POSITIVE:
            case UNARY_NEGATIVE:
            case UNARY_NOT:
            case UNARY_INVERT:
            case UNARY_NEW:
            case UNARY_DELETE:
            case UNARY_TYPEOF: {
                auto op1 = pop().lock();
                auto result = op1->unary_op(*this, code.code);
                assert(result);
                push(result);
            }
                break;
            case BINARY_MATRIX_MULTIPLY:
                break;
            case INPLACE_MATRIX_MULTIPLY:
                break;
            case COMPARE_LESS:
            case COMPARE_LESS_EQUAL:
            case COMPARE_EQUAL:
            case COMPARE_NOT_EQUAL:
            case COMPARE_GREATER:
            case COMPARE_GREATER_EQUAL:
            case COMPARE_FEQUAL:
            case COMPARE_FNOT_EQUAL:
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
            case BINARY_SUBSCR: {
                auto n = code.op1;
                auto key = pop().lock()->to_string();
                auto obj = pop().lock();
                if (obj->get_type() == r_object || obj->get_type() == r_function) {
                    const auto &o = JS_OBJ(obj);
                    auto f = o.find(key);
                    if (f != o.end()) {
                        push(f->second);
                        break;
                    }
                }
                if (key == "__proto__") {
                    auto p = obj->__proto__.lock();
                    push(p ? p : permanents._null);
                    break;
                }
                auto proto = obj->__proto__.lock();
                if (!proto) {
                    push(new_undefined()); // type error
                    break;
                }
                auto p = proto;
                auto failed = true;
                while (p) {
                    assert(p->get_type() == r_object);
                    const auto &o = JS_OBJ(p);
                    auto f = o.find(key);
                    if (f != o.end()) {
                        push(f->second);
                        failed = false;
                        break;
                    }
                    p = p->__proto__.lock();
                }
                if (failed)
                    push(new_undefined());
            }
                break;
            case BINARY_INC:
            case BINARY_DEC: {
                auto op1 = pop().lock();
                auto result = op1->binary_op(
                        *this,
                        code.code == BINARY_INC ? BINARY_ADD : BINARY_SUBTRACT,
                        permanents._one);
                assert(result);
                push(result);
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
            case STORE_SUBSCR: {
                auto key = pop().lock()->to_string();
                auto obj = pop().lock();
                auto value = top();
                auto &o = JS_OBJ(obj);
                if (obj->get_type() == r_object || obj->get_type() == r_function) {
                    auto f = o.find(key);
                    if (f != o.end() && (readonly && (f->second.lock()->attr & js_value::at_readonly))) {
                        break;
                    }
                    o[key] = std::move(value);
                    break;
                }
            }
                break;
            case DELETE_SUBSCR:
                break;
            case INPLACE_POWER:
                break;
            case GET_ITER: {
                auto obj = top().lock();
                if (obj->get_type() == r_object) {
                    auto &o = JS_OBJ(obj);
                    pop();
                    auto arr = new_array();
                    if (obj->__proto__.lock() != permanents._proto_array) {
                        std::vector<std::string> ar(o.size());
                        std::transform(o.begin(), o.end(), ar.begin(), [](auto &x) { return x.first; });
                        for (size_t i = 0; i < ar.size(); i++) {
                            std::stringstream ss;
                            ss << i;
                            arr->obj[ss.str()] = new_string(ar[i]);
                        }
                        arr->obj["length"] = new_number(ar.size());
                        push(arr);
                    } else {
                        auto f = o.find("length");
                        if (f != o.end()) {
                            auto len = f->second.lock();
                            auto i = 0, j = 0;
                            if (len->get_type() == r_number) {
                                auto l = JS_NUM(len);
                                if (!std::isinf(l) && !std::isnan(l)) {
                                    auto L = (int) l;
                                    std::stringstream ss;
                                    while (i < L) {
                                        ss.str("");
                                        ss << j++;
                                        auto ff = o.find(ss.str());
                                        if (ff != o.end()) {
                                            arr->obj[ss.str()] = new_string(ff->first);
                                        }
                                        i++;
                                    }
                                }
                            }
                            arr->obj["length"] = new_number(j);
                        } else {
                            arr->obj["length"] = new_number(0.0);
                        }
                        push(arr);
                    }
                    push(new_number(0.0));
                } else {
                    assert(!"invalid iter");
                }
            }
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
            case UNPACK_SEQUENCE: {
                auto obj = pop().lock();
                assert(obj->get_type() == r_object);
                const auto &o = JS_OBJ(obj);
                auto f = o.find("length");
                if (f != o.end()) {
                    auto len = f->second.lock();
                    if (len->get_type() == r_number) {
                        auto l = JS_NUM(len);
                        if (!std::isinf(l) && !std::isnan(l)) {
                            for (auto i = 0; i < l; i++) {
                                std::stringstream ss;
                                ss << i;
                                auto ff = o.find(ss.str());
                                if (ff != o.end()) {
                                    push(ff->second);
                                }
                            }
                            break;
                        }
                    }
                }
                push(obj); // failed
            }
                break;
            case FOR_ITER: {
                auto jmp = code.op1;
                auto idx = pop().lock();
                assert(idx->get_type() == r_number);
                auto i = JS_NUM(idx);
                assert(!std::isinf(i) && !std::isnan(i));
                auto obj = top().lock();
                assert(obj->get_type() == r_object && obj->__proto__.lock() == permanents._proto_array);
                const auto &o = JS_OBJ(obj);
                auto f = o.find("length");
                if (f != o.end()) {
                    auto len = f->second.lock();
                    if (len->get_type() == r_number) {
                        auto l = JS_NUM(len);
                        if (!std::isinf(l) && !std::isnan(l)) {
                            if (i < l) {
                                std::stringstream ss;
                                auto failed = true;
                                while (i < l) {
                                    ss.str("");
                                    ss << i;
                                    auto ff = o.find(ss.str());
                                    if (ff != o.end()) {
                                        push(new_number(i + 1));
                                        push(ff->second);
                                        failed = false;
                                        break;
                                    }
                                    i++;
                                }
                                if (failed) {
                                    current_stack->pc += jmp;
                                    pop();
                                    return 0;
                                }
                                break;
                            }
                            current_stack->pc += jmp;
                            pop();
                            return 0;
                        }
                    }
                }
                assert(!"invalid iter");
            }
                break;
            case UNPACK_EX:
                break;
            case STORE_ATTR: {
                auto n = code.op1;
                auto key = current_stack->info->names.at(n);
                auto obj = pop().lock();
                auto value = top();
                auto &o = JS_OBJ(obj);
                if (obj->get_type() == r_object || obj->get_type() == r_function) {
                    auto f = o.find(key);
                    if (f != o.end() && (readonly && (f->second.lock()->attr & js_value::at_readonly))) {
                        break;
                    }
                    o[key] = std::move(value);
                    break;
                }
            }
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
                if (n == -1) {
                    assert(!current_stack->rests.empty());
                    n = (int) current_stack->stack.size() - current_stack->rests.back();
                    current_stack->rests.pop_back();
                }
                assert(current_stack->stack.size() >= n);
                auto obj = new_array();
                std::stringstream ss;
                for (auto i = n - 1; i >= 0; i--) {
                    auto v = pop().lock();
                    if (v) {
                        ss.str("");
                        ss << i;
                        obj->obj[ss.str()] = v;
                    }
                }
                obj->obj["length"] = new_number(n);
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
                        obj->obj.insert({JS_STR(k), v});
                    } else if (k->get_type() == r_number) {
                        std::stringstream ss;
                        ss << JS_NUM(k);
                        obj->obj.insert({ss.str(), v});
                    } else {
                        continue;
                    }
                }
                push(obj);
            }
                break;
            case LOAD_ATTR: {
                auto n = code.op1;
                auto key = current_stack->info->names.at(n);
                auto obj = pop().lock();
                if (obj->get_type() == r_object || obj->get_type() == r_function) {
                    const auto &o = JS_OBJ(obj);
                    auto f = o.find(key);
                    if (f != o.end()) {
                        push(f->second);
                        break;
                    }
                }
                if (key == "__proto__") {
                    auto p = obj->__proto__.lock();
                    push(p ? p : permanents._null);
                    break;
                }
                auto proto = obj->__proto__.lock();
                if (!proto) {
                    push(new_undefined()); // type error
                    break;
                }
                auto p = proto;
                auto failed = true;
                while (p) {
                    assert(p->get_type() == r_object);
                    const auto &o = JS_OBJ(p);
                    auto f = o.find(key);
                    if (f != o.end()) {
                        push(f->second);
                        failed = false;
                        break;
                    }
                    p = p->__proto__.lock();
                }
                if (failed)
                    push(new_undefined());
            }
                break;
            case IMPORT_NAME:
                break;
            case IMPORT_FROM:
                break;
            case JUMP_FORWARD: {
                auto jmp = code.op1;
                current_stack->pc += jmp;
                return 0;
            }
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
                if (n == -1) {
                    assert(!current_stack->rests.empty());
                    n = (int) current_stack->stack.size() - current_stack->rests.back();
                    current_stack->rests.pop_back();
                }
                assert((int) current_stack->stack.size() > n);
                std::vector<js_value::weak_ref> args;
                args.resize(n);
                auto m = n;
                while (m-- > 0) {
                    args[m] = pop();
                }
                auto f = pop();
                if (f.lock()->get_type() != r_function) {
                    push(new_undefined());
                    break;
                }
                auto func = JS_FUN(f.lock());
                if (func->builtin) {
                    js_value::weak_ref _this = JS_V(stack.front()->envs.lock());
                    func->builtin(current_stack, _this, args, *this, 0);
                    break;
                }
                stack.push_back(new_stack(func->code));
                auto new_stack = stack.back();
                auto env = new_stack->envs.lock();
                if (!func->code->arrow && func->name.front() != '<')
                    env->obj[func->name] = f;
                auto arg = new_object();
                env->obj["arguments"] = arg;
                size_t i = 0;
                auto args_num = func->code->args_num;
                for (; i < n; i++) {
                    std::stringstream ss;
                    ss << i;
                    arg->obj[ss.str()] = args.at(i);
                    if (i < args_num)
                        env->obj[func->code->args.at(i)] = args.at(i);
                }
                for (; i < args_num; i++) {
                    env->obj[func->code->args.at(i)] = new_undefined();
                }
                if (func->code->rest) {
                    auto rest = new_array();
                    env->obj[func->code->args.at(args_num)] = rest;
                    auto j = 0;
                    for (i = args_num; i < n; i++) {
                        std::stringstream ss;
                        ss << j++;
                        rest->obj[ss.str()] = args.at(i);
                    }
                    rest->obj["length"] = new_number(j);
                }
                arg->obj["length"] = new_number(n);
                if (func->closure.lock())
                    new_stack->closure = func->closure;
                current_stack->pc++;
                return 1;
            }
            case MAKE_FUNCTION: {
                auto op = (uint32_t) code.op1;
                if (op & 8U) {
                    auto name = pop();
                    auto f = pop();
                    auto closure = pop();
                    if (f.lock()->get_type() != r_function) {
                        push(new_undefined());
                        break;
                    }
                    assert(name.lock()->get_type() == r_string);
                    assert(closure.lock()->get_type() == r_object);
                    auto parent = JS_FUN(f.lock());
                    auto func = new_function();
                    func->code = parent->code;
                    func->name = JS_STR(name.lock());
                    if (closure.lock()) {
                        auto c = new_object();
                        c->obj = JS_OBJ(closure.lock());
                        func->closure = c;
                    }
                    auto len = new_number(func->code->args_num);
                    func->obj.insert({"length", len});
                    push(func);
                } else {
                    auto name = pop();
                    auto f = pop();
                    if (f.lock()->get_type() != r_function) {
                        push(new_undefined());
                        break;
                    }
                    assert(name.lock()->get_type() == r_string);
                    auto parent = JS_FUN(f.lock());
                    auto func = new_function();
                    func->code = parent->code;
                    func->name = JS_STR(name.lock());
                    auto len = new_number(func->code->args_num);
                    func->obj.insert({"length", len});
                    push(func);
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
            case REST_ARGUMENT:
                current_stack->rests.push_back(current_stack->stack.size());
                break;
            case CALL_FUNCTION_EX: {
                auto n = code.op1;
                if (n == -1) {
                    assert(!current_stack->rests.empty());
                    n = (int) current_stack->stack.size() - current_stack->rests.back();
                    current_stack->rests.pop_back();
                }
                assert((int) current_stack->stack.size() > n);
                std::vector<js_value::weak_ref> args(n);
                auto m = n;
                while (m-- > 0) {
                    args[m] = pop();
                }
                auto f = pop();
                if (f.lock()->get_type() != r_function) {
                    push(new_undefined());
                    break;
                }
                auto func = JS_FUN(f.lock());
                auto _this = new_object();
                auto prototype = func->obj["prototype"].lock();
                if (!prototype)
                    _this->__proto__ = permanents._proto_object;
                else
                    _this->__proto__ = prototype;
                if (func->builtin) {
                    js_value::weak_ref _t = _this;
                    auto r = func->builtin(current_stack, _t, args, *this, jsv_function::at_new_function);
                    if (r != 0)
                        return r;
                    break;
                }
                auto p = _this->__proto__.lock();
                js_value::weak_ref constructor;
                while (p) {
                    if (p->get_type() != r_object) {
                        push(new_undefined()); // type error
                        break;
                    }
                    auto ob = JS_OBJ(p);
                    auto cons = ob.find("constructor");
                    if (cons != ob.end()) {
                        if (cons->second.lock()->get_type() == r_function) {
                            constructor = cons->second;
                        }
                        break;
                    }
                    p = p->__proto__.lock();
                }
                if (!constructor.lock()) {
                    push(new_undefined()); // no constructor error
                    break;
                }
                if (constructor.lock()->get_type() != r_function) {
                    push(new_undefined()); // type error
                    break;
                }
                auto cons_func = JS_FUN(constructor.lock());
                if (cons_func->builtin) {
                    js_value::weak_ref _t = _this;
                    auto r = cons_func->builtin(current_stack, _t, args, *this, jsv_function::at_new_function);
                    if (r != 0)
                        return r;
                    break;
                }
                stack.push_back(new_stack(cons_func->code));
                auto new_stack = stack.back();
                new_stack->_this = _this;
                new_stack->ret_value = _this;
                auto env = new_stack->envs.lock();
                if (!cons_func->code->arrow && cons_func->name.front() != '<')
                    env->obj[cons_func->name] = f;
                auto arg = new_object();
                env->obj["arguments"] = arg;
                size_t i = 0;
                for (; i < n; i++) {
                    std::stringstream ss;
                    ss << i;
                    arg->obj[ss.str()] = args.at(i);
                    if (i < cons_func->code->args.size())
                        env->obj[cons_func->code->args.at(i)] = args.at(i);
                }
                for (; i < cons_func->code->args.size(); i++) {
                    env->obj[cons_func->code->args.at(i)] = new_undefined();
                }
                arg->obj["length"] = new_number(n);
                if (cons_func->closure.lock())
                    new_stack->closure = cons_func->closure;
                current_stack->pc++;
                return 1;
            }
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
            case LOAD_METHOD: {
                auto n = code.op1;
                auto key = current_stack->info->names.at(n);
                auto obj = top();
                if (obj.lock()->get_type() == r_object || obj.lock()->get_type() == r_function) {
                    auto o = JS_OBJ(obj.lock());
                    auto f = o.find(key);
                    if (f != o.end()) {
                        if (f->second.lock()->get_type() == r_function)
                            push(f->second);
                        else
                            push(new_undefined()); // type error
                        break;
                    }
                }
                auto proto = obj.lock()->__proto__.lock();
                if (!proto) {
                    push(new_undefined()); // type error
                    break;
                }
                auto p = proto;
                auto failed = true;
                while (p) {
                    if (p->get_type() != r_object) {
                        push(new_undefined()); // type error
                        break;
                    }
                    auto ob = JS_OBJ(p);
                    auto f = ob.find(key);
                    if (f != ob.end()) {
                        if (f->second.lock()->get_type() == r_function) {
                            failed = false;
                            push(f->second);
                        }
                        break;
                    }
                    p = p->__proto__.lock();
                }
                if (failed)
                    push(new_undefined()); // type error
            }
                break;
            case CALL_METHOD: {
                auto n = code.op1;
                if (n == -1) {
                    assert(!current_stack->rests.empty());
                    n = (int) current_stack->stack.size() - current_stack->rests.back();
                    current_stack->rests.pop_back();
                }
                assert((int) current_stack->stack.size() > n);
                std::vector<js_value::weak_ref> args(n);
                auto m = n;
                while (m-- > 0) {
                    args[m] = pop();
                }
                auto f = pop();
                auto _this = pop().lock();
                if (f.lock()->get_type() != r_function) {
                    push(new_undefined());
                    break;
                }
                auto func = JS_FUN(f.lock());
                if (func->builtin) {
                    js_value::weak_ref t = _this;
                    auto r = func->builtin(current_stack, t, args, *this, 0);
                    if (r != 0)
                        return r;
                    break;
                }
                stack.push_back(new_stack(func->code));
                auto new_stack = stack.back();
                new_stack->_this = _this;
                new_stack->name = func->name;
                auto env = new_stack->envs.lock();
                if (!func->code->arrow && func->name.front() != '<')
                    env->obj[func->name] = f;
                auto arg = new_object();
                env->obj["arguments"] = arg;
                size_t i = 0;
                auto args_num = func->code->args_num;
                for (; i < n; i++) {
                    std::stringstream ss;
                    ss << i;
                    arg->obj[ss.str()] = args.at(i);
                    if (i < func->code->args.size())
                        env->obj[func->code->args.at(i)] = args.at(i);
                }
                for (; i < func->code->args.size(); i++) {
                    env->obj[func->code->args.at(i)] = new_undefined();
                }
                if (func->code->rest) {
                    auto rest = new_array();
                    env->obj[func->code->args.at(args_num)] = rest;
                    auto j = 0;
                    for (i = args_num; i < n; i++) {
                        std::stringstream ss;
                        ss << j++;
                        rest->obj[ss.str()] = args.at(i);
                    }
                    rest->obj["length"] = new_number(j);
                }
                arg->obj["length"] = new_number(n);
                if (func->closure.lock())
                    new_stack->closure = func->closure;
                current_stack->pc++;
                return 1;
            }
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
        auto L = current_stack->envs.lock()->obj.find(name);
        if (L != current_stack->envs.lock()->obj.end()) {
            return L->second.lock();
        }
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_name(int op) {
        auto name = current_stack->info->names.at(op);
        for (auto i = stack.rbegin(); i != stack.rend(); i++) {
            auto L = (*i)->envs.lock()->obj.find(name);
            if (L != (*i)->envs.lock()->obj.end()) {
                return L->second.lock();
            }
        }
        assert(!"cannot find value by name");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_global(int op) {
        auto g = current_stack->info->globals.at(op);
        auto G = stack.front()->envs.lock()->obj.find(g);
        if (G != stack.front()->envs.lock()->obj.end()) {
            return G->second.lock();
        }
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_closure(const std::string &name) {
        for (auto i = stack.rbegin(); i != stack.rend(); i++) {
            if ((*i)->closure.lock()) {
                auto L = (*i)->closure.lock()->obj.find(name);
                if (L != (*i)->closure.lock()->obj.end()) {
                    return L->second.lock();
                }
            }
            auto L2 = (*i)->envs.lock()->obj.find(name);
            if (L2 != (*i)->envs.lock()->obj.end()) {
                return L2->second.lock();
            }
        }
        assert(!"cannot load closure value by name");
        return permanents._undefined;
    }

    js_value::ref cjsruntime::load_deref(const std::string &name) {
        if (current_stack->closure.lock()) {
            auto f = current_stack->closure.lock()->obj.find(name);
            if (f != current_stack->closure.lock()->obj.end()) {
                return f->second.lock();
            }
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
            fprintf(stdout, "**** Stack [%p] \"%.100s\" '%.100s'\n",
                    s->get(), (*s)->name.c_str(),
                    (*s)->info ? (*s)->info->text.c_str() : "[builtin]");
            const auto &st = (*s)->stack;
            auto sti = (int) st.size();
            for (auto s2 = st.rbegin(); s2 != st.rend(); s2++) {
                fprintf(stdout, "%4d | [%p] ", sti--, s2->lock().get());
                if (s2->lock() == permanents.global_env)
                    fprintf(stdout, "<global env>\n");
                else
                    print(s2->lock(), 0, std::cout);
            }
#if DUMP_ENV
            const auto &env = (*s)->envs.lock()->obj;
            if (!env.empty()) {
                std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
                for (const auto &e : env) {
                    fprintf(stdout, " Env | [%p] \"%.100s\" '%.100s' ",
                            e.second.lock().get(), e.first.c_str(),
                            e.second.lock()->to_string().c_str());
                    if (e.second.lock() == permanents.global_env)
                        fprintf(stdout, "<global env>\n");
                    else
                        print(e.second.lock(), 0, std::cout);
                }
            }
#endif
#if DUMP_CLOSURE
            if ((*s)->closure.lock()) {
                const auto &cl = (*s)->closure.lock()->obj;
                std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
                for (const auto &e : cl) {
                    fprintf(stdout, " Clo | [%p] \"%.100s\" '%.100s' ",
                            e.second.lock().get(), e.first.c_str(),
                            e.second.lock()->to_string().c_str());
                    print(e.second.lock(), 0, std::cout);
                }
            }
            std::cout << std::setfill('-') << std::setw(60) << "" << std::endl;
#endif
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
            auto s = _new_number(n);
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_numbers.back();
        register_value(r);
        r->number = n;
        r->__proto__ = permanents._proto_number;
        reuse.reuse_numbers.pop_back();
        return std::move(r);
    }

    jsv_string::ref cjsruntime::new_string(const std::string &s) {
        if (s.empty())
            return permanents._empty;
        if (reuse.reuse_strings.empty()) {
            auto t = _new_string(s);
            register_value(t);
            return t;
        }
        auto r = reuse.reuse_strings.back();
        register_value(r);
        r->str = s;
        r->__proto__ = permanents._proto_string;
        reuse.reuse_strings.pop_back();
        return std::move(r);
    }

    jsv_boolean::ref cjsruntime::new_boolean(bool b) {
        if (b) return permanents._true;
        return permanents._false;
    }

    jsv_object::ref cjsruntime::new_object() {
        if (reuse.reuse_objects.empty()) {
            auto s = _new_object();
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_objects.back();
        register_value(r);
        r->__proto__ = permanents._proto_object;
        reuse.reuse_objects.pop_back();
        return std::move(r);
    }

    jsv_object::ref cjsruntime::new_object_box(const js_value::ref &v) {
        auto obj = new_object();
        switch (v->get_type()) {
            case r_number:
                obj->__proto__ = permanents._proto_number;
                break;
            case r_string:
                obj->__proto__ = permanents._proto_string;
                break;
            case r_boolean:
                obj->__proto__ = permanents._proto_boolean;
                break;
            case r_function:
                obj->__proto__ = permanents._proto_function;
                break;
            default:
                break;
        }
        obj->special.insert({"PrimitiveValue", v});
        return obj;
    }

    jsv_function::ref cjsruntime::new_function() {
        if (reuse.reuse_functions.empty()) {
            auto s = _new_function(nullptr);
            register_value(s);
            return s;
        }
        auto r = reuse.reuse_functions.back();
        register_value(r);
        r->__proto__ = permanents._proto_function;
        reuse.reuse_functions.pop_back();
        return std::move(r);
    }

    jsv_null::ref cjsruntime::new_null() {
        return permanents._null;
    }

    jsv_undefined::ref cjsruntime::new_undefined() {
        return permanents._undefined;
    }

    cjs_function::ref cjsruntime::new_func(const cjs_function_info::ref &code) {
        auto f = new_stack(code);
        stack.push_back(f);
        return f;
    }

    jsv_object::ref cjsruntime::new_array() {
        auto arr = new_object();
        arr->__proto__ = permanents._proto_array;
        arr->obj["length"] = new_number(0.0);
        return arr;
    }

    void cjsruntime::exec(const std::string &n, const std::string &s) {
        ((cjs *) pjs)->exec(n, s, false);
    }

    std::string cjsruntime::get_stacktrace() const {
        std::stringstream ss;
        auto j = stack.size();
        for (auto i = stack.rbegin(); i != stack.rend(); i++) {
            ss << j-- << ": " << (*i)->name;
            if (!(*i)->info)
                ss << " (builtin)";
            ss << std::endl;
        }
        auto s = ss.str();
        if (!s.empty())
            s.pop_back();
        return s;
    }

    bool cjsruntime::to_number(const js_value::ref &obj, double &d) {
        switch (obj->get_type()) {
            case r_number:
                d = JS_NUM(obj);
                return true;
            case r_string: {
                switch (JS_STR2NUM(obj, d)) {
                    case 0:
                    case 1:
                        d = 0.0;
                        return true;
                    case 2:
                        return true;
                    case 3:
                        return false;
                    default:
                        break;
                }
            }
                break;
            case r_boolean:
                d = JS_BOOL(obj) ? 1.0 : 0.0;
                break;
            case r_function:
            case r_object:
            case r_null:
            case r_undefined:
                break;
            default:
                assert(!"invalid number");
                break;
        }
        return false;
    }

    std::vector<js_value::weak_ref> cjsruntime::to_array(const js_value::ref &f) {
        std::vector<std::weak_ptr<js_value>> ret;
        if (f->get_type() != r_object) {
            return ret;
        }
        const auto &obj = JS_OBJ(f);
        auto l = obj.find("length");
        if (l == obj.end()) {
            return ret;
        }
        auto len = l->second.lock();
        auto length = 0;
        double d = 0.0;
        if (to_number(len, d)) {
            if (!(std::isinf(d) && std::isnan(d)))
                length = std::floor(d);
        }
        for (auto i = 0; i < length; i++) {
            std::stringstream ss;
            ss << i;
            auto ff = obj.find(ss.str());
            if (ff != obj.end()) {
                ret.push_back(ff->second);
            }
        }
        return ret;
    }

    void cjsruntime::reuse_value(const js_value::ref &v) {
        if (!v)
            return;
        v->__proto__.reset();
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
                        std::dynamic_pointer_cast<jsv_function>(v)->clear2());
                break;
            default:
                break;
        }
    }

    cjs_function::ref cjsruntime::new_stack(const sym_code_t::ref &code) {
        if (reuse_stack.empty()) {
            auto st = std::make_shared<cjs_function>(code, *this);
            st->envs = new_object();
            st->_this = stack.front()->envs;
            return st;
        } else {
            auto st = reuse_stack.back();
            st->envs = new_object();
            reuse_stack.pop_back();
            st->reset(code, *this);
            return st;
        }
    }

    cjs_function::ref cjsruntime::new_stack(const cjs_function_info::ref &code) {
        if (reuse_stack.empty()) {
            auto st = std::make_shared<cjs_function>(code);
            st->envs = new_object();
            st->_this = stack.front()->envs;
            return st;
        } else {
            auto st = reuse_stack.back();
            st->envs = new_object();
            reuse_stack.pop_back();
            st->reset(code);
            return st;
        }
    }

    void cjsruntime::delete_stack(const cjs_function::ref &f) {
        f->clear();
        reuse_stack.push_back(f);
    }

    void cjsruntime::gc() {
        permanents.global_env->mark(0);
        std::for_each(objs.begin(), objs.end(), [](auto &x) { x->mark(0); });
        for (const auto &s : stack) {
            const auto &st = s->stack;
            const auto &ret = s->ret_value.lock();
            const auto &th = s->_this.lock();
            const auto &env = s->envs.lock();
            const auto &closure = s->closure.lock();
            for (const auto &s2 : st) {
                if (s2.lock())
                    s2.lock()->mark(1);
            }
            env->mark(2);
            if (closure)
                closure->mark(2);
            if (th)
                th->mark(3);
            if (ret)
                th->mark(4);
        }
#if DUMP_STEP && DUMP_GC
        dump_step3();
#endif
        for (auto i = objs.begin(); i != objs.end();) {
            if ((*i)->marked == 0) {
                if (!((*i)->attr & js_value::at_const)) {
#if DUMP_STEP && DUMP_GC
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

    jsv_number::ref cjsruntime::_new_number(double n, uint32_t attr) {
        auto s = std::make_shared<jsv_number>(n);
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_number;
        return s;
    }

    jsv_string::ref cjsruntime::_new_string(const std::string &str, uint32_t attr) {
        auto s = std::make_shared<jsv_string>(str);
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_string;
        return s;
    }

    jsv_boolean::ref cjsruntime::_new_boolean(bool b, uint32_t attr) {
        auto s = std::make_shared<jsv_boolean>(b);
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_boolean;
        return s;
    }

    jsv_object::ref cjsruntime::_new_object(uint32_t attr) {
        auto s = std::make_shared<jsv_object>();
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_object;
        return s;
    }

    jsv_function::ref cjsruntime::_new_function(jsv_object::ref proto, uint32_t attr) {
        auto s = std::make_shared<jsv_function>();
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_function;
        if (proto) {
            proto->obj["constructor"] = s;
            s->obj["prototype"] = proto;
        } else {
            auto prototype = _new_object(js_value::at_refs);
            prototype->obj["constructor"] = s;
            s->obj["prototype"] = prototype;
        }
        return s;
    }

    jsv_null::ref cjsruntime::_new_null(uint32_t attr) {
        auto s = std::make_shared<jsv_null>();
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_root;
        return s;
    }

    jsv_undefined::ref cjsruntime::_new_undefined(uint32_t attr) {
        auto s = std::make_shared<jsv_undefined>();
        if (attr & js_value::at_refs) {
            attr &= (uint32_t) ~js_value::at_refs;
            permanents.refs.push_back(s);
        }
        if (attr > 0U) s->attr = attr;
        s->__proto__ = permanents._proto_root;
        return s;
    }

    void cjsruntime::print(const js_value::ref &value, int level, std::ostream &os) {
        if (value == nullptr) {
            os << "undefined" << std::endl;
            return;
        }
        if (level > 10) {
            os << "too many lines" << std::endl;
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
                if (!n->special.empty()) {
                    os << "object: [[primitive]] " << n->to_string() << std::endl;
                } else {
                    os << "object: " << std::endl;
                    for (const auto &s : n->obj) {
                        os << std::setfill(' ') << std::setw(level) << "";
                        os << s.first << ": " << std::endl;
                        print(s.second.lock(), level + 1, os);
                    }
                }
            }
                break;
            case r_function: {
                auto n = std::dynamic_pointer_cast<jsv_function>(value);
                if (n->builtin)
                    os << "function: builtin " << n->name << std::endl;
                else if (n->code) {
                    os << "function: " << n->code->fullname << " ";
                    os << n->code->text << std::endl;;
                    if (n->closure.lock()) {
                        print(n->closure.lock(), level + 1, os);
                    }
                } else {
                    os << "function: (cleaned) " << n->name << std::endl;
                }
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