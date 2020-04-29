//
// Project: clibjs
// Created by bajdcc
//

#include <utility>
#include <cmath>
#include <cassert>
#include <sstream>

#include "cjsgen.h"
#include "cjsast.h"

namespace clib {

    symbol_t sym_t::get_type() const {
        return s_sym;
    }

    symbol_t sym_t::get_base_type() const {
        return s_sym;
    }

    std::string sym_t::to_string() const {
        return "";
    }

    int sym_t::gen_lvalue(ijsgen &gen) {
        return no_lvalue;
    }

    int sym_t::gen_rvalue(ijsgen &gen) {
        return 0;
    }

    int sym_t::gen_invoke(ijsgen &gen, sym_t::ref &list) {
        return 0;
    }

    int sym_t::set_parent(sym_t::ref node) {
        parent = node;
        return 0;
    }

    // ----

    symbol_t sym_exp_t::get_type() const {
        return s_expression;
    }

    symbol_t sym_exp_t::get_base_type() const {
        return s_expression;
    }

    // ----

    symbol_t sym_id_t::get_type() const {
        return s_id;
    }

    symbol_t sym_id_t::get_base_type() const {
        return s_id;
    }

    std::string sym_id_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_id_t::gen_lvalue(ijsgen &gen) {
        for (const auto &s : ids) {
            s->gen_lvalue(gen);
        }
        return sym_t::gen_lvalue(gen);
    }

    int sym_id_t::gen_rvalue(ijsgen &gen) {
        if (init) {
            init->gen_rvalue(gen);
        } else
            gen.emit(this, LOAD_UNDEFINED);
        for (const auto &s : ids) {
            s->gen_lvalue(gen);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_id_t::gen_rvalue_decl(ijsgen &gen) {
        gen.emit(this, LOAD_UNDEFINED);
        for (const auto &s : ids) {
            s->gen_lvalue(gen);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_id_t::set_parent(sym_t::ref node) {
        for (auto &s : ids) {
            s->set_parent(shared_from_this());
        }
        if (init)
            init->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    void sym_id_t::parse() {
        if (!init)return;
        auto i = init;
        while (i) {
            if (i->get_type() == s_binop) {
                auto b = std::dynamic_pointer_cast<sym_binop_t>(i);
                if (b->op->data._op == T_ASSIGN) {
                    auto d = b->exp1;
                    if (d->get_type() == s_var_id) {
                        auto old_id = std::dynamic_pointer_cast<sym_var_t>(b->exp1);
                        auto new_id = std::make_shared<sym_var_t>(old_id->node);
                        copy_info(new_id, old_id);
                        ids.push_back(new_id);
                        i = b->exp2;
                        init = i;
                        continue;
                    }
                }
            }
            break;
        }
    }

    // ----

    sym_var_t::sym_var_t(ast_node *node) : node(node) {

    }

    symbol_t sym_var_t::get_type() const {
        return s_var;
    }

    std::string sym_var_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_var_t::gen_lvalue(ijsgen &gen) {
        switch (node->flag) {
            case a_literal:
                if (clazz == local) {
                    gen.emit(this, STORE_NAME, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_name));
                    if (parent.lock()->get_type() == s_id) {
                        if (gen.get_var(node->data._string, sq_local) != nullptr)
                            gen.error(this, "id conflict");
                        gen.add_var(node->data._string, shared_from_this());
                    }
                } else if (clazz == fast) {
                    gen.emit(this, STORE_FAST, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_name));
                    if (parent.lock()->get_type() == s_id) {
                        if (gen.get_var(node->data._string, sq_local) != nullptr)
                            gen.error(this, "id conflict");
                        gen.add_var(node->data._string, shared_from_this());
                    }
                } else if (clazz == global) {
                    gen.emit(this, STORE_GLOBAL, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_global));
                } else if (clazz == closure) {
                    gen.emit(this, STORE_DEREF, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_deref));
                } else {
                    gen.error(this, "unsupported class type");
                }
                break;
            default:
                gen.error(this, "unsupported var type");
                break;
        }
        return can_be_lvalue;
    }

    int sym_var_t::gen_rvalue(ijsgen &gen) {
        switch (node->flag) {
            case a_literal:
                if (clazz == local) {
                    gen.emit(this, LOAD_NAME, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_name));
                } else if (clazz == fast) {
                    gen.emit(this, LOAD_FAST, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_name));
                } else if (clazz == global) {
                    gen.emit(this, LOAD_GLOBAL, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_global));
                } else if (clazz == closure) {
                    gen.emit(this, LOAD_DEREF, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_deref));
                } else {
                    gen.error(this, "unsupported class type");
                }
                break;
            case a_string:
                gen.emit(this, LOAD_CONST, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_string));
                break;
            case a_number:
                if (node->data._number != 0) {
                    gen.emit(this, LOAD_CONST, gen.load_number(node->data._number));
                } else {
                    if (std::signbit(node->data._number) == 0)
                        gen.emit(this, LOAD_ZERO, 0);
                    else
                        gen.emit(this, LOAD_ZERO, 1);
                }
                break;
            case a_regex:
                gen.emit(this, LOAD_CONST, gen.load_string(node->data._string, cjs_consts::get_string_t::gs_regex));
                break;
            case a_keyword:
                if (node->data._keyword == K_TRUE)
                    gen.emit(this, LOAD_TRUE);
                else if (node->data._keyword == K_FALSE)
                    gen.emit(this, LOAD_FALSE);
                else if (node->data._keyword == K_NULL)
                    gen.emit(this, LOAD_NULL);
                else if (node->data._keyword == K_UNDEFINED)
                    gen.emit(this, LOAD_UNDEFINED);
                else if (node->data._keyword == K_THIS)
                    gen.emit(this, LOAD_THIS);
                else
                    gen.error(this, "unsupported var type");
                break;
            default:
                gen.error(this, "unsupported var type");
                break;
        }
        return sym_t::gen_rvalue(gen);
    }

    // ----

    sym_var_id_t::sym_var_id_t(ast_node *node, const sym_t::ref &symbol) : sym_var_t(node), id(symbol) {

    }

    symbol_t sym_var_id_t::get_type() const {
        return s_var_id;
    }

    std::string sym_var_id_t::to_string() const {
        return sym_var_t::to_string();
    }

    int sym_var_id_t::gen_lvalue(ijsgen &gen) {
        init_id(gen);
        return sym_var_t::gen_lvalue(gen);
    }

    int sym_var_id_t::gen_rvalue(ijsgen &gen) {
        init_id(gen);
        return sym_var_t::gen_rvalue(gen);
    }

    int sym_var_id_t::gen_invoke(ijsgen &gen, sym_t::ref &list) {
        return sym_t::gen_invoke(gen, list);
    }

    void sym_var_id_t::init_id(ijsgen &gen) {
        auto str = std::string(node->data._identifier);
        if (str == "arguments") {
            clazz = gen.get_func_level() == 1 ? global : fast;
            return;
        }
        auto i = gen.get_var(str, sq_local_func);
        if (i) {
            id = i;
            clazz = gen.get_func_level() == 1 ? local : fast;
        } else {
            i = gen.get_var(str, sq_all);
            if (i) {
                id = i;
                clazz = closure;
                gen.add_closure(std::dynamic_pointer_cast<sym_var_id_t>(shared_from_this()));
            } else {
                clazz = global;
            }
        }
    }

    // ----

    sym_unop_t::sym_unop_t(sym_exp_t::ref exp, ast_node *op) : exp(std::move(exp)), op(op) {

    }

    symbol_t sym_unop_t::get_type() const {
        return s_unop;
    }

    std::string sym_unop_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_unop_t::gen_lvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(this, BINARY_INC);
                gen.emit(this, DUP_TOP);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported unop");
                gen.emit(this, POP_TOP);
                break;
            case T_DEC:
                gen.emit(this, BINARY_DEC);
                gen.emit(this, DUP_TOP);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported unop");
                gen.emit(this, POP_TOP);
                break;
            default:
                gen.error(this, "unsupported unop");
                break;
        }
        return sym_t::gen_lvalue(gen);
    }

    int sym_unop_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(this, BINARY_INC);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported unop");
                break;
            case T_DEC:
                gen.emit(this, BINARY_DEC);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported unop");
                break;
            case T_LOG_NOT:
                gen.emit(this, UNARY_NOT);
                break;
            case T_BIT_NOT:
                gen.emit(this, UNARY_INVERT);
                break;
            case T_ADD:
                gen.emit(this, UNARY_POSITIVE);
                break;
            case T_SUB:
                gen.emit(this, UNARY_NEGATIVE);
                break;
            case K_DELETE:
                gen.emit(this, UNARY_DELETE);
                break;
            case K_TYPEOF:
                gen.emit(this, UNARY_TYPEOF);
                break;
            default:
                gen.error(this, "unsupported unop");
                break;
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_unop_t::set_parent(sym_t::ref node) {
        exp->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    sym_sinop_t::sym_sinop_t(sym_exp_t::ref exp, ast_node *op) : exp(std::move(exp)), op(op) {

    }

    symbol_t sym_sinop_t::get_type() const {
        return s_sinop;
    }

    std::string sym_sinop_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_sinop_t::gen_lvalue(ijsgen &gen) {
        return exp->gen_lvalue(gen);
    }

    int sym_sinop_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(this, DUP_TOP);
                gen.emit(this, BINARY_INC);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported sinop");
                gen.emit(this, POP_TOP);
                break;
            case T_DEC:
                gen.emit(this, DUP_TOP);
                gen.emit(this, BINARY_DEC);
                if (exp->gen_lvalue(gen) == no_lvalue)
                    gen.error(this, "unsupported sinop");
                gen.emit(this, POP_TOP);
                break;
            default:
                gen.error(this, "unsupported sinop");
                break;
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_sinop_t::set_parent(sym_t::ref node) {
        exp->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    sym_binop_t::sym_binop_t(sym_exp_t::ref exp1, sym_exp_t::ref exp2, ast_node *op)
            : exp1(std::move(exp1)), exp2(std::move(exp2)), op(op) {

    }

    symbol_t sym_binop_t::get_type() const {
        return s_binop;
    }

    std::string sym_binop_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_binop_t::gen_lvalue(ijsgen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_binop_t::gen_rvalue(ijsgen &gen) {
        if (op->flag == a_keyword) {
            exp1->gen_rvalue(gen);
            exp2->gen_rvalue(gen);
            if (op->data._keyword == K_INSTANCEOF) {
                gen.emit(this, INSTANCE_OF);
            } else {
                gen.error(this, "unsupported binop");
            }
            return 0;
        }
        assert(op->flag == a_operator);
        switch (op->data._op) {
            case T_ASSIGN: {
                exp2->gen_rvalue(gen);
                if (exp1->gen_lvalue(gen) == 0) {
                    gen.error(this, "invalid assignment");
                }
            }
                return 0;
            case T_ASSIGN_ADD:
            case T_ASSIGN_SUB:
            case T_ASSIGN_MUL:
            case T_ASSIGN_DIV:
            case T_ASSIGN_MOD:
            case T_ASSIGN_LSHIFT:
            case T_ASSIGN_RSHIFT:
            case T_ASSIGN_URSHIFT:
            case T_ASSIGN_AND:
            case T_ASSIGN_OR:
            case T_ASSIGN_XOR:
            case T_ASSIGN_POWER: {
                exp1->gen_rvalue(gen);
                exp2->gen_rvalue(gen);
                switch (op->data._op) {
                    case T_ASSIGN_ADD:
                        gen.emit(this, BINARY_ADD);
                        break;
                    case T_ASSIGN_SUB:
                        gen.emit(this, BINARY_SUBTRACT);
                        break;
                    case T_ASSIGN_MUL:
                        gen.emit(this, BINARY_MULTIPLY);
                        break;
                    case T_ASSIGN_DIV:
                        gen.emit(this, BINARY_TRUE_DIVIDE);
                        break;
                    case T_ASSIGN_MOD:
                        gen.emit(this, BINARY_MODULO);
                        break;
                    case T_ASSIGN_LSHIFT:
                        gen.emit(this, BINARY_LSHIFT);
                        break;
                    case T_ASSIGN_RSHIFT:
                        gen.emit(this, BINARY_RSHIFT);
                        break;
                    case T_ASSIGN_URSHIFT:
                        gen.emit(this, BINARY_URSHIFT);
                        break;
                    case T_ASSIGN_AND:
                        gen.emit(this, BINARY_AND);
                        break;
                    case T_ASSIGN_OR:
                        gen.emit(this, BINARY_OR);
                        break;
                    case T_ASSIGN_XOR:
                        gen.emit(this, BINARY_XOR);
                        break;
                    case T_ASSIGN_POWER:
                        gen.emit(this, BINARY_POWER);
                        break;
                    default:
                        break;
                }
                if (exp1->gen_lvalue(gen) == 0) {
                    gen.error(this, "invalid assignment");
                }
            }
                return 0;
            default:
                break;
        }
        exp1->gen_rvalue(gen);
        switch (op->data._op) {
            case T_LOG_AND: {
                auto idx = gen.code_length();
                gen.emit(this, JUMP_IF_FALSE_OR_POP, 0);
                exp2->gen_rvalue(gen);
                gen.edit(idx, 1, gen.code_length());
            }
                return 0;
            case T_LOG_OR: {
                auto idx = gen.code_length();
                gen.emit(this, JUMP_IF_TRUE_OR_POP, 0);
                exp2->gen_rvalue(gen);
                gen.edit(idx, 1, gen.code_length());
            }
                return 0;
            default:
                break;
        }
        exp2->gen_rvalue(gen);
        switch (op->data._op) {
            case T_ADD:
                gen.emit(this, BINARY_ADD);
                break;
            case T_SUB:
                gen.emit(this, BINARY_SUBTRACT);
                break;
            case T_MUL:
                gen.emit(this, BINARY_MULTIPLY);
                break;
            case T_DIV:
                gen.emit(this, BINARY_TRUE_DIVIDE);
                break;
            case T_MOD:
                gen.emit(this, BINARY_MODULO);
                break;
            case T_POWER:
                gen.emit(this, BINARY_POWER);
                break;
            case T_LESS:
                gen.emit(this, COMPARE_LESS);
                break;
            case T_LESS_EQUAL:
                gen.emit(this, COMPARE_LESS_EQUAL);
                break;
            case T_EQUAL:
                gen.emit(this, COMPARE_EQUAL);
                break;
            case T_NOT_EQUAL:
                gen.emit(this, COMPARE_NOT_EQUAL);
                break;
            case T_GREATER:
                gen.emit(this, COMPARE_GREATER);
                break;
            case T_GREATER_EQUAL:
                gen.emit(this, COMPARE_GREATER_EQUAL);
                break;
            case T_FEQUAL:
                gen.emit(this, COMPARE_FEQUAL);
                break;
            case T_FNOT_EQUAL:
                gen.emit(this, COMPARE_FNOT_EQUAL);
                break;
            case T_BIT_AND:
                gen.emit(this, BINARY_AND);
                break;
            case T_BIT_OR:
                gen.emit(this, BINARY_OR);
                break;
            case T_BIT_XOR:
                gen.emit(this, BINARY_XOR);
                break;
            case T_LSHIFT:
                gen.emit(this, BINARY_LSHIFT);
                break;
            case T_RSHIFT:
                gen.emit(this, BINARY_RSHIFT);
                break;
            case T_URSHIFT:
                gen.emit(this, BINARY_URSHIFT);
                break;
            default:
                gen.error(this, "unsupported binop");
                break;
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_binop_t::set_parent(sym_t::ref node) {
        exp1->set_parent(shared_from_this());
        exp2->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    sym_triop_t::sym_triop_t(sym_exp_t::ref exp1, sym_exp_t::ref exp2,
                             sym_exp_t::ref exp3, ast_node *op1, ast_node *op2)
            : exp1(std::move(exp1)), exp2(std::move(exp2)), exp3(std::move(exp3)), op1(op1), op2(op2) {

    }

    symbol_t sym_triop_t::get_type() const {
        return s_triop;
    }

    std::string sym_triop_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_triop_t::gen_lvalue(ijsgen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_triop_t::gen_rvalue(ijsgen &gen) {
        if (op1->data._op == T_QUERY && op2->data._op == T_COLON) {
            exp1->gen_rvalue(gen);
            auto idx = gen.code_length();
            gen.emit(op1, POP_JUMP_IF_FALSE, 0);
            exp2->gen_rvalue(gen);
            auto idx2 = gen.code_length();
            auto cur = gen.code_length();
            gen.emit(op2, JUMP_FORWARD, 0);
            gen.edit(idx, 1, gen.code_length());
            exp3->gen_rvalue(gen);
            gen.edit(idx2, 1, gen.code_length() - cur);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_triop_t::set_parent(sym_t::ref node) {
        exp1->set_parent(shared_from_this());
        exp2->set_parent(shared_from_this());
        exp3->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    sym_member_dot_t::sym_member_dot_t(sym_exp_t::ref exp) : exp(std::move(exp)) {

    }

    symbol_t sym_member_dot_t::get_type() const {
        return s_member_dot;
    }

    std::string sym_member_dot_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_member_dot_t::gen_lvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        size_t i = 0;
        for (const auto &s : dots) {
            if (i + 1 < dots.size())
                gen.emit(s, LOAD_ATTR, gen.load_string(s->data._string, cjs_consts::get_string_t::gs_name));
            else
                gen.emit(s, STORE_ATTR, gen.load_string(s->data._string, cjs_consts::get_string_t::gs_name));
            i++;
        }
        return can_be_lvalue;
    }

    int sym_member_dot_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        for (const auto &s : dots) {
            gen.emit(s, LOAD_ATTR, gen.load_string(s->data._string, cjs_consts::get_string_t::gs_name));
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_member_dot_t::set_parent(sym_t::ref node) {
        exp->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    sym_member_index_t::sym_member_index_t(sym_exp_t::ref exp) : exp(std::move(exp)) {

    }

    symbol_t sym_member_index_t::get_type() const {
        return s_member_index;
    }

    std::string sym_member_index_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_member_index_t::gen_lvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        size_t i = 0;
        for (const auto &s : indexes) {
            if (i + 1 < indexes.size()) {
                s->gen_rvalue(gen);
                gen.emit(s.get(), BINARY_SUBSCR);
            } else {
                s->gen_rvalue(gen);
                gen.emit(s.get(), STORE_SUBSCR);
            }
            i++;
        }
        return can_be_lvalue;
    }

    int sym_member_index_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        for (const auto &s : indexes) {
            s->gen_rvalue(gen);
            gen.emit(s.get(), BINARY_SUBSCR);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_member_index_t::set_parent(sym_t::ref node) {
        exp->set_parent(shared_from_this());
        for (const auto &s : indexes) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_exp_seq_t::get_type() const {
        return s_expression_seq;
    }

    std::string sym_exp_seq_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_exp_seq_t::gen_rvalue(ijsgen &gen) {
        size_t i = 0;
        for (const auto &s : exps) {
            s->gen_rvalue(gen);
            if (i + 1 < exps.size())
                gen.emit(s.get(), POP_TOP);
            i++;
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_exp_seq_t::set_parent(sym_t::ref node) {
        for (const auto &s : exps) {
            s->set_parent(node);
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_array_t::get_type() const {
        return s_array;
    }

    std::string sym_array_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_array_t::gen_rvalue(ijsgen &gen) {
        for (const auto &s : exps) {
            if (s)
                s->gen_rvalue(gen);
            else
                gen.emit(nullptr, LOAD_EMPTY);
        }
        gen.emit(this, BUILD_LIST, exps.size());
        return sym_t::gen_rvalue(gen);
    }

    int sym_array_t::set_parent(sym_t::ref node) {
        for (const auto &s : exps) {
            if (s)
                s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_object_pair_t::get_type() const {
        return s_object_pair;
    }

    symbol_t sym_object_pair_t::get_base_type() const {
        return s_object_pair;
    }

    std::string sym_object_pair_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_object_pair_t::gen_rvalue(ijsgen &gen) {
        key->gen_rvalue(gen);
        value->gen_rvalue(gen);
        return sym_t::gen_rvalue(gen);
    }

    int sym_object_pair_t::set_parent(sym_t::ref node) {
        key->set_parent(shared_from_this());
        value->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_new_t::get_type() const {
        return s_new;
    }

    std::string sym_new_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_new_t::gen_rvalue(ijsgen &gen) {
        obj->gen_rvalue(gen);
        if (rests.empty()) {
            for (const auto &s : args) {
                s->gen_rvalue(gen);
            }
            gen.emit(this, CALL_FUNCTION_EX, args.size());
        } else {
            size_t i = 0, j = 0;
            gen.emit(this, CALL_FUNCTION_KW);
            for (const auto &s : args) {
                s->gen_rvalue(gen);
                if (i < rests.size() && rests[i] == j) {
                    gen.emit(s.get(), UNPACK_SEQUENCE);
                    i++;
                }
                j++;
            }
            gen.emit(this, CALL_FUNCTION_EX, -1);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_new_t::set_parent(sym_t::ref node) {
        obj->set_parent(shared_from_this());
        for (const auto &s : args) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_object_t::get_type() const {
        return s_object;
    }

    std::string sym_object_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_object_t::gen_rvalue(ijsgen &gen) {
        for (const auto &s : pairs) {
            s->gen_rvalue(gen);
        }
        gen.emit(this, BUILD_MAP, pairs.size());
        return sym_t::gen_rvalue(gen);
    }

    int sym_object_t::set_parent(sym_t::ref node) {
        for (const auto &s : pairs) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_call_method_t::get_type() const {
        return s_call_method;
    }

    std::string sym_call_method_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_call_method_t::gen_rvalue(ijsgen &gen) {
        obj->gen_rvalue(gen);
        gen.emit(method, LOAD_METHOD, gen.load_string(method->data._string, cjs_consts::get_string_t::gs_name));
        if (rests.empty()) {
            for (const auto &s : args) {
                s->gen_rvalue(gen);
            }
            gen.emit(this, CALL_METHOD, args.size());
        } else {
            size_t i = 0, j = 0;
            gen.emit(this, CALL_FUNCTION_KW);
            for (const auto &s : args) {
                s->gen_rvalue(gen);
                if (i < rests.size() && rests[i] == j) {
                    gen.emit(s.get(), UNPACK_SEQUENCE);
                    i++;
                }
                j++;
            }
            gen.emit(this, CALL_METHOD, -1);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_call_method_t::set_parent(sym_t::ref node) {
        obj->set_parent(shared_from_this());
        for (const auto &s : args) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_call_function_t::get_type() const {
        return s_call_function;
    }

    std::string sym_call_function_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_call_function_t::gen_rvalue(ijsgen &gen) {
        obj->gen_rvalue(gen);
        if (rests.empty()) {
            for (const auto &s : args) {
                s->gen_rvalue(gen);
            }
            gen.emit(this, CALL_FUNCTION, args.size());
        } else {
            size_t i = 0, j = 0;
            gen.emit(this, CALL_FUNCTION_KW);
            for (const auto &s : args) {
                s->gen_rvalue(gen);
                if (i < rests.size() && rests[i] == j) {
                    gen.emit(s.get(), UNPACK_SEQUENCE);
                    i++;
                }
                j++;
            }
            gen.emit(this, CALL_FUNCTION, -1);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_call_function_t::set_parent(sym_t::ref node) {
        obj->set_parent(shared_from_this());
        for (const auto &s : args) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_t::get_type() const {
        return s_statement;
    }

    symbol_t sym_stmt_t::get_base_type() const {
        return s_statement;
    }

    std::string sym_stmt_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_stmt_t::gen_rvalue(ijsgen &gen) {
        return sym_t::gen_rvalue(gen);
    }

    // ----

    symbol_t sym_stmt_var_t::get_type() const {
        return s_statement_var;
    }

    std::string sym_stmt_var_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_var_t::gen_rvalue(ijsgen &gen) {
        for (const auto &s : vars) {
            s->gen_rvalue(gen);
            gen.emit(s.get(), POP_TOP);
        }
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_var_t::set_parent(sym_t::ref node) {
        for (const auto &s : vars) {
            s->set_parent(shared_from_this());
        }
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_exp_t::get_type() const {
        return s_statement_exp;
    }

    std::string sym_stmt_exp_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_exp_t::gen_rvalue(ijsgen &gen) {
        seq->gen_rvalue(gen);
        gen.emit(this, POP_TOP);
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_exp_t::set_parent(sym_t::ref node) {
        seq->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_return_t::get_type() const {
        return s_statement_return;
    }

    std::string sym_stmt_return_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_return_t::gen_rvalue(ijsgen &gen) {
        if (seq)
            seq->gen_rvalue(gen);
        else
            gen.emit(nullptr, LOAD_UNDEFINED);
        gen.emit(this, RETURN_VALUE);
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_return_t::set_parent(sym_t::ref node) {
        if (seq)
            seq->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_control_t::get_type() const {
        return s_statement_control;
    }

    std::string sym_stmt_control_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_control_t::gen_rvalue(ijsgen &gen) {
        gen.push_rewrites(gen.code_length(), keyword);
        gen.emit(this, JUMP_ABSOLUTE, 0);
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_control_t::set_parent(sym_t::ref node) {
        if (label)
            label->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_if_t::get_type() const {
        return s_statement_if;
    }

    std::string sym_stmt_if_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_if_t::gen_rvalue(ijsgen &gen) {
        seq->gen_rvalue(gen);
        auto idx = gen.code_length();
        gen.emit(this, POP_JUMP_IF_FALSE, 0);
        true_stmt->gen_rvalue(gen);
        if (false_stmt) {
            auto idx2 = gen.code_length();
            gen.emit(nullptr, JUMP_FORWARD, 0);
            gen.edit(idx, 1, gen.code_length());
            false_stmt->gen_rvalue(gen);
            gen.edit(idx2, 1, gen.code_length() - idx2);
        } else {
            gen.edit(idx, 1, gen.code_length());
        }
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_if_t::set_parent(sym_t::ref node) {
        seq->set_parent(shared_from_this());
        true_stmt->set_parent(shared_from_this());
        if (false_stmt)
            false_stmt->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_while_t::get_type() const {
        return s_statement_while;
    }

    std::string sym_stmt_while_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_while_t::gen_rvalue(ijsgen &gen) {
        auto idx = gen.code_length();
        seq->gen_rvalue(gen);
        auto idx2 = gen.code_length();
        gen.emit(this, POP_JUMP_IF_FALSE, 0);
        stmt->gen_rvalue(gen);
        gen.emit(nullptr, JUMP_ABSOLUTE, idx);
        gen.edit(idx2, 1, gen.code_length());
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_while_t::set_parent(sym_t::ref node) {
        seq->set_parent(shared_from_this());
        stmt->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_for_t::get_type() const {
        return s_statement_for;
    }

    std::string sym_stmt_for_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_for_t::gen_rvalue(ijsgen &gen) {
        gen.enter(sp_for);
        if (exp) {
            exp->gen_rvalue(gen);
            gen.emit(exp.get(), POP_TOP);
        } else if (vars)
            vars->gen_rvalue(gen);
        auto L1 = 0;
        auto L2 = gen.code_length(); // cond
        if (cond) {
            cond->gen_rvalue(gen);
            L1 = gen.code_length();
            gen.emit(cond.get(), POP_JUMP_IF_FALSE, 0); // exit
        }
        body->gen_rvalue(gen);
        if (iter) {
            iter->gen_rvalue(gen);
            gen.emit(iter.get(), POP_TOP);
        }
        gen.emit(nullptr, JUMP_ABSOLUTE, L2);
        if (cond)
            gen.edit(L1, 1, gen.code_length());
        auto j_break = gen.code_length();
        auto j_continue = L2;
        const auto &re = gen.get_rewrites();
        for (const auto &s : re) {
            if (s.second == K_BREAK)
                gen.edit(s.first, 1, j_break);
            else if (s.second == K_CONTINUE)
                gen.edit(s.first, 1, j_continue);
            else
                assert(!"invalid rewrites");
        }
        gen.leave();
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_for_t::set_parent(sym_t::ref node) {
        if (exp)
            exp->set_parent(shared_from_this());
        else if (vars)
            vars->set_parent(shared_from_this());
        if (cond)
            cond->set_parent(shared_from_this());
        if (iter)
            iter->set_parent(shared_from_this());
        body->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_stmt_for_in_t::get_type() const {
        return s_statement_for_in;
    }

    std::string sym_stmt_for_in_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_for_in_t::gen_rvalue(ijsgen &gen) {
        gen.enter(sp_for_each);
        iter->gen_rvalue(gen);
        gen.emit(iter.get(), GET_ITER);
        auto idx_exit = gen.code_length();
        gen.emit(iter.get(), FOR_ITER, 0);
        if (exp)
            exp->gen_lvalue(gen);
        else
            vars->gen_lvalue(gen);
        gen.emit(nullptr, POP_TOP);
        body->gen_rvalue(gen);
        gen.emit(nullptr, JUMP_ABSOLUTE, idx_exit);
        gen.edit(idx_exit, 1, gen.code_length() - idx_exit);
        auto j_break = gen.code_length();
        auto j_continue = idx_exit;
        const auto &re = gen.get_rewrites();
        for (const auto &s : re) {
            if (s.second == K_BREAK)
                gen.edit(s.first, 1, j_break);
            else if (s.second == K_CONTINUE)
                gen.edit(s.first, 1, j_continue);
            else
                assert(!"invalid rewrites");
        }
        gen.leave();
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_for_in_t::set_parent(sym_t::ref node) {
        if (exp)
            exp->set_parent(shared_from_this());
        else
            vars->set_parent(shared_from_this());
        iter->set_parent(shared_from_this());
        body->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_block_t::get_type() const {
        return s_block;
    }

    std::string sym_block_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_block_t::gen_rvalue(ijsgen &gen) {
        gen.enter(sp_block);
        decltype(stmts) func_stmts;
        decltype(stmts) var_decls;
        decltype(stmts) others;
        for (const auto &s : stmts) {
            if (s->get_type() == s_statement_exp) {
                auto exp = std::dynamic_pointer_cast<sym_stmt_exp_t>(s);
                const auto c = exp->seq->exps;
                if (c.size() == 1) {
                    if (c.front()->get_type() == s_code) {
                        func_stmts.push_back(s);
                        continue;
                    } else {
                        if (c.front()->get_type() == s_id) {
                            var_decls.push_back(s);
                            continue;
                        }
                    }
                }
            }
            others.push_back(s);
        }
        for (const auto &s : func_stmts) {
            s->gen_rvalue(gen);
        }
        for (const auto &s : var_decls) {
            std::dynamic_pointer_cast<sym_id_t>(s)->gen_rvalue_decl(gen);
        }
        for (const auto &s : others) {
            s->gen_rvalue(gen);
        }
        gen.leave();
        return sym_t::gen_rvalue(gen);
    }

    int sym_block_t::set_parent(sym_t::ref node) {
        for (const auto &s : stmts) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }

    // ----

    symbol_t sym_code_t::get_type() const {
        return s_code;
    }

    std::string sym_code_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_code_t::gen_rvalue(ijsgen &gen) {
        fullname = gen.get_fullname(name ? name->data._identifier : LAMBDA_ID);
        auto p = parent.lock();
        if (p) {
            if (p->get_type() == s_id) {
                auto _id = std::dynamic_pointer_cast<sym_id_t>(p);
                if (!_id->ids.empty()) {
                    std::stringstream ss2;
                    auto idx = _id->ids.back().get();
                    ss2 << "(" << gen.get_filename() << ":" << idx->line << ":" << idx->column
                        << ") " << gen.get_code_text(idx);
                    debugname = ss2.str();
                }
            } else if (p->get_type() == s_binop) {
                do {
                    auto _binop = std::dynamic_pointer_cast<sym_binop_t>(p);
                    if (_binop->op->flag == a_operator && _binop->op->data._op == T_ASSIGN) {
                        if (_binop->exp1->get_type() == s_member_dot) {
                            auto _dot = std::dynamic_pointer_cast<sym_member_dot_t>(_binop->exp1);
                            if (gen.get_code_text(_dot->exp.get()) == "this" && _dot->dots.size() == 1) {
                                std::stringstream ss2;
                                auto idx = _dot->exp.get();
                                ss2 << "(" << gen.get_filename() << ":" << idx->line << ":" << idx->column
                                    << ") " << gen.get_func_name() << ".prototype." << gen.get_code_text(_dot->dots.front());
                                debugname = ss2.str();
                                break;
                            }
                        }
                        std::stringstream ss3;
                        auto idx = _binop->exp1.get();
                        ss3 << "(" << gen.get_filename() << ":" << idx->line << ":" << idx->column
                            << ") " << fullname << " " << gen.get_code_text(idx);
                        debugname = ss3.str();
                    }
                } while (false);
            }
        }
        if (debugname.empty()) {
            std::stringstream ss;
            ss << "(" << gen.get_filename() << ":" << this->line << ":" << this->column
               << ") " << fullname;
            debugname = ss.str();
        }
        gen.add_var(fullname, shared_from_this());
        if (!args.empty())
            gen.enter(sp_param);
        auto id = gen.push_function(std::dynamic_pointer_cast<sym_code_t>(shared_from_this()));
        if (body) {
            if (!arrow && name) {
                gen.enter(sp_block);
                gen.add_var(fullname, shared_from_this());
            }
            body->gen_rvalue(gen);
            if (body->get_base_type() == s_expression)
                gen.emit(nullptr, RETURN_VALUE);
            auto l = gen.code_length();
            if (l > 0 && gen.get_ins(l - 1) != RETURN_VALUE) {
                gen.emit(nullptr, RETURN_VALUE);
            }
            if (!arrow && name) {
                gen.leave();
            }
        }
        gen.pop_function();
        uint32_t flag = 0;
        if (!closure.empty()) {
            for (const auto &s : closure) {
                gen.emit(s->node, LOAD_CLOSURE, gen.load_string(
                        s->node->data._identifier, cjs_consts::get_string_t::gs_name));
            }
            flag |= 8U;
            gen.emit(nullptr, BUILD_MAP, closure.size());
        }
        if (name) {
            gen.emit(name, LOAD_CONST, id);
            gen.emit(name, LOAD_CONST, gen.load_string(debugname, cjs_consts::get_string_t::gs_string));
            gen.emit(this, MAKE_FUNCTION, (int) flag);
            if (parent.lock()->get_type() == s_statement_exp)
                gen.emit(name, STORE_NAME, gen.load_string(name->data._identifier, cjs_consts::get_string_t::gs_name));
        } else {
            gen.emit(nullptr, LOAD_CONST, id);
            gen.emit(nullptr, LOAD_CONST, gen.load_string(debugname, cjs_consts::get_string_t::gs_string));
            gen.emit(this, MAKE_FUNCTION, (int) flag);
        }
        if (!args.empty())
            gen.leave();
        consts.save();
        return sym_t::gen_rvalue(gen);
    }

    int sym_code_t::set_parent(sym_t::ref node) {
        if (body)
            body->set_parent(shared_from_this());
        return sym_t::set_parent(node);
    }
}