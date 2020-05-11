//
// Project: clibjs
// Created by bajdcc
//

#ifndef CLIBJS_CJSRUNTIME_H
#define CLIBJS_CJSRUNTIME_H

#include <chrono>
#include <list>
#include <map>
#include "cjsgen.h"

#define JS_BOOL(op) (std::dynamic_pointer_cast<jsv_boolean>(op)->b)
#define JS_NUM(op) (std::dynamic_pointer_cast<jsv_number>(op)->number)
#define JS_STR(op) (std::dynamic_pointer_cast<jsv_string>(op)->str)
#define JS_STR2NUM(op, d) std::dynamic_pointer_cast<jsv_string>(op)->to_number(d)
#define JS_STRF(op) (std::dynamic_pointer_cast<jsv_function>(op)->code->text)
#define JS_OBJ(op) (std::dynamic_pointer_cast<jsv_object>(op)->obj)
#define JS_O(op) (std::dynamic_pointer_cast<jsv_object>(op))
#define JS_FUN(op) (std::dynamic_pointer_cast<jsv_function>(op))
#define JS_V(op) (std::dynamic_pointer_cast<js_value>(op))

namespace clib {

    std::string trim(std::string s);

    double fix(const double &d);

    class js_value;

    class jsv_number;

    class jsv_string;

    class jsv_boolean;

    class jsv_object;

    class jsv_function;

    class jsv_undefined;

    class jsv_null;

    class cjs_function;

    class cjs_function_info;

    class js_value_new {
    public:
        virtual std::shared_ptr<jsv_number> new_number(double n) = 0;
        virtual std::shared_ptr<jsv_string> new_string(const std::string &s) = 0;
        virtual std::shared_ptr<jsv_boolean> new_boolean(bool b) = 0;
        virtual std::shared_ptr<jsv_object> new_object() = 0;
        virtual std::shared_ptr<jsv_object> new_object_box(const std::shared_ptr<js_value> &) = 0;
        virtual std::shared_ptr<jsv_function> new_function() = 0;
        virtual std::shared_ptr<jsv_null> new_null() = 0;
        virtual std::shared_ptr<jsv_undefined> new_undefined() = 0;
        virtual std::shared_ptr<cjs_function> new_func(const std::shared_ptr<cjs_function_info> &code) = 0;
        virtual std::shared_ptr<jsv_object> new_array() = 0;
        virtual std::shared_ptr<jsv_object> new_error(int) = 0;
        virtual void exec(const std::string &, const std::string &) = 0;
        virtual std::string get_stacktrace() const = 0;
        enum api {
            API_none,
            API_setTimeout,
            API_setInterval,
            API_clearTimeout,
            API_clearInterval,
        };
        virtual int call_api(int, std::weak_ptr<js_value> &,
                             std::vector<std::weak_ptr<js_value>> &, uint32_t) = 0;
        virtual int call_api(const std::shared_ptr<jsv_function> &, std::weak_ptr<js_value> &,
                             std::vector<std::weak_ptr<js_value>> &, uint32_t) = 0;
        virtual std::shared_ptr<js_value> fast_api(const std::shared_ptr<jsv_function> &, std::weak_ptr<js_value> &,
                                                   std::vector<std::weak_ptr<js_value>> &, uint32_t) = 0;
    };

    class js_value : public std::enable_shared_from_this<js_value> {
    public:
        enum attr_t {
            at_const = 1U << 0U,
            at_readonly = 1U << 1U,
            at_refs = 1U << 2U,
        };
        enum primitive_t {
            conv_default,
            conv_number,
            conv_string,
        };

        using ref = std::shared_ptr<js_value>;
        using weak_ref = std::weak_ptr<js_value>;
        virtual runtime_t get_type() = 0;
        virtual js_value::ref unary_op(js_value_new &n, int code) = 0;
        virtual bool to_bool() const = 0;
        virtual void mark(int n) = 0;
        virtual bool is_primitive() const;
        virtual js_value::ref to_primitive(js_value_new &n, primitive_t);
        virtual std::string to_string(js_value_new *n, int hint) const = 0;
        virtual double to_number(js_value_new *n) const = 0;
        uint8_t marked{0};
        uint8_t attr{0};
        uint8_t reserved1{0};
        uint8_t reserved2{0};
        weak_ref __proto__;
    };

    class jsv_number : public js_value {
    public:
        using ref = std::shared_ptr<jsv_number>;
        using weak_ref = std::weak_ptr<jsv_number>;
        explicit jsv_number(double n);
        runtime_t get_type() override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
        static std::string number_to_string(double d);
        double number;
    };

    class jsv_string : public js_value {
    public:
        using ref = std::shared_ptr<jsv_string>;
        using weak_ref = std::weak_ptr<jsv_string>;
        explicit jsv_string(std::string s);
        runtime_t get_type() override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
        int to_number(double &d);
        static int to_number(const std::string &s, double &d);
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
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
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
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        js_value::ref get(const std::string &name) const;
        bool is_primitive() const override;
        js_value::ref to_primitive(js_value_new &n, primitive_t) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
        ref clear();
        std::unordered_map<std::string, js_value::weak_ref> obj;
        std::unordered_map<std::string, js_value::weak_ref> special;
    };

    class jsv_null : public js_value {
    public:
        static std::string _str;
        using ref = std::shared_ptr<jsv_null>;
        using weak_ref = std::weak_ptr<jsv_null>;
        runtime_t get_type() override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
    };

    class jsv_undefined : public js_value {
    public:
        static std::string _str;
        using ref = std::shared_ptr<jsv_undefined>;
        using weak_ref = std::weak_ptr<jsv_undefined>;
        runtime_t get_type() override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
    };

    class cjs_function_info;

    class cjs_function;

    class jsv_function : public jsv_object {
    public:
        enum attr_t {
            at_new_function = 1U << 1U,
        };
        using ref = std::shared_ptr<jsv_function>;
        using weak_ref = std::weak_ptr<jsv_function>;
        jsv_function() = default;
        explicit jsv_function(const sym_code_t::ref &c, js_value_new &n);
        runtime_t get_type() override;
        js_value::ref unary_op(js_value_new &n, int code) override;
        bool to_bool() const override;
        void mark(int n) override;
        std::string to_string(js_value_new *n, int hint) const override;
        double to_number(js_value_new *n) const override;
        ref clear2();
        std::shared_ptr<cjs_function_info> code;
        std::function<int(std::shared_ptr<cjs_function> &, js_value::weak_ref &_this, std::vector<js_value::weak_ref> &, js_value_new &, uint32_t attr)> builtin;
        jsv_object::weak_ref closure;
        std::string name;
    };

    class cjs_function_info : public std::enable_shared_from_this<cjs_function_info> {
    public:
        using ref = std::shared_ptr<cjs_function_info>;
        using weak_ref = std::weak_ptr<cjs_function_info>;
        explicit cjs_function_info(const sym_code_t::ref &code, js_value_new &n);
        static js_value::ref load_const(const cjs_consts &c, int op, js_value_new &n);
        bool arrow{false};
        std::string debugName;
        std::string simpleName;
        std::string fullName;
        std::string text;
        int args_num{0};
        bool rest{false};
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
        explicit cjs_function(const sym_code_t::ref &code, js_value_new &n);
        explicit cjs_function(cjs_function_info::ref code);
        void reset(const sym_code_t::ref &code, js_value_new &n);
        void reset(cjs_function_info::ref code);
        void clear();
        void store_name(const std::string &name, js_value::weak_ref obj);
        void store_fast(const std::string &name, js_value::weak_ref obj);
        void store_deref(const std::string &name, js_value::weak_ref obj);
        cjs_function_info::ref info;
        std::string name{"UNKNOWN"};
        int pc{0};
        std::vector<js_value::weak_ref> stack;
        js_value::weak_ref ret_value;
        js_value::weak_ref _this;
        jsv_object::weak_ref envs;
        jsv_object::weak_ref closure;
        std::vector<int> rests;
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
        cjsruntime() = default;
        ~cjsruntime() = default;

        cjsruntime(const cjsruntime &) = delete;
        cjsruntime &operator=(const cjsruntime &) = delete;

        void init(void *);

        void eval(cjs_code_result::ref code, bool top);
        void set_readonly(bool);

        jsv_number::ref new_number(double n) override;
        jsv_string::ref new_string(const std::string &s) override;
        jsv_boolean::ref new_boolean(bool b) override;
        jsv_object::ref new_object() override;
        jsv_object::ref new_object_box(const js_value::ref &) override;
        jsv_function::ref new_function() override;
        jsv_null::ref new_null() override;
        jsv_undefined::ref new_undefined() override;
        cjs_function::ref new_func(const cjs_function_info::ref &code) override;
        jsv_object::ref new_array() override;
        jsv_object::ref new_error(int) override;
        void exec(const std::string &, const std::string &) override;
        std::string get_stacktrace() const override;
        int call_api(int type, js_value::weak_ref &_this,
                     std::vector<js_value::weak_ref> &args, uint32_t attr) override;
        int call_api(const jsv_function::ref &, js_value::weak_ref &_this, std::vector<js_value::weak_ref> &, uint32_t attr) override;
        js_value::ref fast_api(const jsv_function::ref &, js_value::weak_ref &_this,
                               std::vector<js_value::weak_ref> &args, uint32_t attr) override;

        static bool to_number(const js_value::ref &, double &);
        static std::vector<js_value::weak_ref> to_array(const js_value::ref &);

    private:
        int run(const cjs_code &code);
        js_value::ref load_const(int op);
        js_value::ref load_fast(int op);
        js_value::ref load_name(int op);
        js_value::ref load_global(int op);
        bool remove_global(int op);
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

        jsv_number::ref _new_number(double n, uint32_t attr = 0U);
        jsv_string::ref _new_string(const std::string &s, uint32_t attr = 0U);
        jsv_boolean::ref _new_boolean(bool b, uint32_t attr = 0U);
        jsv_object::ref _new_object(uint32_t attr = 0U);
        jsv_function::ref _new_function(jsv_object::ref proto, uint32_t attr = 0U);
        jsv_null::ref _new_null(uint32_t attr = 0U);
        jsv_undefined::ref _new_undefined(uint32_t attr = 0U);

        static void print(const js_value::ref &value, int level, std::ostream &os);

        js_value::ref binop(int code, js_value::ref op1, js_value::ref op2);

        void eval_timeout();

        double api_setTimeout(int time, const jsv_function::ref &func, std::vector<js_value::weak_ref> args, uint32_t attr, bool once);
        void api_clearTimeout(double id);

    private:
        void *pjs{nullptr};
        bool readonly{true};
        std::vector<cjs_function::ref> stack;
        cjs_function::ref current_stack;
        std::vector<cjs_function::ref> reuse_stack;
        std::list<js_value::ref> objs;
        struct _permanents_t {
            // refs
            std::vector<js_value::ref> refs;
            // cached objects
            jsv_object::ref global_env;
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
            // debug function
            jsv_function::ref _debug_print;
            jsv_function::ref _debug_dump;
            // global function
            jsv_function::ref global_setTimeout;
            jsv_function::ref global_setInterval;
            jsv_function::ref global_clearTimeout;
            jsv_function::ref global_clearInterval;
            // proto
            jsv_object::ref _proto_boolean;
            jsv_object::ref _proto_function;
            jsv_function::ref _proto_function_call;
            jsv_function::ref _proto_function_apply;
            jsv_object::ref _proto_number;
            jsv_object::ref _proto_object;
            jsv_function::ref _proto_object_hasOwnProperty;
            jsv_function::ref _proto_object_toString;
            jsv_object::ref _proto_string;
            jsv_object::ref _proto_root;
            // console
            jsv_object::ref console;
            jsv_function::ref console_log;
            jsv_function::ref console_trace;
            // sys
            jsv_object::ref sys;
            jsv_function::ref sys_exec_file;
            // function
            jsv_function::ref f_number;
            jsv_function::ref f_boolean;
            jsv_function::ref f_object;
            jsv_function::ref f_string;
            jsv_function::ref f_function;
            // array
            jsv_object::ref _proto_array;
            jsv_function::ref f_array;
            // error
            jsv_object::ref _proto_error;
            jsv_function::ref f_error;
        } permanents;
        cjs_runtime_reuse reuse;
        struct timeout_t {
            bool once{true};
            int time{0};
            uint32_t id{0};
            jsv_function::ref func;
            std::vector<js_value::weak_ref> args;
            uint32_t attr;
        };
        struct timeout_struct {
            std::time_t startup_time{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
            uint32_t global_id{0};
            std::map<std::time_t, std::list<std::shared_ptr<timeout_t>>> queues;
            std::unordered_map<uint32_t, std::shared_ptr<timeout_t>> ids;
        } timeout;
    };
}

#endif //CLIBJS_CJSRUNTIME_H
