//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSAST_H
#define CLIBJS_CJSAST_H

#include <cstdint>
#include "cjstypes.h"
#include "cjsmem.h"

namespace clib {

    using namespace types;

    // 结点
    struct ast_node {
        // 类型
        uint16_t flag;
        uint16_t attr;
        int line, column, start, end;

        union {
            double _number;
            const char* _string;
            const char* _regex;
            const char* _identifier;
            lexer_t _keyword;
            lexer_t _op;
            coll_t _coll;
            struct {
                uint32_t _1, _2;
            } _ins;
        } data; // 数据

        // 树型数据结构，广义表
        ast_node* parent; // 父亲
        ast_node* prev; // 左兄弟
        ast_node* next; // 右兄弟
        ast_node* child; // 最左儿子
    };

    class cjsast {
    public:
        cjsast();
        ~cjsast() = default;

        cjsast(const cjsast&) = delete;
        cjsast& operator=(const cjsast&) = delete;

        ast_node* get_root() const;

        ast_node* new_node(ast_t type);
        ast_node* new_child(ast_t type, bool step = true);
        ast_node* new_sibling(ast_t type, bool step = true);

        void remove(ast_node*);

        ast_node* add_child(ast_node*);
        static ast_node* set_child(ast_node*, ast_node*);
        static ast_node* set_sibling(ast_node*, ast_node*);
        static int children_size(ast_node*);

        void set_str(ast_node* node, const std::string& str);
        static std::string display_str(const char* str);

        void to(ast_to_t type);

        static void print(ast_node* node, int level, std::ostream& os);
        static std::string to_string(ast_node* node);
        static bool ast_equal(ast_t type, lexer_t lex);

        static void unlink(ast_node* node);

        static ast_node* index(ast_node* node, int index);
        static ast_node* index(ast_node* node, const std::string& index);

        void reset();
    private:
        void init();

    private:
        cjsmem nodes; // 全局AST结点内存管理
        cjsmem strings; // 全局字符串管理
        ast_node* root{ nullptr }; // 根结点
        ast_node* current{ nullptr }; // 当前结点
    };
};


#endif //CLIBJS_CJSAST_H
