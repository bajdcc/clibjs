//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <iomanip>
#include "cjsparser.h"

namespace clib {

    std::ostream &operator<<(std::ostream &os, std::tuple<const std::shared_ptr<cjs_node>, int> arg) {
        std::get<0>(arg)->print(os, std::get<1>(arg));
        return os;
    }

    const char *cjs_node::get_name() const {
        return "NODE";
    }

    int cjs_expr_sinop::print(std::ostream &os, int level) const {
        return 0;
    }

    int cjs_expr_unary::print(std::ostream &os, int level) const {
        return 0;
    }

    int cjs_expr_binop::print(std::ostream &os, int level) const {
        return 0;
    }

    int cjs_expr_triop::print(std::ostream &os, int level) const {
        return 0;
    }

    int cjs_expr_id::print(std::ostream &os, int level) const {
        os << id;
    }

    int cjs_expr_number::print(std::ostream &os, int level) const {
        os << number;
    }

    int cjs_expr_string::print(std::ostream &os, int level) const {
        os << str;
    }

    int cjs_expr_regex::print(std::ostream &os, int level) const {
        os << re;
    }

    int cjs_expr_bool::print(std::ostream &os, int level) const {
        os << std::boolalpha << b;
    }

    int cjs_block::print(std::ostream &os, int level) const {
        if (!stmts.empty()) {
            if (level == 0) {
                for (const auto &s : stmts) {
                    char buf[128];
                    snprintf(buf, sizeof(buf), "S [%04d-%04d] Line: %04d, Column: %03d |%-10s| ",
                             s->start, s->end, s->line, s->column, s->get_name());
                    os << buf << std::make_tuple(s, level + 1) << std::endl;
                }
            } else {
                for (const auto &s : stmts) {
                    os << std::setfill(' ') << std::setw(level * 2) << std::make_tuple(s, level + 1) << std::endl;
                }
            }
        }
        return 0;
    }

    int cjs_stmt_var_decl::print(std::ostream &os, int level) const {
        os << id;
        if (expr) {
            os << " = " << std::make_tuple(expr, level);
        }
        return 0;
    }

    const char *cjs_stmt_var_decls::get_name() const {
        return "VAR";
    }

    int cjs_stmt_var_decls::print(std::ostream &os, int level) const {
        if (!decls.empty()) {
            os << "var ";
            for (const auto &s : decls) {
                os << std::make_tuple(s, level);
                if (&s != &decls.back()) {
                    os << ", ";
                }
            }
            os << ";";
        }
        return 0;
    }
}