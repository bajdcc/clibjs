//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <fstream>
#include <cassert>
#include "cjs.h"
#include "cjsparser.h"
#include "cjsgen.h"

#define LOG_AST 0
#define LOG_FILE 0

namespace clib {

    cjs::cjs() {
        rt.init();
    }

    backtrace_direction cjs::check(pda_edge_t edge, ast_node *node) {
        return b_next;
    }

    void cjs::error_handler(int, const std::vector<pda_trans> &, int &) {
    }

    void cjs::exec(const std::string &filename, const std::string &input) {
        auto p = std::make_unique<cjsparser>();
        p->parse(input, this);
#if LOG_AST
        cjsast::print(p->root(), 0, input, std::cout);
#endif
        auto g = std::make_unique<cjsgen>();
        g->gen_code(p->root(), &input);
        p = nullptr;
#if LOG_FILE
        std::ofstream ofs(filename);
        if (ofs)
            cjsast::print(p.root(), 0, input, ofs);
#endif
        auto code = std::move(g->get_code());
        assert(code);
        g = nullptr;
        rt.eval(std::move(code));
    }
}