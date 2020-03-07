//
// Project: clibjs
// Created by bajdcc
//

#include "cjsmem.h"

namespace clib {

    char *cjsmem::alloc(size_t size) {
        if (removed.empty()) {
            data.emplace_back(size);
            return data.back().data();
        }
        auto&d = data[*removed.begin()];
        removed.erase(removed.begin());
        d.resize(size);
        return d.data();
    }

    void cjsmem::free(char *ptr) {
        auto f = map_data.find(ptr);
        if (f != map_data.end()) {
            if (data[f->second].size() > 1 << 8)
                data[f->second].clear();
            if (f->second + 1 == data.size()) {
                map_data.erase(ptr);
                data.pop_back();
                while (!removed.empty()) {
                    auto i = *removed.begin();
                    if (i + 1 == data.size()) {
                        removed.erase(i);
                        map_data.erase(data.back().data());
                        data.pop_back();
                    }
                }
            } else
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