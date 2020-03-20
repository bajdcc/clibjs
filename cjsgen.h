//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSGEN_H
#define CLIBJS_CJSGEN_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include "cjsast.h"

namespace clib {

    enum symbol_t {
        s_sym,
        s_id,
        s_function,
        s_var,
        s_var_id,
        s_expression,
        s_unop,
        s_sinop,
        s_binop,
        s_triop,
        s_list,
        s_ctrl,
        s_statement,
        s_statement_var,
        s_block,
    };

    class igen {
    public:
        virtual void emit(ins_t) = 0;
        virtual void emit(ins_t, int) = 0;
        virtual void emit(ins_t, int, int) = 0;
        virtual void emit(int, int, lexer_t) = 0;
        virtual int current() const = 0;
        virtual void edit(int, int) = 0;
        virtual int load_string(const std::string &) = 0;
        virtual void add_label(int, int, int, const std::string &) = 0;
        virtual void error(int, int, const std::string &) const = 0;
    };

    class sym_t {
    public:
        using ref = std::shared_ptr<sym_t>;
        using weak_ref = std::weak_ptr<sym_t>;
        virtual symbol_t get_type() const;
        virtual symbol_t get_base_type() const;
        virtual std::string to_string() const;
        virtual int gen_lvalue(igen &gen);
        virtual int gen_rvalue(igen &gen);
        virtual int gen_invoke(igen &gen, ref &list);
        int line{0}, column{0}, start{0}, end{0};
    };

    enum sym_class_t {
        z_undefined,
        z_local_var,
        z_param_var,
        z_function,
        z_end,
    };

    class sym_exp_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_exp_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
    };

    class sym_var_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_var_t>;
        explicit sym_var_t(ast_node *node);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        ast_node *node{nullptr};
    };

    class sym_var_id_t : public sym_var_t {
    public:
        using ref = std::shared_ptr<sym_var_id_t>;
        explicit sym_var_id_t(ast_node *node, const sym_t::ref &symbol);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        int gen_invoke(igen &gen, sym_t::ref &list) override;
        sym_t::weak_ref id;
    };

    class sym_id_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_id_t>;
        explicit sym_id_t(sym_var_t::ref id);
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        sym_var_t::ref id;
        sym_exp_t::ref init;
        sym_class_t clazz{z_undefined};
    };

    class sym_unop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_unop_t>;
        explicit sym_unop_t(sym_exp_t::ref exp, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        sym_exp_t::ref exp;
        ast_node *op{nullptr};
    };

    class sym_sinop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_sinop_t>;
        explicit sym_sinop_t(sym_exp_t::ref exp, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        sym_exp_t::ref exp;
        ast_node *op{nullptr};
    };

    class sym_binop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_binop_t>;
        explicit sym_binop_t(sym_exp_t::ref exp1, sym_exp_t::ref exp2, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        sym_exp_t::ref exp1, exp2;
        ast_node *op{nullptr};
    };

    class sym_triop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_triop_t>;
        explicit sym_triop_t(sym_exp_t::ref exp1, sym_exp_t::ref exp2,
                             sym_exp_t::ref exp3, ast_node *op1, ast_node *op2);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(igen &gen) override;
        int gen_rvalue(igen &gen) override;
        sym_exp_t::ref exp1, exp2, exp3;
        ast_node *op1{nullptr}, *op2{nullptr};
    };

    class sym_stmt_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_stmt_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_rvalue(igen &gen) override;
        sym_t::ref r;
    };

    class sym_stmt_var_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(igen &gen) override;
        std::vector<sym_id_t::ref> vars;
    };

    class sym_block_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_block_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_rvalue(igen &gen) override;
        std::vector<sym_stmt_t::ref> stmts;
    };

    class cjs_consts {
    public:
        int get_number(double n);
        int get_string(const std::string &str);
        std::unordered_map<double, int> numbers;
        std::unordered_map<std::string, int> strings;
    };

    enum cjs_scope_t {
        sp_function,
        sp_block,
        sp_for,
        sp_for_each,
        sp_while,
        sp_do_while,
        sp_switch,
    };
    struct cjs_scope {
        cjs_scope_t type;
    };

    class cjsgen {
    public:
        cjsgen();
        ~cjsgen() = default;

        cjsgen(const cjsgen &) = delete;
        cjsgen &operator=(const cjsgen &) = delete;

        bool gen_code(ast_node *node);

        static void print(const sym_t::ref &node, int level, std::ostream &os);

    private:
        void gen_rec(ast_node *node, int level);
        void gen_coll(const std::vector<ast_node *> &nodes, int level, ast_node *node);
        bool gen_before(const std::vector<ast_node *> &nodes, int level, ast_node *node);
        void gen_after(const std::vector<ast_node *> &nodes, int level, ast_node *node);

        void error(ast_node *, const std::string &, bool info = false) const;
        void error(sym_t::ref s, const std::string &) const;

        sym_exp_t::ref to_exp(sym_t::ref s);

        sym_t::ref find_symbol(ast_node *node);
        sym_var_t::ref primary_node(ast_node *node);

    private:
        std::vector<std::string> err;
        std::vector<char> data;
        std::vector<char> consts;
        std::vector<std::vector<ast_node *>> ast;
        std::vector<std::vector<sym_t::ref>> tmp;
        std::vector<cjs_scope> scopes;
    };
}

#endif //CLIBJS_CJSGEN_H
