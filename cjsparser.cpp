//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include "cjsparser.h"

namespace clib {

#define COPY_INFO(node, U, start_, end_, next_) \
    do { \
        node->line = U.line; \
        node->column = U.column; \
        node->start = U.start; \
        node->end = U.end; \
        node->_start = start_; \
        node->_end = end_; \
        node->_next = next_; \
    } while (0)

    void cjsparser::compile(const std::string &input) {
        L.input(input);
        L.dump();
        auto block = std::make_shared<cjs_block>();
        auto i = 0;
        auto end = L.get_unit_size();
        while (i < end) {
            auto obj = parse_stmt(i, end);
            if (!obj)break;
            block->stmts.push_back(obj);
            i = obj->_next;
        }
        block->print(std::cout, 0);
    }

    cjs_stmt::ptr cjsparser::parse_stmt(int start, int end) const {
        if (start >= end)
            return nullptr;
        auto U = L.get_unit(start);
        if (U.t == cjslexer::K_VAR) {
            return parse_var_decls(start + 1, end);
        }
        return nullptr;
    }

    cjs_stmt_var_decls::ptr cjsparser::parse_var_decls(int start, int end) const {
        if (start >= end)
            return nullptr;
        auto decls = std::make_shared<cjs_stmt_var_decls>();
        int i = start;
        while (i < end) {
            auto U = L.get_unit(i);
            if (U.t == cjslexer::ID) {
                auto decl = parse_var_decl(i, end);
                if (!decl) {
                    return nullptr;
                }
                decls->decls.push_back(decl);
                i = decl->_next;
            } else if (U.t == cjslexer::T_SEMI) {
                COPY_INFO(decls, L.get_unit(start - 1), start - 1, i, i + 1);
                break;
            } else {
                fprintf(stderr, "A Location: %s, Error: var !ID! = ...\n", L.get_unit_desc(start).c_str());
                return nullptr;
            }
        }
        return decls;
    }

    cjs_stmt_var_decl::ptr cjsparser::parse_var_decl(int start, int end) const {
        auto decl = std::make_shared<cjs_stmt_var_decl>();
        auto next = start + 1;
        auto U = L.get_unit(start);
        COPY_INFO(decl, U, start, next, next);
        decl->id = L.get_data(U.idx);
        auto N = L.get_unit(next); // next
        if (N.t == cjslexer::T_ASSIGN) {
            decl->_next++;
            auto expr = parse_expr(start + 2, end, -1);
            if (!expr) {
                return nullptr;
            }
            decl->expr = expr;
            next = expr->_next;
            decl->_next = next;
        }
        N = L.get_unit(next); // next
        if (N.t == cjslexer::T_COMMA) {
            decl->_next = next + 1;
        }
        return decl;
    }

    cjs_expr::ptr cjsparser::parse_expr(int start, int end, int level) const {
        auto U = L.get_unit(start);
        cjs_expr::ptr ret;
        if (U.t >= cjslexer::NUMBER && U.t <= cjslexer::STRING) {
            if (U.t == cjslexer::ID) {
                auto exp = std::make_shared<cjs_expr_id>();
                COPY_INFO(exp, U, start, start + 1, start + 1);
                exp->id = L.get_data(U.idx);
                ret = exp;
            } else if (U.t == cjslexer::NUMBER) {
                auto exp = std::make_shared<cjs_expr_number>();
                COPY_INFO(exp, U, start, start + 1, start + 1);
                exp->number = *(double *) L.get_data(U.idx);
                ret = exp;
            } else if (U.t == cjslexer::STRING) {
                auto exp = std::make_shared<cjs_expr_string>();
                COPY_INFO(exp, U, start, start + 1, start + 1);
                exp->str = L.get_data(U.idx);
                ret = exp;
            } else {
                auto exp = std::make_shared<cjs_expr_regex>();
                COPY_INFO(exp, U, start, start + 1, start + 1);
                exp->re = L.get_data(U.idx);
                ret = exp;
            }
        } else if (U.t > cjslexer::OPERATOR_START && U.t < cjslexer::OPERATOR_END) {
            fprintf(stderr, "A Location: %s, Error: expr -> !op!\n", L.get_unit_desc(start).c_str());
        } else if (U.t > cjslexer::KEYWORD_START && U.t < cjslexer::KEYWORD_END) {
            if (U.t == cjslexer::K_TRUE || U.t == cjslexer::K_FALSE) {
                auto exp = std::make_shared<cjs_expr_bool>();
                COPY_INFO(exp, U, start, start + 1, start + 1);
                exp->b = U.t == cjslexer::K_TRUE;
                ret = exp;
            } else {
                fprintf(stderr, "A Location: %s, Error: expr -> !keyword!\n", L.get_unit_desc(start).c_str());
            }
        }
        return ret;
    }
}