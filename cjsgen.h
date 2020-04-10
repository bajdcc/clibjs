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

#define LAMBDA_ID "<lambda>"

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
        s_member_dot,
        s_member_index,
        s_expression_seq,
        s_array,
        s_object,
        s_object_pair,
        s_call_method,
        s_call_function,
        s_ctrl,
        s_statement,
        s_statement_var,
        s_statement_exp,
        s_statement_return,
        s_statement_if,
        s_statement_while,
        s_block,
        s_code,
    };

    class sym_t;

    class sym_code_t;

    class sym_var_id_t;

    class ijsgen {
    public:
        virtual void emit(ast_node_index *, ins_t) = 0;
        virtual void emit(ast_node_index *, ins_t, int) = 0;
        virtual void emit(ast_node_index *, ins_t, int, int) = 0;
        virtual int code_length() const = 0;
        virtual void edit(int, int, int) = 0;
        virtual int load_number(double d) = 0;
        virtual int load_string(const std::string &, int) = 0;
        virtual int push_function(std::shared_ptr<sym_code_t>) = 0;
        virtual void pop_function() = 0;
        virtual void enter(int) = 0;
        virtual void leave() = 0;
        virtual std::shared_ptr<sym_t> get_var(const std::string &, int) = 0;
        virtual void add_var(const std::string &, std::shared_ptr<sym_t>) = 0;
        virtual void add_closure(std::shared_ptr<sym_var_id_t>) = 0;
        virtual int get_func_level() const = 0;
        virtual std::string get_fullname(const std::string &name) const = 0;
        virtual void error(ast_node_index *, const std::string &) const = 0;
    };

    class sym_t : public ast_node_index, public std::enable_shared_from_this<sym_t> {
    public:
        using ref = std::shared_ptr<sym_t>;
        using weak_ref = std::weak_ptr<sym_t>;
        virtual symbol_t get_type() const;
        virtual symbol_t get_base_type() const;
        virtual std::string to_string() const;
        virtual int gen_lvalue(ijsgen &gen);
        virtual int gen_rvalue(ijsgen &gen);
        virtual int gen_invoke(ijsgen &gen, ref &list);
        virtual int set_parent(ref node);
        weak_ref parent;
    };

    enum sym_lvalue_t {
        no_lvalue,
        can_be_lvalue,
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
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        enum class_t {
            local,
            fast,
            closure,
            global,
        } clazz{local};
        ast_node *node{nullptr};
    };

    class sym_var_id_t : public sym_var_t {
    public:
        using ref = std::shared_ptr<sym_var_id_t>;
        explicit sym_var_id_t(ast_node *node, const sym_t::ref &symbol);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int gen_invoke(ijsgen &gen, sym_t::ref &list) override;
        sym_t::weak_ref id;
    private:
        void init_id(ijsgen &gen);
    };

    class sym_id_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_id_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        void parse();
        std::vector<sym_var_t::ref> ids;
        sym_exp_t::ref init;
    };

    class sym_unop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_unop_t>;
        explicit sym_unop_t(sym_exp_t::ref exp, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref exp;
        ast_node *op{nullptr};
    };

    class sym_sinop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_sinop_t>;
        explicit sym_sinop_t(sym_exp_t::ref exp, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref exp;
        ast_node *op{nullptr};
    };

    class sym_binop_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_binop_t>;
        explicit sym_binop_t(sym_exp_t::ref exp1, sym_exp_t::ref exp2, ast_node *op);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
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
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref exp1, exp2, exp3;
        ast_node *op1{nullptr}, *op2{nullptr};
    };

    class sym_member_dot_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_member_dot_t>;
        explicit sym_member_dot_t(sym_exp_t::ref exp);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref exp;
        std::vector<ast_node *> dots;
    };

    class sym_member_index_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_member_index_t>;
        explicit sym_member_index_t(sym_exp_t::ref exp);
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_lvalue(ijsgen &gen) override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref exp;
        std::vector<sym_exp_t::ref> indexes;
    };

    class sym_exp_seq_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_exp_seq_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        std::vector<sym_exp_t::ref> exps;
    };

    class sym_array_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_array_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        std::vector<sym_exp_t::ref> exps;
    };

    class sym_object_pair_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_object_pair_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref key, value;
    };

    class sym_object_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_array_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        std::vector<sym_object_pair_t::ref> pairs;
    };

    class sym_call_method_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_array_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref obj;
        ast_node *method{nullptr};
        std::vector<sym_exp_t::ref> args;
    };

    class sym_call_function_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_array_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_t::ref obj;
        std::vector<sym_exp_t::ref> args;
    };

    class sym_stmt_t : public sym_t {
    public:
        using ref = std::shared_ptr<sym_stmt_t>;
        symbol_t get_type() const override;
        symbol_t get_base_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
    };

    class sym_stmt_var_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        std::vector<sym_id_t::ref> vars;
    };

    class sym_stmt_exp_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_exp_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_seq_t::ref seq;
    };

    class sym_stmt_return_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_exp_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_seq_t::ref seq;
    };

    class sym_stmt_if_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_if_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_seq_t::ref seq;
        sym_stmt_t::ref true_stmt;
        sym_stmt_t::ref false_stmt;
    };

    class sym_stmt_while_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_stmt_while_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        sym_exp_seq_t::ref seq;
        sym_stmt_t::ref stmt;
    };

    class sym_block_t : public sym_stmt_t {
    public:
        using ref = std::shared_ptr<sym_block_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        std::vector<sym_stmt_t::ref> stmts;
    };

    class sym_code_t;

    class cjs_consts {
    public:
        enum get_string_t {
            gs_string,
            gs_name,
            gs_global,
            gs_deref,
            gs_regex,
        };
        int get_number(double n);
        int get_string(const std::string &str, get_string_t type);
        int get_function(std::shared_ptr<sym_code_t> code);
        std::string get_desc(int n) const;
        runtime_t get_type(int n) const;
        char *get_data(int n) const;
        const char *get_name(int n) const;
        const char *get_global(int n) const;
        void dump(const std::string *text) const;
        void save();
        const std::vector<char *> &get_consts_data() const;
        const std::vector<const char *> &get_names_data() const;
        const std::vector<const char *> &get_globals_data() const;
        const std::vector<const char *> &get_derefs_data() const;
    private:
        std::unordered_map<double, int> numbers;
        std::unordered_map<std::string, int> strings;
        std::unordered_map<std::string, int> regexes;
        std::unordered_map<std::string, int> globals;
        std::unordered_map<std::string, int> derefs;
        std::unordered_map<std::string, int> names;
        std::unordered_map<int, std::weak_ptr<sym_code_t>> functions;
        std::vector<runtime_t> consts;
        std::vector<char *> consts_data;
        std::vector<const char *> names_data;
        std::vector<const char *> globals_data;
        std::vector<const char *> derefs_data;
        int index{0};
    };

    void copy_info(sym_t::ref dst, sym_t::ref src);
    void copy_info(sym_t::ref dst, ast_node *src);

    enum cjs_scope_t {
        sp_none,
        sp_block,
        sp_param,
        sp_for,
        sp_for_each,
        sp_while,
        sp_do_while,
        sp_switch,
    };

    enum cjs_scope_query_t {
        sq_local,
        sq_local_func,
        sq_all,
    };

    struct cjs_scope {
        cjs_scope_t type;
        std::unordered_map<std::string, sym_t::weak_ref> vars;
    };

    struct cjs_code {
        int line, column, start, end;
        int code, opnum, op1, op2;
        std::string desc;
    };

    class sym_code_t : public sym_exp_t {
    public:
        using ref = std::shared_ptr<sym_code_t>;
        using weak_ref = std::weak_ptr<sym_code_t>;
        symbol_t get_type() const override;
        std::string to_string() const override;
        int gen_rvalue(ijsgen &gen) override;
        int set_parent(sym_t::ref node) override;
        ast_node *name{nullptr};
        std::string fullname;
        std::string text;
        std::vector<sym_var_t::ref> args;
        std::vector<std::string> args_str;
        sym_t::ref body;
        cjs_consts consts;
        std::vector<cjs_scope> scopes;
        std::vector<cjs_code> codes;
        std::vector<sym_var_id_t::ref> closure;
        std::vector<std::string> closure_str;
    };

    struct cjs_code_result {
        using ref = std::unique_ptr<cjs_code_result>;
        sym_code_t::ref code;
        std::vector<sym_code_t::ref> funcs;
    };

    class cjsgen : public ijsgen {
    public:
        cjsgen();
        ~cjsgen() = default;

        cjsgen(const cjsgen &) = delete;
        cjsgen &operator=(const cjsgen &) = delete;

        bool gen_code(ast_node *node, const std::string *str);
        cjs_code_result::ref get_code() const;

        static void print(const sym_t::ref &node, int level, std::ostream &os);

        void emit(ast_node_index *, ins_t) override;
        void emit(ast_node_index *, ins_t, int) override;
        void emit(ast_node_index *, ins_t, int, int) override;
        int code_length() const override;
        void edit(int, int, int) override;
        int load_number(double d) override;
        int load_string(const std::string &, int) override;
        int push_function(std::shared_ptr<sym_code_t>) override;
        void pop_function() override;
        void enter(int) override;
        void leave() override;
        std::shared_ptr<sym_t> get_var(const std::string &, int) override;
        void add_var(const std::string &, std::shared_ptr<sym_t>) override;
        void add_closure(std::shared_ptr<sym_var_id_t>) override;
        int get_func_level() const override;
        std::string get_fullname(const std::string &name) const override;
        void error(ast_node_index *, const std::string &) const override;

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

        void dump() const;
        void dump(sym_code_t::ref, bool print) const;

    private:
        const std::string *text{nullptr};
        std::vector<std::string> err;
        std::vector<std::vector<ast_node *>> ast;
        std::vector<std::vector<sym_t::ref>> tmp;
        std::vector<sym_code_t::ref> codes;
        std::vector<sym_code_t::ref> funcs;
    };
}

#endif //CLIBJS_CJSGEN_H
