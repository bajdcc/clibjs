//
// Project: clibjs
// Created by bajdcc
//

#include "cjs.h"
#include "cjsparser.h"

namespace clib {

    void cjs::exec(const std::string &input) {
        cjsparser p;
        p.compile(input);
    }
}