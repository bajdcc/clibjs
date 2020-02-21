//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSLEXER_H
#define CLIBJS_CJSLEXER_H

#include <string>
#include <vector>
#include <unordered_map>

namespace clib {

    class cjslexer {
    public:
        cjslexer();

        enum lexer_t {
            NONE,
            NUMBER,
            ID,
            REGEX,
            STRING,
            SPACE,
            NEWLINE,
            COMMENT,
            END,
            KEYWORD_START,
            K_NEW,
            K_VAR,
            K_LET,
            K_FUNCTION,
            K_IF,
            K_ELSE,
            K_FOR,
            K_WHILE,
            K_IN,
            K_DO,
            K_BREAK,
            K_CONTINUE,
            K_RETURN,
            K_SWITCH,
            K_DEFAULT,
            K_CASE,
            K_NULL,
            K_TRUE,
            K_FALSE,
            KEYWORD_END,
            OPERATOR_START,
            T_ADD,
            T_SUB,
            T_MUL,
            T_DIV,
            T_MOD,
            T_POWER,
            T_INC,
            T_DEC,
            T_ASSIGN,
            T_ASSIGN_ADD,
            T_ASSIGN_SUB,
            T_ASSIGN_MUL,
            T_ASSIGN_DIV,
            T_ASSIGN_MOD,
            T_ASSIGN_LSHIFT,
            T_ASSIGN_RSHIFT,
            T_ASSIGN_URSHIFT,
            T_ASSIGN_AND,
            T_ASSIGN_OR,
            T_ASSIGN_XOR,
            T_ASSIGN_POWER,
            T_LESS,
            T_LESS_EQUAL,
            T_GREATER,
            T_GREATER_EQUAL,
            T_EQUAL,
            T_FEQUAL,
            T_NOT_EQUAL,
            T_FNOT_EQUAL,
            T_LOG_NOT,
            T_LOG_AND,
            T_LOG_OR,
            T_BIT_NOT,
            T_BIT_AND,
            T_BIT_OR,
            T_BIT_XOR,
            T_DOT,
            T_COMMA,
            T_SEMI,
            T_COLON,
            T_QUERY,
            T_LSHIFT,
            T_RSHIFT,
            T_URSHIFT,
            T_LPARAN,
            T_RPARAN,
            T_LSQUARE,
            T_RSQUARE,
            T_LBRACE,
            T_RBRACE,
            OPERATOR_END,
        };

        struct unit {
            lexer_t t{NONE};
            int idx{0};
            int len{0};
            int line{0};
            int column{0};
            int start{0};
            int end{0};
        };

        void input(const std::string &text);
        void dump() const;

        const unit &get_unit(int idx) const;
        const char *get_data(int idx) const;

    private:
        bool allow_expr() const;
        int alloc(int size);
        static unit alloc_unit(int line, int column, int start, int end);

    private:
        std::string text;
        std::vector<char> data;
        std::vector<unit> units;
        std::unordered_map<std::string, lexer_t> mapKeyword;
    };
}

#endif //CLIBJS_CJSLEXER_H
