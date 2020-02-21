//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJS_H
#define CLIBJS_CJS_H

#include <string>

namespace clib {

    class cjs {
    public:
        void exec(const std::string& input);
    };
}

#endif //CLIBJS_CJS_H
