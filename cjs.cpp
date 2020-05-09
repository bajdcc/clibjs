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
#define LOG_FILENAME "output.txt"
#define LIBRARY_FILE R"(..\\lib\\clib.js)"

namespace clib {

    cjs::cjs() {
        rt.set_readonly(false);
        rt.init(this);
        init_lib();
        rt.set_readonly(true);
    }

    backtrace_direction cjs::check(pda_edge_t edge, ast_node *node) {
        return b_next;
    }

    void cjs::error_handler(int, const std::vector<pda_trans> &, int &) {
    }

    void cjs::exec(const std::string &filename, const std::string &input, bool top) {
        auto p = std::make_unique<cjsparser>();
        p->parse(input, this);
#if LOG_AST
        cjsast::print(p->root(), 0, input, std::cout);
#endif
        auto g = std::make_unique<cjsgen>();
        g->gen_code(p->root(), &input, filename);
        p = nullptr;
#if LOG_FILE
        std::ofstream ofs(LOG_FILENAME);
        if (ofs)
            cjsast::print(p.root(), 0, input, ofs);
#endif
        auto code = std::move(g->get_code());
        assert(code);
        if (!filename.empty() && filename[0] != '<')
            code->code->debugName = "(" + filename + ":1:1) <entry>";
        else
            code->code->debugName = "(" + filename + ") <entry>";
        g = nullptr;
        rt.eval(std::move(code), top);
    }

    void cjs::init_lib() {
        char buf[256];
        snprintf(buf, sizeof(buf), "sys.exec_file(\"%s\");\n", LIBRARY_FILE);
        try {
            exec("<library>", buf);
        } catch (const clib::cexception &e) {
            std::cout << e.message() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
}