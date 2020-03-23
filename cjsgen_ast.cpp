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
        return 0;
    }

    int sym_t::gen_rvalue(ijsgen &gen) {
        return 0;
    }

    int sym_t::gen_invoke(ijsgen &gen, sym_t::ref &list) {
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
        size_t i = 0;
        for (const auto &s : ids) {
            if (i + 1 < ids.size())
                gen.emit(s->line, s->column, s->start, s->end, DUP_TOP);
            s->gen_lvalue(gen);
            i++;
        }
        return sym_t::gen_rvalue(gen);
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
                gen.emit(line, column, start, end, STORE_NAME, gen.load_string(node->data._string, true));
                break;
            default:
                gen.error(line, column, "unsupported var type");
                break;
        }
        return sym_t::gen_lvalue(gen);
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
                gen.error(line, column, "unsupported var type");
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
        return sym_t::gen_lvalue(gen);
    }

    int sym_unop_t::gen_rvalue(ijsgen &gen) {
        return sym_t::gen_rvalue(gen);
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
        return sym_t::gen_lvalue(gen);
    }

    int sym_sinop_t::gen_rvalue(ijsgen &gen) {
        return sym_t::gen_rvalue(gen);
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
                gen.error(line, column, "unsupported binop");
                break;
        }
        return sym_t::gen_rvalue(gen);
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
        return sym_t::gen_rvalue(gen);
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
}