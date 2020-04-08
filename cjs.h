//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJS_H
#define CLIBJS_CJS_H

#include <string>
#include "cjsparser.h"
#include "cjsruntime.h"

namespace clib {

    class cjs : public csemantic {
    public:
        cjs() = default;
        ~cjs() = default;

        cjs(const cjs&) = delete;
        cjs& operator=(const cjs&) = delete;

        backtrace_direction check(pda_edge_t, ast_node*) override;
        void error_handler(int, const std::vector<pda_trans>&, int&) override;

        void exec(const std::string &filename, const std::string& input);

    private:
        cjsruntime rt;
    };
}

#endif //CLIBJS_CJS_H
