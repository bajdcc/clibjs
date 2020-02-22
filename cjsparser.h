//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSPARSER_H
#define CLIBJS_CJSPARSER_H

#include <string>
#include <memory>
#include "cjslexer.h"

namespace clib {

    class cjs_node : public std::enable_shared_from_this<cjs_node> {
    public:
        using ptr = std::shared_ptr<cjs_node>;

        int line{0};
        int column{0};
        int start{0};
        int end{0};
        int _start{0};
        int _end{0};
        int _next{0};

        virtual const char *get_name() const;
        virtual int print(std::ostream &os, int level) const = 0;
    };

    std::ostream &operator<<(std::ostream &os, std::tuple<const std::shared_ptr<cjs_node>, int> arg);

    class cjs_expr : public cjs_node {
    public:
        using ptr = std::shared_ptr<cjs_expr>;
    };

    class cjs_expr_sinop : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_sinop>;

        int print(std::ostream &os, int level) const override;

        cjslexer::lexer_t op;
        cjs_expr::ptr exp;
    };

    class cjs_expr_unary : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_unary>;

        int print(std::ostream &os, int level) const override;

        cjslexer::lexer_t op;
        cjs_expr::ptr exp;
    };

    class cjs_expr_binop : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_binop>;

        int print(std::ostream &os, int level) const override;

        cjslexer::lexer_t op;
        cjs_expr::ptr exp1, exp2;
    };

    class cjs_expr_triop : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_triop>;

        int print(std::ostream &os, int level) const override;

        cjslexer::lexer_t op1, op2;
        cjs_expr::ptr exp1, exp2, exp3;
    };

    class cjs_expr_id : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_id>;

        int print(std::ostream &os, int level) const override;

        std::string id;
    };

    class cjs_expr_number : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_number>;

        int print(std::ostream &os, int level) const override;

        double number{0};
    };

    class cjs_expr_string : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_string>;

        int print(std::ostream &os, int level) const override;

        std::string str;
    };

    class cjs_expr_regex : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_regex>;

        int print(std::ostream &os, int level) const override;

        std::string re;
    };

    class cjs_expr_bool : public cjs_expr {
    public:
        using ptr = std::shared_ptr<cjs_expr_bool>;

        int print(std::ostream &os, int level) const override;

        bool b{false};
    };

    class cjs_stmt : public cjs_node {
    public:
        using ptr = std::shared_ptr<cjs_stmt>;

        cjs_expr::ptr exp;
    };

    class cjs_block : public cjs_node {
    public:
        using ptr = std::shared_ptr<cjs_block>;

        int print(std::ostream &os, int level) const override;

        std::vector<cjs_stmt::ptr> stmts;
    };

    class cjs_stmt_var_decl : public cjs_node {
    public:
        using ptr = std::shared_ptr<cjs_stmt_var_decl>;

        int print(std::ostream &os, int level) const override;

        std::string id;
        cjs_expr::ptr expr;
    };

    class cjs_stmt_var_decls : public cjs_stmt {
    public:
        using ptr = std::shared_ptr<cjs_stmt_var_decls>;

        const char *get_name() const override;
        int print(std::ostream &os, int level) const override;

        std::vector<cjs_stmt_var_decl::ptr> decls;
    };

    class cjsparser {
    public:

        void compile(const std::string &input);

    private:
        cjs_stmt::ptr parse_stmt(int start, int end) const;
        cjs_stmt_var_decls::ptr parse_var_decls(int start, int end) const;
        cjs_stmt_var_decl::ptr parse_var_decl(int start, int end) const;
        cjs_expr::ptr parse_expr(int start, int end, int level) const;

    private:
        cjslexer L;
    };
}

#endif //CLIBJS_CJSPARSER_H
