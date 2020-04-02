//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSRUNTIME_H
#define CLIBJS_CJSRUNTIME_H

#include <list>
#include "cjsgen.h"

namespace clib {

    class js_value : public std::enable_shared_from_this<js_value> {
    public:
        using ref = std::shared_ptr<js_value>;
        using weak_ref = std::weak_ptr<js_value>;
        virtual js_value::ref clone() const = 0;
        virtual runtime_t get_type() = 0;
        weak_ref parent;
        std::vector<weak_ref> children;
    };

    class jsv_number : public js_value {
    public:
        using ref = std::shared_ptr<jsv_number>;
        using weak_ref = std::weak_ptr<jsv_number>;
        explicit jsv_number(double n);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        double number;
    };

    class jsv_string : public js_value {
    public:
        using ref = std::shared_ptr<jsv_string>;
        using weak_ref = std::weak_ptr<jsv_string>;
        explicit jsv_string(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        std::string str;
    };

    class jsv_boolean : public js_value {
    public:
        using ref = std::shared_ptr<jsv_boolean>;
        using weak_ref = std::weak_ptr<jsv_boolean>;
        explicit jsv_boolean(bool flag);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        bool b{false};
    };

    class jsv_regex : public js_value {
    public:
        using ref = std::shared_ptr<jsv_regex>;
        using weak_ref = std::weak_ptr<jsv_regex>;
        explicit jsv_regex(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        std::string re;
    };

    class jsv_array : public js_value {
    public:
        using ref = std::shared_ptr<jsv_array>;
        using weak_ref = std::weak_ptr<jsv_array>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
    };

    class jsv_object : public js_value {
    public:
        using ref = std::shared_ptr<jsv_object>;
        using weak_ref = std::weak_ptr<jsv_object>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
    };

    class jsv_function : public js_value {
    public:
        using ref = std::shared_ptr<jsv_function>;
        using weak_ref = std::weak_ptr<jsv_function>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
    };

    class cjs_function : public std::enable_shared_from_this<cjs_function> {
    public:
        using ref = std::shared_ptr<cjs_function>;
        using weak_ref = std::weak_ptr<cjs_function>;
        explicit cjs_function(sym_code_t::ref code, int pc);
        sym_code_t::ref code;
        int pc{0};
        std::vector<js_value::weak_ref> stack;
        js_value::ref ret_value;
    };

    class cjsruntime {
    public:
        cjsruntime() = default;
        ~cjsruntime() = default;

        cjsruntime(const cjsruntime &) = delete;
        cjsruntime &operator=(const cjsruntime &) = delete;

        void eval(sym_code_t::ref code);

    private:
        int run(const sym_code_t::ref &fun, const cjs_code &code);
        js_value::ref load_const(const sym_code_t::ref &fun, int op);
        void push(js_value::weak_ref value);
        const js_value::weak_ref &top() const;
        js_value::weak_ref pop();

        js_value::ref register_value(const js_value::ref &value);
        void dump_step(const cjs_code &code);

    private:
        std::vector<cjs_function::ref> stack;
        cjs_function::ref current_stack;
        std::list<js_value::ref> objs;
    };
}

#endif //CLIBJS_CJSRUNTIME_H
