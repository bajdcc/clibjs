//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSGEN_H
#define CLIBJS_CJSGEN_H

#include <vector>
#include <string>
#include <memory>
#include "cjsast.h"

namespace clib {

    class sym_t {
    public:
        using ref = std::shared_ptr<sym_t>;
        using weak_ref = std::weak_ptr<sym_t>;
        int line{0}, column{0}, start{0}, end{0};
    };

    class cjsgen {
    public:
        cjsgen();
        ~cjsgen() = default;

        cjsgen(const cjsgen &) = delete;
        cjsgen &operator=(const cjsgen &) = delete;

        bool gen_code(ast_node *node);

    private:
        void gen_rec(ast_node *node, int level);
        void gen_coll(const std::vector<ast_node *> &nodes, int level, ast_node *node);

    private:
        std::vector<std::string> err;
        std::vector<char> data;
        std::vector<char> consts;
        std::vector<std::vector<ast_node *>> ast;
        std::vector<std::vector<sym_t::ref>> tmp;
        bool gen_before(const std::vector<ast_node *> &nodes, int level, ast_node *node);
        void gen_after(const std::vector<ast_node *> &nodes, int level, ast_node *node);
    };
}

#endif //CLIBJS_CJSGEN_H
