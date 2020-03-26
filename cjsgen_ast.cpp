//
// Project: clibjs
// Created by bajdcc
//

#include <utility>

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
        return sym_t::gen_lvalue(gen);
    }

    int sym_id_t::gen_rvalue(ijsgen &gen) {
        if (init)
            init->gen_rvalue(gen);
        else
            gen.emit(line, column, start, end, LOAD_UNDEFINED);
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
                gen.emit(line, column, start, end, DUP_TOP);
                gen.emit(line, column, start, end, STORE_NAME, gen.load_string(node->data._string, true));
                break;
            default:
                gen.error(line, column, start, end, "unsupported var type");
                break;
        }
        return can_be_lvalue;
    }

    int sym_var_t::gen_rvalue(ijsgen &gen) {
        switch (node->flag) {
            case a_literal:
                gen.emit(line, column, start, end, LOAD_NAME, gen.load_string(node->data._string, true));
                break;
            case a_string:
                gen.emit(line, column, start, end, LOAD_CONST, gen.load_string(node->data._string, false));
                break;
            case a_number:
                gen.emit(line, column, start, end, LOAD_CONST, gen.load_number(node->data._number));
                break;
            default:
                gen.error(line, column, start, end, "unsupported var type");
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
        return sym_var_t::gen_lvalue(gen);
    }

    int sym_var_id_t::gen_rvalue(ijsgen &gen) {
        return sym_var_t::gen_rvalue(gen);
    }

    int sym_var_id_t::gen_invoke(ijsgen &gen, sym_t::ref &list) {
        return sym_t::gen_invoke(gen, list);
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
        exp->gen_lvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(line, column, start, end, BINARY_INC);
                gen.emit(line, column, start, end, DUP_TOP);
                exp->gen_lvalue(gen);
                break;
            case T_DEC:
                gen.emit(line, column, start, end, BINARY_DEC);
                gen.emit(line, column, start, end, DUP_TOP);
                exp->gen_lvalue(gen);
                break;
            default:
                gen.error(line, column, start, end, "unsupported unop");
                break;
        }
        return sym_t::gen_lvalue(gen);
    }

    int sym_unop_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(line, column, start, end, BINARY_INC);
                gen.emit(line, column, start, end, DUP_TOP);
                exp->gen_lvalue(gen);
                break;
            case T_DEC:
                gen.emit(line, column, start, end, BINARY_DEC);
                gen.emit(line, column, start, end, DUP_TOP);
                exp->gen_lvalue(gen);
                break;
            default:
                gen.error(line, column, start, end, "unsupported unop");
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
        exp->gen_lvalue(gen);
        return sym_t::gen_lvalue(gen);
    }

    int sym_sinop_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        switch (op->data._op) {
            case T_INC:
                gen.emit(line, column, start, end, DUP_TOP);
                gen.emit(line, column, start, end, BINARY_INC);
                exp->gen_lvalue(gen);
                break;
            case T_DEC:
                gen.emit(line, column, start, end, DUP_TOP);
                gen.emit(line, column, start, end, BINARY_DEC);
                exp->gen_lvalue(gen);
                break;
            default:
                gen.error(line, column, start, end, "unsupported sinop");
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
        switch (op->data._op) {
            case T_ASSIGN: {
                exp2->gen_rvalue(gen);
                if (exp1->gen_lvalue(gen) == 0) {
                    gen.error(line, column, start, end, "invalid assignment");
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
                        gen.emit(line, column, start, end, BINARY_ADD);
                        break;
                    case T_ASSIGN_SUB:
                        gen.emit(line, column, start, end, BINARY_SUBTRACT);
                        break;
                    case T_ASSIGN_MUL:
                        gen.emit(line, column, start, end, BINARY_MULTIPLY);
                        break;
                    case T_ASSIGN_DIV:
                        gen.emit(line, column, start, end, BINARY_TRUE_DIVIDE);
                        break;
                    case T_ASSIGN_MOD:
                        gen.emit(line, column, start, end, BINARY_MODULO);
                        break;
                    case T_ASSIGN_LSHIFT:
                        gen.emit(line, column, start, end, BINARY_LSHIFT);
                        break;
                    case T_ASSIGN_RSHIFT:
                        gen.emit(line, column, start, end, BINARY_RSHIFT);
                        break;
                    case T_ASSIGN_URSHIFT:
                        gen.emit(line, column, start, end, BINARY_URSHIFT);
                        break;
                    case T_ASSIGN_AND:
                        gen.emit(line, column, start, end, BINARY_AND);
                        break;
                    case T_ASSIGN_OR:
                        gen.emit(line, column, start, end, BINARY_OR);
                        break;
                    case T_ASSIGN_XOR:
                        gen.emit(line, column, start, end, BINARY_XOR);
                        break;
                    case T_ASSIGN_POWER:
                        gen.emit(line, column, start, end, BINARY_POWER);
                        break;
                    default:
                        break;
                }
                if (exp1->gen_lvalue(gen) == 0) {
                    gen.error(line, column, start, end, "invalid assignment");
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
                gen.emit(line, column, start, end, JUMP_IF_TRUE_OR_POP, 0);
                exp2->gen_rvalue(gen);
                gen.edit(idx, 1, gen.current());
            }
                return 0;
            case T_LOG_OR: {
                auto idx = gen.code_length();
                gen.emit(line, column, start, end, JUMP_IF_FALSE_OR_POP, 0);
                exp2->gen_rvalue(gen);
                gen.edit(idx, 1, gen.current());
            }
                return 0;
            default:
                break;
        }
        exp2->gen_rvalue(gen);
        switch (op->data._op) {
            case T_ADD:
                gen.emit(line, column, start, end, BINARY_ADD);
                break;
            case T_SUB:
                gen.emit(line, column, start, end, BINARY_SUBTRACT);
                break;
            case T_MUL:
                gen.emit(line, column, start, end, BINARY_MULTIPLY);
                break;
            case T_DIV:
                gen.emit(line, column, start, end, BINARY_TRUE_DIVIDE);
                break;
            case T_MOD:
                gen.emit(line, column, start, end, BINARY_MODULO);
                break;
            case T_POWER:
                gen.emit(line, column, start, end, BINARY_POWER);
                break;
            case T_LESS:
                gen.emit(line, column, start, end, COMPARE_OP, 0);
                break;
            case T_LESS_EQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 1);
                break;
            case T_EQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 2);
                break;
            case T_NOT_EQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 3);
                break;
            case T_GREATER:
                gen.emit(line, column, start, end, COMPARE_OP, 4);
                break;
            case T_GREATER_EQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 5);
                break;
            case T_FEQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 6);
                break;
            case T_FNOT_EQUAL:
                gen.emit(line, column, start, end, COMPARE_OP, 7);
                break;
            case T_BIT_AND:
                gen.emit(line, column, start, end, BINARY_AND);
                break;
            case T_BIT_OR:
                gen.emit(line, column, start, end, BINARY_OR);
                break;
            case T_BIT_XOR:
                gen.emit(line, column, start, end, BINARY_XOR);
                break;
            case T_LSHIFT:
                gen.emit(line, column, start, end, BINARY_LSHIFT);
                break;
            case T_RSHIFT:
                gen.emit(line, column, start, end, BINARY_RSHIFT);
                break;
            case T_URSHIFT:
                gen.emit(line, column, start, end, BINARY_URSHIFT);
                break;
            default:
                gen.error(line, column, start, end, "unsupported binop");
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
        exp1->gen_rvalue(gen);
        auto idx = gen.code_length();
        gen.emit(op1->line, op1->column, op1->start, op1->end, POP_JUMP_IF_FALSE, 0);
        exp2->gen_rvalue(gen);
        auto idx2 = gen.code_length();
        auto cur = gen.current();
        gen.emit(op2->line, op2->column, op2->start, op2->end, JUMP_FORWARD, 0);
        gen.edit(idx, 1, gen.current());
        exp3->gen_rvalue(gen);
        gen.edit(idx2, 1, gen.current() - cur);
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
                gen.emit(s->line, s->column, s->start, s->end, LOAD_ATTR, gen.load_string(s->data._string, true));
            else
                gen.emit(s->line, s->column, s->start, s->end, STORE_ATTR, gen.load_string(s->data._string, true));
            i++;
        }
        return can_be_lvalue;
    }

    int sym_member_dot_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        for (const auto &s : dots) {
            gen.emit(s->line, s->column, s->start, s->end, LOAD_ATTR, gen.load_string(s->data._string, true));
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
                gen.emit(s->line, s->column, s->start, s->end, BINARY_SUBSCR);
            } else {
                s->gen_rvalue(gen);
                gen.emit(s->line, s->column, s->start, s->end, STORE_SUBSCR);
            }
            i++;
        }
        return can_be_lvalue;
    }

    int sym_member_index_t::gen_rvalue(ijsgen &gen) {
        exp->gen_rvalue(gen);
        for (const auto &s : indexes) {
            s->gen_rvalue(gen);
            gen.emit(s->line, s->column, s->start, s->end, BINARY_SUBSCR);
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
                gen.emit(s->line, s->column, s->start, s->end, POP_TOP);
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
            s->gen_rvalue(gen);
        }
        gen.emit(line, column, start, end, BUILD_LIST, exps.size());
        return sym_t::gen_rvalue(gen);
    }

    int sym_array_t::set_parent(sym_t::ref node) {
        for (const auto &s : exps) {
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
        gen.emit(line, column, start, end, BUILD_MAP, pairs.size());
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
        gen.emit(method->line, method->column, method->start, method->end,
                 LOAD_METHOD, gen.load_string(method->data._string, true));
        for (const auto &s : args) {
            s->gen_rvalue(gen);
        }
        gen.emit(line, column, start, end, CALL_METHOD, args.size());
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
        for (const auto &s : args) {
            s->gen_rvalue(gen);
        }
        gen.emit(line, column, start, end, CALL_FUNCTION, args.size());
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
        gen.emit(line, column, start, end, POP_TOP);
        return sym_stmt_t::gen_rvalue(gen);
    }

    int sym_stmt_exp_t::set_parent(sym_t::ref node) {
        seq->set_parent(shared_from_this());
        return sym_stmt_t::set_parent(node);
    }

    // ----

    symbol_t sym_block_t::get_type() const {
        return s_block;
    }

    symbol_t sym_block_t::get_base_type() const {
        return s_block;
    }

    std::string sym_block_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_block_t::gen_rvalue(ijsgen &gen) {
        for (const auto &s : stmts) {
            s->gen_rvalue(gen);
        }
        return sym_t::gen_rvalue(gen);
    }

    int sym_block_t::set_parent(sym_t::ref node) {
        for (const auto &s : stmts) {
            s->set_parent(shared_from_this());
        }
        return sym_t::set_parent(node);
    }
}