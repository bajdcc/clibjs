//
// Project: clibjs
// Created by bajdcc
//

#include "cjsmem.h"

namespace clib {

    char *cjsmem::alloc(size_t size) {
        data.emplace_back();
        data.back().resize(size);
        return data.back().data();
    }

    void cjsmem::free(char *ptr) {
        auto f = map_data.find(ptr);
        if (f != map_data.end()) {
            data[f->second].clear();
            removed.insert(f->second);
        }
    }

    void cjsmem::gc() {

    }

    void cjsmem::reset() {
        data.clear();
        map_data.clear();
        removed.clear();
    }
}