//
// Project: clibjs
// Created by bajdcc
//

#include <utility>

#include "cjsgen.h"

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

    int sym_t::gen_lvalue(igen &gen) {
        return 0;
    }

    int sym_t::gen_rvalue(igen &gen) {
        return 0;
    }

    int sym_t::gen_invoke(igen &gen, sym_t::ref &list) {
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

    sym_id_t::sym_id_t(sym_var_t::ref id) : id(std::move(id)) {

    }

    symbol_t sym_id_t::get_type() const {
        return s_id;
    }

    symbol_t sym_id_t::get_base_type() const {
        return s_id;
    }

    std::string sym_id_t::to_string() const {
        return sym_t::to_string();
    }

    int sym_id_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_id_t::gen_rvalue(igen &gen) {
        return sym_t::gen_rvalue(gen);
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

    int sym_var_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_var_t::gen_rvalue(igen &gen) {
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

    int sym_var_id_t::gen_lvalue(igen &gen) {
        return sym_var_t::gen_lvalue(gen);
    }

    int sym_var_id_t::gen_rvalue(igen &gen) {
        return sym_var_t::gen_rvalue(gen);
    }

    int sym_var_id_t::gen_invoke(igen &gen, sym_t::ref &list) {
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

    int sym_unop_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_unop_t::gen_rvalue(igen &gen) {
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

    int sym_sinop_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_sinop_t::gen_rvalue(igen &gen) {
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

    int sym_binop_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_binop_t::gen_rvalue(igen &gen) {
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

    int sym_triop_t::gen_lvalue(igen &gen) {
        return sym_t::gen_lvalue(gen);
    }

    int sym_triop_t::gen_rvalue(igen &gen) {
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

    int sym_stmt_t::gen_rvalue(igen &gen) {
        return sym_t::gen_rvalue(gen);
    }

    // ----

    symbol_t sym_stmt_var_t::get_type() const {
        return s_statement_var;
    }

    std::string sym_stmt_var_t::to_string() const {
        return sym_stmt_t::to_string();
    }

    int sym_stmt_var_t::gen_rvalue(igen &gen) {
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

    int sym_block_t::gen_rvalue(igen &gen) {
        return sym_t::gen_rvalue(gen);
    }
}