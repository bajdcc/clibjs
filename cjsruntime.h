//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSRUNTIME_H
#define CLIBJS_CJSRUNTIME_H

#include <list>
#include "cjsgen.h"

#define JS_BOOL(op) (std::dynamic_pointer_cast<jsv_boolean>(op)->b)
#define JS_NUM(op) (std::dynamic_pointer_cast<jsv_number>(op)->number)
#define JS_STR(op) (std::dynamic_pointer_cast<jsv_string>(op)->str)
#define JS_STR2NUM(op,d) std::dynamic_pointer_cast<jsv_string>(op)->to_number(d)
#define JS_STRF(op) (std::dynamic_pointer_cast<jsv_function>(op)->code->text)

namespace clib {

    std::string trim(std::string s);

    double fix(const double &d);

    class jsv_number;

    class jsv_string;

    class jsv_boolean;

    class jsv_object;

    class jsv_function;

    class jsv_undefined;

    class jsv_null;

    class js_value_new {
    public:
        virtual std::shared_ptr<jsv_number> new_number(double n) = 0;
        virtual std::shared_ptr<jsv_string> new_string(const std::string &s) = 0;
        virtual std::shared_ptr<jsv_boolean> new_boolean(bool b) = 0;
        virtual std::shared_ptr<jsv_object> new_object() = 0;
        virtual std::shared_ptr<jsv_function> new_function() = 0;
        virtual std::shared_ptr<jsv_null> new_null() = 0;
        virtual std::shared_ptr<jsv_undefined> new_undefined() = 0;
    };

    class js_value : public std::enable_shared_from_this<js_value> {
    public:
        enum attr_t {
            at_const = 1U << 0,
            at_readonly = 1U << 1,
        };

        using ref = std::shared_ptr<js_value>;
        using weak_ref = std::weak_ptr<js_value>;
        virtual runtime_t get_type() = 0;
        virtual js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) = 0;
        virtual js_value::ref unary_op(js_value_new &n, int code) = 0;
        virtual bool to_bool() const = 0;
        virtual void mark(int n) = 0;
        virtual void print(std::ostream &os) = 0;
        virtual std::string to_string() const = 0;
        uint8_t marked{0};
        uint8_t attr{0};
        uint8_t reserved1{0};
        uint8_t reserved2{0};
    };

    class jsv_number : public js_value {
    public:
        using ref = std::shared_ptr<jsv_number>;
        using weak_ref = std::weak_ptr<jsv_number>;
        explicit jsv_number(double n);
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
        static std::string number_to_string(double d);
        double number;
    };

    class jsv_string : public js_value {
    public:
        using ref = std::shared_ptr<jsv_string>;
        using weak_ref = std::weak_ptr<jsv_string>;
        explicit jsv_string(std::string s);
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
        int to_number(double &d) const;
        ref clear();
        std::string str;
        double number{0};
        int number_state{0};
        bool calc_number{false};
    private:
        void calc();
    };

    class jsv_boolean : public js_value {
    public:
        static std::string _str_t;
        static std::string _str_f;
        using ref = std::shared_ptr<jsv_boolean>;
        using weak_ref = std::weak_ptr<jsv_boolean>;
        explicit jsv_boolean(bool flag);
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
        bool b{false};
    private:
        js_value::ref binary_true(js_value_new &n, int code, const js_value::ref &op);
        js_value::ref binary_false(js_value_new &n, int code, const js_value::ref &op);
    };

    class jsv_object : public js_value {
    public:
        static std::string _str;
        using ref = std::shared_ptr<jsv_object>;
        using weak_ref = std::weak_ptr<jsv_object>;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
        ref clear();
        std::unordered_map<std::string, js_value::weak_ref> obj;
    };

    class jsv_null : public js_value {
    public:
        static std::string _str;
        using ref = std::shared_ptr<jsv_null>;
        using weak_ref = std::weak_ptr<jsv_null>;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
    };

    class jsv_undefined : public js_value {
    public:
        static std::string _str;
        using ref = std::shared_ptr<jsv_undefined>;
        using weak_ref = std::weak_ptr<jsv_undefined>;
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
    };

    class cjs_function_info;

    class cjs_function;

    class jsv_function : public js_value {
    public:
        using ref = std::shared_ptr<jsv_function>;
        using weak_ref = std::weak_ptr<jsv_function>;
        jsv_function() = default;
        explicit jsv_function(const sym_code_t::ref &c);
        runtime_t get_type() override;
        js_value::ref binary_op(js_value_new &n, int code, const js_value::ref &op) override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        void print(std::ostream &os) override;
        std::string to_string() const override;
        ref clear();
        std::shared_ptr<cjs_function_info> code;
        std::function<void(std::shared_ptr<cjs_function> &, std::vector<js_value::weak_ref> &, js_value_new &)> builtin;
        jsv_object::weak_ref closure;
        std::string name;
    };

    class cjs_function_info : public std::enable_shared_from_this<cjs_function_info> {
    public:
        using ref = std::shared_ptr<cjs_function_info>;
        using weak_ref = std::weak_ptr<cjs_function_info>;
        explicit cjs_function_info(const sym_code_t::ref &code);
        static js_value::ref load_const(const cjs_consts &c, int op);
        std::string fullname;
        std::string text;
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
        explicit cjs_function(const sym_code_t::ref &code);
        explicit cjs_function(cjs_function_info::ref code);
        void reset(const sym_code_t::ref &code);
        void reset(cjs_function_info::ref code);
        void clear();
        void store_name(const std::string &name, js_value::weak_ref obj);
        void store_fast(const std::string &name, js_value::weak_ref obj);
        cjs_function_info::ref info;
        std::string name{"UNKNOWN"};
        int pc{0};
        std::vector<js_value::weak_ref> stack;
        js_value::weak_ref ret_value;
        jsv_object::weak_ref envs;
        jsv_object::weak_ref closure;
    };

    struct cjs_runtime_reuse {
        std::vector<jsv_number::ref> reuse_numbers;
        std::vector<jsv_string::ref> reuse_strings;
        std::vector<jsv_boolean::ref> reuse_booleans;
        std::vector<jsv_object::ref> reuse_objects;
        std::vector<jsv_function::ref> reuse_functions;
    };

    class cjsruntime : public js_value_new {
    public:
        cjsruntime();
        ~cjsruntime() = default;

        cjsruntime(const cjsruntime &) = delete;
        cjsruntime &operator=(const cjsruntime &) = delete;

        void eval(cjs_code_result::ref code);

        jsv_number::ref new_number(double n) override;
        jsv_string::ref new_string(const std::string &s) override;
        jsv_boolean::ref new_boolean(bool b) override;
        jsv_object::ref new_object() override;
        jsv_function::ref new_function() override;
        jsv_null::ref new_null() override;
        jsv_undefined::ref new_undefined() override;

    private:
        int run(const sym_code_t::ref &fun, const cjs_code &code);
        js_value::ref load_const(int op);
        js_value::ref load_fast(int op);
        js_value::ref load_name(int op);
        js_value::ref load_global(int op);
        js_value::ref load_closure(const std::string &name);
        js_value::ref load_deref(const std::string &name);
        void push(js_value::weak_ref value);
        const js_value::weak_ref &top() const;
        js_value::weak_ref pop();

        js_value::ref register_value(const js_value::ref &value);
        void dump_step(const cjs_code &code) const;
        void dump_step2(const cjs_code &code) const;
        void dump_step3() const;

        void reuse_value(const js_value::ref &);
        cjs_function::ref new_stack(const sym_code_t::ref &code);
        cjs_function::ref new_stack(const cjs_function_info::ref &code);
        void delete_stack(const cjs_function::ref &);

        void gc();

        static void print(const js_value::ref &value, int level, std::ostream &os);

    private:
        std::vector<cjs_function::ref> stack;
        cjs_function::ref current_stack;
        std::vector<cjs_function::ref> reuse_stack;
        std::unordered_map<std::string, js_value::weak_ref> global_env;
        std::list<js_value::ref> objs;
        struct _permanents_t {
            jsv_null::ref _null;
            jsv_undefined::ref _undefined;
            jsv_boolean::ref _true;
            jsv_boolean::ref _false;
            jsv_number::ref __nan;
            jsv_number::ref _inf;
            jsv_number::ref _minus_inf;
            jsv_number::ref _zero;
            jsv_number::ref _minus_zero;
            jsv_number::ref _one;
            jsv_number::ref _minus_one;
            jsv_string::ref _empty;
            jsv_function::ref _debug_print;
        } permanents;
        cjs_runtime_reuse reuse;
    };
}

#endif //CLIBJS_CJSRUNTIME_H
