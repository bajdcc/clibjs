//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSPARSER_H
#define CLIBJS_CJSPARSER_H

#include <string>
#include <memory>
#include <unordered_set>
#include "cjslexer.h"
#include "cjsast.h"
#include "cjsunit.h"

namespace clib {

    using namespace types;

    enum backtrace_direction {
        b_success,
        b_next,
        b_error,
        b_fail,
        b_fallback,
    };

    struct backtrace_t {
        int lexer_index;
        std::vector<int> state_stack;
        std::vector<ast_node *> ast_stack;
        int current_state;
        uint32_t coll_index;
        uint32_t reduce_index;
        std::vector<int> trans_ids;
        std::unordered_set<int> ast_ids;
        backtrace_direction direction;
    };

    class csemantic {
    public:
        virtual backtrace_direction check(pda_edge_t, ast_node *) = 0;
        virtual void error_handler(int, const std::vector<pda_trans> &, int &) = 0;
    };

    class cjsparser {
    public:
        cjsparser() = default;
        ~cjsparser() = default;

        cjsparser(const cjsparser &) = delete;
        cjsparser &operator=(const cjsparser &) = delete;

        ast_node *parse(const std::string &str, csemantic *s = nullptr);
        ast_node *root() const;
        void clear_ast();

    private:

        void next();

        void gen();
        void program();
        ast_node *terminal();

        bool valid_trans(const pda_trans &trans) const;
        void do_trans(int state, backtrace_t &bk, const pda_trans &trans);
        bool LA(unit *u) const;

        void expect(bool, const std::string &);
        void match_type(lexer_t);

        void error(const std::string &);

    private:
        const lexer_unit *current{nullptr};
        std::vector<int> state_stack;
        std::vector<ast_node *> ast_stack;
        std::vector<ast_node *> ast_cache;
        size_t ast_cache_index{0};
        std::vector<ast_node *> ast_coll_cache;
        std::vector<ast_node *> ast_reduce_cache;

    private:
        cjsunit unit;
        std::unique_ptr<cjslexer> lexer;
        csemantic *semantic{nullptr};
        std::unique_ptr<cjsast> ast;
    };
}

#endif //CLIBJS_CJSPARSER_H
