//
// Project: clibjs
// Created by bajdcc
//

#include <iostream>
#include <fstream>
#include "cjs.h"
#include "cjsparser.h"
#include "cjsgen.h"
#include "cjsruntime.h"

#define LOG_FILE 0

namespace clib {

    backtrace_direction cjs::check(pda_edge_t edge, ast_node *node) {
        return b_next;
    }

    void cjs::error_handler(int, const std::vector<pda_trans> &, int &) {
    }

    void cjs::exec(const std::string &filename, const std::string &input) {
        cjsparser p;
        p.parse(input, this);
        cjsast::print(p.root(), 0, input, std::cout);
        cjsgen g;
        g.gen_code(p.root(), &input);
#if LOG_FILE
        std::ofstream ofs(filename);
        if (ofs)
            cjsast::print(p.root(), 0, input, ofs);
#endif
        auto fun = g.get_code();
        cjsruntime rt;
        rt.eval(fun);
    }
}