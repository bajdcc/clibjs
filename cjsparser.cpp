//
// Project: clibjs
// Created by bajdcc
//

#include "cjsparser.h"
#include "cjslexer.h"

namespace clib {

    void cjsparser::compile(const std::string &input) {
        cjslexer L;
        L.input(input);
        //L.dump();
    }
}