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
        virtual js_value::ref binary_op(int code, js_value::ref op) = 0;
        virtual void mark(int n) = 0;
        int marked{0};
    };

    class jsv_number : public js_value {
    public:
        using ref = std::shared_ptr<jsv_number>;
        using weak_ref = std::weak_ptr<jsv_number>;
        explicit jsv_number(double n);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
        double number;
    };

    class jsv_string : public js_value {
    public:
        using ref = std::shared_ptr<jsv_string>;
        using weak_ref = std::weak_ptr<jsv_string>;
        explicit jsv_string(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
        std::string str;
    };

    class jsv_boolean : public js_value {
    public:
        using ref = std::shared_ptr<jsv_boolean>;
        using weak_ref = std::weak_ptr<jsv_boolean>;
        explicit jsv_boolean(bool flag);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
        bool b{false};
    };

    class jsv_regex : public js_value {
    public:
        using ref = std::shared_ptr<jsv_regex>;
        using weak_ref = std::weak_ptr<jsv_regex>;
        explicit jsv_regex(std::string s);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
        std::string re;
    };

    class jsv_array : public js_value {
    public:
        using ref = std::shared_ptr<jsv_array>;
        using weak_ref = std::weak_ptr<jsv_array>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
    };

    class jsv_object : public js_value {
    public:
        using ref = std::shared_ptr<jsv_object>;
        using weak_ref = std::weak_ptr<jsv_object>;
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
    };

    class cjs_function_info;

    class jsv_function : public js_value {
    public:
        using ref = std::shared_ptr<jsv_function>;
        using weak_ref = std::weak_ptr<jsv_function>;
        explicit jsv_function(sym_code_t::weak_ref c);
        js_value::ref clone() const override;
        runtime_t get_type() override;
        js_value::ref binary_op(int code, js_value::ref op) override;
        void mark(int n) override;
        std::shared_ptr<cjs_function_info> code;
        std::string name;
    };

    class cjs_function_info : public std::enable_shared_from_this<cjs_function_info> {
    public:
        using ref = std::shared_ptr<cjs_function_info>;
        using weak_ref = std::weak_ptr<cjs_function_info>;
        explicit cjs_function_info(const sym_code_t::ref& code);
        static js_value::ref load_const(const cjs_consts &c, int op);
        std::string fullname;
        std::vector<std::string> args;
        std::vector<std::string> names;
        std::vector<std::string> globals;
        std::vector<std::string> derefs;
        std::vector<js_value::ref> consts;
        std::vector<cjs_code> codes;
        std::vector<std::string> closure;
    };

    class cjs_function : public std::enable_shared_from_this<cjs_function> {
    public:
        using ref = std::shared_ptr<cjs_function>;
        using weak_ref = std::weak_ptr<cjs_function>;
        explicit cjs_function(sym_code_t::ref code);
        explicit cjs_function(cjs_function_info::ref code);
        void store_name(const std::string &name, js_value::weak_ref obj);
        void store_fast(const std::string &name, js_value::weak_ref obj);
        cjs_function_info::ref info;
        std::string name{"UNKNOWN"};
        int pc{0};
        std::vector<js_value::weak_ref> stack;
        js_value::weak_ref ret_value;
        std::unordered_map<std::string, js_value::weak_ref> envs;
    };

    class cjsruntime {
    public:
        cjsruntime() = default;
        ~cjsruntime() = default;

        cjsruntime(const cjsruntime &) = delete;
        cjsruntime &operator=(const cjsruntime &) = delete;

        void eval(cjs_code_result::ref code);

    private:
        int run(const sym_code_t::ref &fun, const cjs_code &code);
        js_value::ref load_const(int op);
        js_value::ref load_fast(int op);
        js_value::ref load_name(int op);
        js_value::ref load_global(int op);
        void push(js_value::weak_ref value);
        const js_value::weak_ref &top() const;
        js_value::weak_ref pop();

        js_value::ref register_value(const js_value::ref &value);
        void dump_step(const cjs_code &code) const;
        void dump_step2(const cjs_code &code) const;
        void dump_step3() const;

        void gc();

        static void print(const js_value::ref &value, int level, std::ostream &os);

    private:
        std::vector<cjs_function::ref> stack;
        cjs_function::ref current_stack;
        std::list<js_value::ref> objs;
    };
}

#endif //CLIBJS_CJSRUNTIME_H
