//
// Project: clibjs
// Created by bajdcc
//

#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cassert>
#include "cjsgen.h"
#include "cjsast.h"

#define DUMP_CODE 1
#define PRINT_AST 1

#define AST_IS_KEYWORD(node) ((node)->flag == a_keyword)
#define AST_IS_KEYWORD_K(node, k) ((node)->data._keyword == (k))
#define AST_IS_KEYWORD_N(node, k) (AST_IS_KEYWORD(node) && AST_IS_KEYWORD_K(node, k))
#define AST_IS_OP(node) ((node)->flag == a_operator)
#define AST_IS_OP_K(node, k) ((node)->data._op == (k))
#define AST_IS_OP_N(node, k) (AST_IS_OP(node) && AST_IS_OP_K(node, k))
#define AST_IS_ID(node) ((node)->flag == ast_literal)
#define AST_IS_COLL(node) ((node)->flag == a_collection)
#define AST_IS_COLL_K(node, k) ((node)->data._coll == (k))
#define AST_IS_COLL_N(node, k) (AST_IS_COLL(node) && AST_IS_COLL_K(node, k))

namespace clib {

    cjsgen::cjsgen() {
        tmp.emplace_back();
        ast.emplace_back();
        codes.push_back(std::make_shared<sym_code_t>());
    }

    void copy_info(sym_t::ref dst, ast_node *src) {
        dst->line = src->line;
        dst->column = src->column;
        dst->start = src->start;
        dst->end = src->end;
    }

    void copy_info(sym_t::ref dst, sym_t::ref src) {
        dst->line = src->line;
        dst->column = src->column;
        dst->start = src->start;
        dst->end = src->end;
    }

    int cjs_consts::get_number(double n) {
        auto f = numbers.find(n);
        if (f == numbers.end()) {
            auto idx = index++;
            numbers.insert({n, idx});
            return idx;
        }
        return f->second;
    }

    int cjs_consts::get_string(const std::string &str, get_string_t type) {
        if (type == gs_name) {
            auto f = names.find(str);
            if (f == names.end()) {
                auto idx = (int) names.size();
                names.insert({str, idx});
                return idx;
            }
            return f->second;
        }
        if (type == gs_string) {
            auto f = strings.find(str);
            if (f == strings.end()) {
                auto idx = index++;
                strings.insert({str, idx});
                return idx;
            }
            return f->second;
        }
        if (type == gs_global) {
            auto f = globals.find(str);
            if (f == globals.end()) {
                auto idx = (int) globals.size();
                globals.insert({str, idx});
                return idx;
            }
            return f->second;
        }
        if (type == gs_deref) {
            auto f = derefs.find(str);
            if (f == derefs.end()) {
                auto idx = (int) derefs.size();
                derefs.insert({str, idx});
                return idx;
            }
            return f->second;
        }
        if (type == gs_regex) {
            auto f = regexes.find(str);
            if (f == regexes.end()) {
                auto idx = index++;
                regexes.insert({str, idx});
                return idx;
            }
            return f->second;
        }
        assert(!"invalid type");
        return -1;
    }

    int cjs_consts::get_function(std::shared_ptr<sym_code_t> code) {
        auto idx = index++;
        functions.insert({idx, code});
        return idx;
    }

    std::string cjs_consts::get_desc(int n) const {
        auto f = functions.find(n);
        if (f != functions.end()) {
            auto func = f->second.lock();
            return func->name ? func->name->data._identifier : LAMBDA_ID;
        }
        for (const auto &x : strings) {
            if (x.second == n)
                return x.first;
        }
        for (const auto &x : numbers) {
            if (x.second == n) {
                std::stringstream ss;
                ss << x.first;
                return ss.str();
            }
        }
        for (const auto &x : regexes) {
            if (x.second == n)
                return x.first;
        }
        return "";
    }

    runtime_t cjs_consts::get_type(int n) const {
        return consts.at(n);
    }

    char *cjs_consts::get_data(int n) const {
        return consts_data.at(n);
    }

    void cjs_consts::dump(const std::string *text) const {
        auto i = 0;
        std::vector<const char *> linksa(names.size());
        for (const auto &x : names) {
            linksa[x.second] = x.first.c_str();
        }
        for (const auto &x : linksa) {
            fprintf(stdout, "C [#%03d] [NAME  ] %s\n", i++, x);
        }
        i = 0;
        std::vector<std::tuple<int, void *>> links(index);
        for (const auto &x : strings) {
            links[x.second] = {0, (void *) &x.first};
        }
        for (const auto &x : numbers) {
            links[x.second] = {1, (void *) &x.first};
        }
        for (const auto &x : functions) {
            auto f = x.second.lock();
            auto desc = f->name ? f->name->data._identifier : "[lambda]";
            links[x.first] = {2, (void *) desc};
        }
        for (const auto &x : regexes) {
            links[x.second] = {3, (void *) &x.first};
        }
        for (const auto &x : links) {
            switch (std::get<0>(x)) {
                case 0:
                    fprintf(stdout, "C [#%03d] [STRING] %s\n", i, ((std::string *) std::get<1>(x))->c_str());
                    break;
                case 1:
                    fprintf(stdout, "C [#%03d] [NUMBER] %lf\n", i, *(double *) std::get<1>(x));
                    break;
                case 2: {
                    auto f = functions.at(i).lock();
                    fprintf(stdout, "C [#%03d] [FUNC  ] %s | %s\n", i, (const char *) std::get<1>(x),
                            text->substr(f->start, f->end - f->start).c_str());
                }
                    break;
                case 3:
                    fprintf(stdout, "C [#%03d] [REGEX ] %s\n", i, ((std::string *) std::get<1>(x))->c_str());
                    break;
                default:
                    break;
            }
            i++;
        }
    }

    void cjs_consts::save() {
        consts.resize(index);
        std::fill(consts.begin(), consts.end(), r__end);
        consts_data.resize(index);
        std::fill(consts_data.begin(), consts_data.end(), nullptr);
        for (const auto &x : strings) {
            consts[x.second] = r_string;
            consts_data[x.second] = (char *) &x.first;
        }
        for (const auto &x : numbers) {
            consts[x.second] = r_number;
            consts_data[x.second] = (char *) &x.first;
        }
        for (const auto &x : functions) {
            consts[x.first] = r_function;
        }
        for (const auto &x : regexes) {
            consts[x.second] = r_regex;
            consts_data[x.second] = (char *) &x.first;
        }
    }

    bool cjsgen::gen_code(ast_node *node, const std::string *str) {
        text = str;
        gen_rec(node, 0);
        if (tmp.front().empty())
            return false;
        tmp.front().front()->set_parent(nullptr);
        tmp.front().front()->gen_rvalue(*this);
#if PRINT_AST
        print(tmp.front().front(), 0, std::cout);
#endif
#if DUMP_CODE
        dump();
#endif
        codes.front()->consts.save();
        for (const auto &c : funcs) {
            c->consts.save();
        }
        return true;
    }

    sym_code_t::ref cjsgen::get_code() const {
        return codes.empty() ? nullptr : codes.front();
    }

    template<class T>
    static void gen_recursion(ast_node *node, int level, T f) {
        if (node == nullptr)
            return;
        auto i = node;
        if (i->next == i) {
            f(i, level);
            return;
        }
        f(i, level);
        i = i->next;
        while (i != node) {
            f(i, level);
            i = i->next;
        }
    }

    template<class T>
    static std::vector<T *> gen_get_children(T *node) {
        std::vector<T *> v;
        if (node == nullptr)
            return v;
        auto i = node;
        if (i->next == i) {
            v.push_back(i);
            return v;
        }
        v.push_back(i);
        i = i->next;
        while (i != node) {
            v.push_back(i);
            i = i->next;
        }
        return v;
    }

    template<class T>
    static std::vector<T *> gen_get_children_reverse(T *node) {
        std::vector<T *> v;
        if (node == nullptr)
            return v;
        node = node->prev;
        auto i = node;
        if (i->next == i) {
            v.push_back(i);
            return v;
        }
        v.push_back(i);
        i = i->prev;
        while (i != node) {
            v.push_back(i);
            i = i->prev;
        }
        return v;
    }

    void cjsgen::gen_rec(ast_node *node, int level) {
        if (node == nullptr)
            return;
        auto rec = [this](auto n, auto l) { this->gen_rec(n, l); };
        auto type = (ast_t) node->flag;
        if (type == a_collection) {
            if ((node->attr & a_exp) && node->child == node->child->next) {
                gen_recursion(node->child, level, rec);
                return;
            }
        }
        tmp.emplace_back();
        ast.emplace_back();
        switch (type) {
            case a_root: {
                gen_recursion(node->child, level, rec);
            }
                break;
            case a_collection: {
                auto children = (node->attr & ((uint16_t) a_reverse)) ?
                                gen_get_children_reverse(node->child) :
                                gen_get_children(node->child);
                gen_coll(children, level + 1, node);
            }
                break;
            case a_keyword:
            case a_operator:
            case a_literal:
            case a_string:
            case a_number:
            case a_regex:
                ast.back().push_back(node);
                break;
            default:
                break;
        }
        auto &tmps = tmp.back();
        if (!tmps.empty()) {
            //assert(tmps.size() == 1);
            auto &top = tmp[tmp.size() - 2];
            for (auto &t : tmps) {
                top.push_back(t);
            }
        }
        tmp.pop_back();
        if (!ast.back().empty()) {
            auto &top = ast[ast.size() - 2];
            std::copy(ast.back().begin(), ast.back().end(), std::back_inserter(top));
        }
        ast.pop_back();
    }

    void cjsgen::gen_coll(const std::vector<ast_node *> &nodes, int level, ast_node *node) {
        if (!gen_before(nodes, level, node)) {
            return;
        }
        for (auto &n : nodes) {
            gen_rec(n, level);
        }
        gen_after(nodes, level, node);
    }

    bool cjsgen::gen_before(const std::vector<ast_node *> &nodes, int level, ast_node *node) {
        switch (node->data._coll) {
            case c_program:
                break;
            case c_sourceElement:
                break;
            case c_statement:
                break;
            case c_block:
                break;
            case c_statementList:
                break;
            case c_variableStatement:
                break;
            case c_variableDeclarationList:
                break;
            case c_variableDeclaration:
                break;
            case c_emptyStatement:
                break;
            case c_expressionStatement:
                break;
            case c_ifStatement:
                break;
            case c_iterationStatement:
                break;
            case c_doStatement:
                break;
            case c_whileStatement:
                break;
            case c_forStatement:
                break;
            case c_forInStatement:
                break;
            case c_continueStatement:
                break;
            case c_breakStatement:
                break;
            case c_returnStatement:
                break;
            case c_withStatement:
                break;
            case c_switchStatement:
                break;
            case c_caseBlock:
                break;
            case c_caseClauses:
                break;
            case c_caseClause:
                break;
            case c_defaultClause:
                break;
            case c_labelledStatement:
                break;
            case c_throwStatement:
                break;
            case c_tryStatement:
                break;
            case c_catchProduction:
                break;
            case c_finallyProduction:
                break;
            case c_debuggerStatement:
                break;
            case c_functionStatement:
                break;
            case c_functionDeclaration:
                break;
            case c_classDeclaration:
                break;
            case c_classTail:
                break;
            case c_classElement:
                break;
            case c_classElements:
                break;
            case c_methodDefinition:
                break;
            case c_formalParameterList:
                break;
            case c_formalParameterArg:
                break;
            case c_functionBody:
                break;
            case c_sourceElements:
                break;
            case c_arrayLiteral:
                break;
            case c_elementList:
                break;
            case c_arrayElement:
                break;
            case c_objectLiteral:
                break;
            case c_propertyAssignment:
                break;
            case c_propertyAssignments:
                break;
            case c_propertyName:
                break;
            case c_arguments:
                break;
            case c_argument:
                break;
            case c_expressionSequence:
                break;
            case c_singleExpression:
                break;
            case c_assignable:
                break;
            case c_anonymousFunction:
                break;
            case c_arrowFunctionParameters:
                break;
            case c_arrowFunctionBody:
                break;
            case c_literal:
                break;
            case c_numericLiteral:
                break;
            case c_identifierName:
                break;
            case c_reservedWord:
                break;
            case c_keyword:
                break;
            case c_eos:
                break;
            case c_propertyExpressionAssignment:
                break;
            case c_computedPropertyExpressionAssignment:
                break;
            case c_propertyShorthand:
                break;
            case c_functionDecl:
                break;
            case c_anoymousFunctionDecl:
                break;
            case c_arrowFunction:
                break;
            case c_functionExpression:
                break;
            case c_classExpression:
                break;
            case c_memberIndexExpression:
                break;
            case c_memberDotExpression:
                break;
            case c_argumentsExpression:
                break;
            case c_newExpression:
                break;
            case c_primaryExpression: {
                if (AST_IS_COLL_K(nodes.front(), c_prefixExpression)) {
                    gen_rec(nodes[1], level); // gen exp first
                    nodes[0]->attr |= (uint16_t) a_reverse;
                    gen_rec(nodes[0], level); // then prefix
                    nodes[0]->attr &= ~(uint16_t) a_reverse;
                    if (nodes.size() > 2) {
                        gen_rec(nodes[2], level); // last postfix
                    }
                    gen_after(nodes, level, node);
                    return false;
                }
            }
                break;
            case c_prefixExpression:
                break;
            case c_postIncrementExpression:
                break;
            case c_postDecreaseExpression:
                break;
            case c_postfixExpression:
                break;
            case c_deleteExpression:
                break;
            case c_voidExpression:
                break;
            case c_typeofExpression:
                break;
            case c_preIncrementExpression:
                break;
            case c_preDecreaseExpression:
                break;
            case c_unaryPlusExpression:
                break;
            case c_unaryMinusExpression:
                break;
            case c_bitNotExpression:
                break;
            case c_notExpression:
                break;
            case c_powerExpression:
                break;
            case c_multiplicativeExpression:
                break;
            case c_additiveExpression:
                break;
            case c_coalesceExpression:
                break;
            case c_bitShiftExpression:
                break;
            case c_relationalExpression:
                break;
            case c_instanceofExpression:
                break;
            case c_inExpression:
                break;
            case c_equalityExpression:
                break;
            case c_bitAndExpression:
                break;
            case c_bitXOrExpression:
                break;
            case c_bitOrExpression:
                break;
            case c_logicalAndExpression:
                break;
            case c_logicalOrExpression:
                break;
            case c_ternaryExpression:
                break;
            case c_assignmentExpression:
                break;
            case c_assignmentOperatorExpression:
                break;
            case c_thisExpression:
                break;
            case c_identifierExpression:
                break;
            case c_superExpression:
                break;
            case c_literalExpression:
                break;
            case c_arrayLiteralExpression:
                break;
            case c_objectLiteralExpression:
                break;
            case c_parenthesizedExpression:
                break;
            default:
                break;
        }
        return true;
    }

    void cjsgen::gen_after(const std::vector<ast_node *> &nodes, int level, ast_node *node) {
        auto &asts = ast.back();
        auto &tmps = tmp.back();
        switch (node->data._coll) {
            case c_program:
                break;
            case c_sourceElement:
                break;
            case c_statement:
                break;
            case c_block: {
                auto block = std::make_shared<sym_block_t>();
                copy_info(block, tmps.front());
                for (const auto &s : tmps) {
                    assert(s->get_base_type() == s_statement);
                    block->stmts.push_back(std::dynamic_pointer_cast<sym_stmt_t>(s));
                    block->end = s->end;
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(block);
            }
                break;
            case c_statementList:
                break;
            case c_variableStatement:
                break;
            case c_variableDeclarationList: {
                auto stmt = std::make_shared<sym_stmt_var_t>();
                copy_info(stmt, tmps.front());
                for (const auto &s : tmps) {
                    assert(s->get_type() == s_id);
                    stmt->vars.push_back(std::dynamic_pointer_cast<sym_id_t>(s));
                    stmt->end = s->end;
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(stmt);
            }
                break;
            case c_variableDeclaration: {
                auto r = std::make_shared<sym_var_t>(asts.front());
                copy_info(r, asts.front());
                auto id = std::make_shared<sym_id_t>();
                id->ids.push_back(r);
                copy_info(id, r);
                if (!tmps.empty()) {
                    assert(tmps.front()->get_base_type() == s_expression);
                    id->init = std::dynamic_pointer_cast<sym_exp_t>(tmps.front());
                    copy_info(id->init, tmps.front());
                    id->end = id->init->end;
                    id->parse();
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(id);
            }
                break;
            case c_emptyStatement:
                break;
            case c_expressionStatement: {
                if (tmps.front()->get_type() == s_expression_seq) {
                    auto stmt = std::make_shared<sym_stmt_exp_t>();
                    copy_info(stmt, tmps.front());
                    stmt->seq = std::dynamic_pointer_cast<sym_exp_seq_t>(tmps.front());
                    asts.clear();
                    tmps.clear();
                    tmps.push_back(stmt);
                } else if (tmps.front()->get_base_type() == s_expression) {
                    auto seq = std::make_shared<sym_exp_seq_t>();
                    copy_info(seq, tmps.front());
                    seq->exps.push_back(std::dynamic_pointer_cast<sym_exp_t>(tmps.front()));
                    auto stmt = std::make_shared<sym_stmt_exp_t>();
                    copy_info(stmt, tmps.front());
                    stmt->seq = seq;
                    asts.clear();
                    tmps.clear();
                    tmps.push_back(stmt);
                }
            }
                break;
            case c_ifStatement:
                break;
            case c_iterationStatement:
                break;
            case c_doStatement:
                break;
            case c_whileStatement:
                break;
            case c_forStatement:
                break;
            case c_forInStatement:
                break;
            case c_continueStatement:
                break;
            case c_breakStatement:
                break;
            case c_returnStatement: {
                auto stmt = std::make_shared<sym_stmt_return_t>();
                copy_info(stmt, asts.front());
                if (!tmps.empty()) {
                    sym_exp_seq_t::ref seq;
                    if (tmps.back()->get_type() == s_expression_seq) {
                        seq = std::dynamic_pointer_cast<sym_exp_seq_t>(tmps.back());
                    } else {
                        seq = std::make_shared<sym_exp_seq_t>();
                        copy_info(seq, tmps.front());
                        seq->exps.push_back(to_exp(tmps.front()));
                    }
                    stmt->seq = seq;
                    stmt->end = seq->end;
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(stmt);
            }
                break;
            case c_withStatement:
                break;
            case c_switchStatement:
                break;
            case c_caseBlock:
                break;
            case c_caseClauses:
                break;
            case c_caseClause:
                break;
            case c_defaultClause:
                break;
            case c_labelledStatement:
                break;
            case c_throwStatement:
                break;
            case c_tryStatement:
                break;
            case c_catchProduction:
                break;
            case c_finallyProduction:
                break;
            case c_debuggerStatement:
                break;
            case c_functionStatement: {
                auto stmt = std::make_shared<sym_stmt_exp_t>();
                auto seq = std::make_shared<sym_exp_seq_t>();
                stmt->seq = seq;
                seq->exps.push_back(to_exp(tmps.front()));
                copy_info(seq, tmps.front());
                copy_info(stmt, tmps.front());
                tmps.clear();
                tmps.push_back(stmt);
            }
                break;
            case c_functionDeclaration: {
                auto code = std::make_shared<sym_code_t>();
                copy_info(code, asts[0]);
                code->name = asts[1];
                code->end = asts.back()->end;
                asts.pop_back();
                std::vector<ast_node *> _asts(asts.begin() + 2, asts.end());
                code->args = _asts;
                code->body = tmps.front();
                asts.clear();
                tmps.clear();
                tmps.push_back(code);
            }
                break;
            case c_classDeclaration:
                break;
            case c_classTail:
                break;
            case c_classElement:
                break;
            case c_classElements:
                break;
            case c_methodDefinition:
                break;
            case c_formalParameterList:
                break;
            case c_formalParameterArg:
                break;
            case c_functionBody:
                break;
            case c_sourceElements: {
                auto block = std::make_shared<sym_block_t>();
                if (!tmps.empty()) {
                    copy_info(block, tmps.front());
                }
                for (const auto &s : tmps) {
                    assert(s->get_base_type() == s_statement);
                    block->stmts.push_back(std::dynamic_pointer_cast<sym_stmt_t>(s));
                    block->end = s->end;
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(block);
            }
                break;
            case c_arrayLiteral: {
                auto array = std::make_shared<sym_array_t>();
                copy_info(array, asts.front());
                array->end = asts.back()->end;
                for (const auto &s : tmps) {
                    assert(s->get_base_type() == s_expression);
                    array->exps.push_back(to_exp(s));
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(array);
            }
                break;
            case c_elementList:
                break;
            case c_arrayElement:
                break;
            case c_objectLiteral: {
                auto obj = std::make_shared<sym_object_t>();
                copy_info(obj, asts.front());
                obj->end = asts.back()->end;
                for (const auto &s : tmps) {
                    assert(s->get_type() == s_object_pair);
                    obj->pairs.push_back(std::dynamic_pointer_cast<sym_object_pair_t>(s));
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(obj);
            }
                break;
            case c_propertyAssignment:
                break;
            case c_propertyAssignments:
                break;
            case c_propertyName:
                break;
            case c_arguments:
                break;
            case c_argument:
                break;
            case c_expressionSequence: {
                if (tmps.size() > 1) {
                    auto seq = std::make_shared<sym_exp_seq_t>();
                    if (!tmps.empty()) {
                        copy_info(seq, tmps.front());
                    }
                    for (const auto &s : tmps) {
                        seq->exps.push_back(to_exp(s));
                        seq->end = s->end;
                    }
                    asts.clear();
                    tmps.clear();
                    tmps.push_back(seq);
                }
            }
                break;
            case c_singleExpression:
                break;
            case c_assignable:
                break;
            case c_anonymousFunction:
                break;
            case c_arrowFunctionParameters:
                break;
            case c_arrowFunctionBody:
                break;
            case c_numericLiteral:
                break;
            case c_identifierName:
                break;
            case c_reservedWord:
                break;
            case c_keyword:
                break;
            case c_eos:
                break;
            case c_propertyExpressionAssignment: {
                auto p = std::make_shared<sym_object_pair_t>();
                if (tmps.size() == 2) {
                    copy_info(p, tmps.front());
                    p->end = tmps.back()->end;
                    p->key = to_exp(tmps.front());
                    p->value = to_exp(tmps.back());
                } else {
                    if (tmps.front()->get_type() == s_var_id) {
                        auto id = std::dynamic_pointer_cast<sym_var_id_t>(tmps.front());
                        id->node->flag = a_string;
                        auto new_id = std::make_shared<sym_var_t>(id->node);
                        copy_info(new_id, tmps.front());
                        tmps.front() = new_id;
                    }
                    copy_info(p, asts.front());
                    p->end = tmps.back()->end;
                    p->key = to_exp(primary_node(asts.front()));
                    copy_info(p->key, asts.front());
                    p->value = to_exp(tmps.back());
                }
                asts.clear();
                tmps.clear();
                tmps.push_back(p);
            }
                break;
            case c_computedPropertyExpressionAssignment:
                break;
            case c_propertyShorthand:
                break;
            case c_functionDecl:
                break;
            case c_anoymousFunctionDecl: {
                auto code = std::make_shared<sym_code_t>();
                copy_info(code, asts[0]);
                code->end = asts.back()->end;
                asts.pop_back();
                std::vector<ast_node *> _asts(asts.begin() + 1, asts.end());
                code->args = _asts;
                code->body = tmps.front();
                asts.clear();
                tmps.clear();
                tmps.push_back(code);
            }
                break;
            case c_arrowFunction:
                break;
            case c_functionExpression:
                break;
            case c_classExpression:
                break;
            case c_memberIndexExpression: {
                auto exp = to_exp((tmp.rbegin() + 2)->front());
                if (exp->get_type() != s_member_index) {
                    auto t = std::make_shared<sym_member_index_t>(exp);
                    copy_info(t, exp);
                    tmps.front()->start = asts.front()->start;
                    tmps.front()->line = asts.front()->line;
                    tmps.front()->column = asts.front()->column;
                    tmps.front()->end = asts.back()->end;
                    t->indexes.push_back(to_exp(tmps.front()));
                    t->end = tmps.front()->end;
                    (tmp.rbegin() + 2)->back() = t;
                } else {
                    auto t = std::dynamic_pointer_cast<sym_member_index_t>(exp);
                    tmps.front()->start = asts.front()->start;
                    tmps.front()->line = asts.front()->line;
                    tmps.front()->column = asts.front()->column;
                    tmps.front()->end = asts.back()->end;
                    t->indexes.push_back(to_exp(tmps.front()));
                    t->end = tmps.front()->end;
                }
                tmps.clear();
                asts.clear();
            }
                break;
            case c_memberDotExpression: {
                auto exp = to_exp((tmp.rbegin() + 2)->front());
                if (exp->get_type() != s_member_dot) {
                    auto t = std::make_shared<sym_member_dot_t>(exp);
                    copy_info(t, exp);
                    t->dots.push_back(asts.front());
                    t->end = asts.front()->end;
                    (tmp.rbegin() + 2)->back() = t;
                } else {
                    auto t = std::dynamic_pointer_cast<sym_member_dot_t>(exp);
                    t->dots.push_back(asts.front());
                    t->end = asts.front()->end;
                }
                asts.clear();
            }
                break;
            case c_argumentsExpression: {
                auto exp = to_exp((tmp.rbegin() + 2)->front());
                if (exp->get_type() == s_member_dot) { // a.b(...)
                    auto old = std::dynamic_pointer_cast<sym_member_dot_t>(exp);
                    if (old->dots.size() > 1) { // a.b.c()
                        auto t = std::make_shared<sym_call_method_t>();
                        copy_info(t, exp);
                        t->method = old->dots.back();
                        old->dots.pop_back();
                        old->end = old->dots.back()->end;
                        t->obj = old;
                        t->end = asts.back()->end;
                        for (const auto &s : tmps)
                            t->args.push_back(to_exp(s));
                        (tmp.rbegin() + 2)->back() = t;
                    } else { // a.b()
                        auto t = std::make_shared<sym_call_method_t>();
                        copy_info(t, exp);
                        t->method = old->dots.back();
                        t->obj = old->exp;
                        t->end = asts.back()->end;
                        for (const auto &s : tmps)
                            t->args.push_back(to_exp(s));
                        (tmp.rbegin() + 2)->back() = t;
                    }
                } else { // a(...)
                    auto t = std::make_shared<sym_call_function_t>();
                    copy_info(t, exp);
                    t->obj = exp;
                    t->end = asts.back()->end;
                    for (const auto &s : tmps)
                        t->args.push_back(to_exp(s));
                    (tmp.rbegin() + 2)->back() = t;
                }
                asts.clear();
                tmps.clear();
            }
                break;
            case c_primaryExpression:
                break;
            case c_prefixExpression:
                break;
            case c_postfixExpression:
                break;
            case c_postIncrementExpression:
            case c_postDecreaseExpression: {
                auto exp = to_exp((tmp.rbegin() + 2)->front());
                auto t = std::make_shared<sym_sinop_t>(exp, asts.front());
                copy_info(t, exp);
                t->end = asts.front()->end;
                (tmp.rbegin() + 2)->back() = t;
                asts.clear();
            }
                break;
            case c_newExpression:
            case c_deleteExpression:
            case c_voidExpression:
            case c_typeofExpression:
            case c_preIncrementExpression:
            case c_preDecreaseExpression:
            case c_unaryPlusExpression:
            case c_unaryMinusExpression:
            case c_bitNotExpression:
            case c_notExpression: {
                auto &op = asts[0];
                auto &_exp = (tmp.rbegin() + 2)->front();
                auto exp = to_exp(_exp);
                auto unop = std::make_shared<sym_unop_t>(exp, op);
                copy_info(unop, exp);
                unop->start = op->start;
                unop->line = op->line;
                unop->column = op->column;
                (tmp.rbegin() + 2)->front() = unop;
                asts.clear();
            }
                break;
            case c_coalesceExpression:
                break;
            case c_instanceofExpression:
                break;
            case c_inExpression:
                break;
            case c_powerExpression:
            case c_multiplicativeExpression:
            case c_additiveExpression:
            case c_bitShiftExpression:
            case c_relationalExpression:
            case c_equalityExpression:
            case c_bitAndExpression:
            case c_bitXOrExpression:
            case c_bitOrExpression:
            case c_logicalAndExpression:
            case c_logicalOrExpression:
            case c_ternaryExpression: {
                size_t tmp_i = 0;
                auto exp1 = to_exp(tmps[tmp_i++]);
                auto exp2 = to_exp(tmps[tmp_i++]);
                for (size_t i = 0; i < asts.size(); ++i) {
                    auto &a = asts[i];
                    if (AST_IS_OP(a)) {
                        if (node->data._coll == c_ternaryExpression &&
                            AST_IS_OP_K(a, T_QUERY)) { // triop
                            auto exp3 = to_exp(tmps[tmp_i++]);
                            auto t = std::make_shared<sym_triop_t>(exp1, exp2, exp3, a, asts[i + 1]);
                            copy_info(t, exp1);
                            t->end = exp3->end;
                            exp1 = t;
                            if (tmp_i < tmps.size())
                                exp2 = to_exp(tmps[tmp_i++]);
                            i++;
                        } else { // binop
                            auto t = std::make_shared<sym_binop_t>(exp1, exp2, a);
                            copy_info(t, exp1);
                            t->end = exp2->end;
                            exp1 = t;
                            if (tmp_i < tmps.size())
                                exp2 = to_exp(tmps[tmp_i++]);
                        }
                    } else {
                        error(a, "invalid binop: coll");
                    }
                }
                tmps.clear();
                tmps.push_back(exp1);
                asts.clear();
            }
                break;
            case c_assignmentExpression:
            case c_assignmentOperatorExpression: {
                size_t tmp_i = 0;
                std::reverse(asts.begin(), asts.end());
                std::reverse(tmps.begin(), tmps.end());
                auto exp1 = to_exp(tmps[tmp_i++]);
                auto exp2 = to_exp(tmps[tmp_i++]);
                for (auto &a : asts) {
                    if (AST_IS_OP(a)) {
                        auto t = std::make_shared<sym_binop_t>(exp2, exp1, a);
                        copy_info(t, exp2);
                        t->end = exp1->end;
                        exp1 = t;
                        if (tmp_i < tmps.size())
                            exp2 = to_exp(tmps[tmp_i++]);
                    } else {
                        error(a, "invalid binop: coll");
                    }
                }
                tmps.clear();
                tmps.push_back(exp1);
                asts.clear();
            }
                break;
            case c_thisExpression:
                break;
            case c_literal:
            case c_literalExpression:
            case c_identifierExpression: {
                if (tmps.empty()) {
                    auto pri = primary_node(asts[0]);
                    copy_info(pri, asts[0]);
                    tmps.push_back(pri);
                    asts.clear();
                }
            }
                break;
            case c_superExpression:
                break;
            case c_arrayLiteralExpression:
                break;
            case c_objectLiteralExpression:
                break;
            case c_parenthesizedExpression: {
                auto exp = tmps.front();
                exp->start = asts.front()->start;
                exp->end = asts.back()->end;
                asts.clear();
            }
                break;
            default:
                break;
        }
    }

    void cjsgen::error(ast_node *node, const std::string &str, bool info) const {
        std::stringstream ss;
        ss << "[" << node->line << ":" << node->column << ":" << node->start << ":" << node->end << "] ";
        ss << str;
        if (info) {
            cjsast::print(node, 0, *text, ss);
        }
        throw cexception(ss.str());
    }

    void cjsgen::error(sym_t::ref s, const std::string &str) const {
        std::stringstream ss;
        ss << "[" << s->line << ":" << s->column << ":" << s->start << ":" << s->end << "] ";
        ss << str;
        throw cexception(ss.str());
    }

    sym_exp_t::ref cjsgen::to_exp(sym_t::ref s) {
        if (s->get_base_type() != s_expression)
            error(s, "need expression: " + s->to_string());
        return std::dynamic_pointer_cast<sym_exp_t>(s);
    }

    sym_t::ref cjsgen::find_symbol(ast_node *node) {
        return nullptr;
    }

    sym_var_t::ref cjsgen::primary_node(ast_node *node) {
        switch (node->flag) {
            case a_literal: {
                auto sym = find_symbol(node);
                return std::make_shared<sym_var_id_t>(node, sym);
            }
            case a_string:
            case a_regex:
            case a_number:
                return std::make_shared<sym_var_t>(node);
            case a_keyword: {
                if (AST_IS_KEYWORD_K(node, K_TRUE) || AST_IS_KEYWORD_K(node, K_FALSE))
                    return std::make_shared<sym_var_t>(node);
                else
                    error(node, "invalid var keyword type: ", true);
            }
                break;
            default:
                break;
        }
        return std::make_shared<sym_var_t>(node);
    }

    void cjsgen::print(const sym_t::ref &node, int level, std::ostream &os) {
        if (node == nullptr)
            return;
        auto type = node->get_type();
        os << std::setfill(' ') << std::setw(level) << "";
        switch (type) {
            case s_sym:
                break;
            case s_id:
                os << "id"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_id_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "id" << std::endl;
                    for (const auto &s : n->ids) {
                        print(s, level + 2, os);
                    }
                    if (n->init) {
                        os << std::setfill(' ') << std::setw(level + 1) << "";
                        os << "init" << std::endl;
                        print(n->init, level + 2, os);
                    }
                }
                break;
            case s_function:
                break;
            case s_var:
                os << "var"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_var_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << cjsast::to_string(n->node) << std::endl;
                }
                break;
            case s_var_id:
                os << "var_id"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_var_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    switch (n->clazz) {
                        case sym_var_t::local:
                            os << "[LOCAL] ";
                            break;
                        case sym_var_t::closure:
                            os << "[CLOSURE] ";
                            break;
                        case sym_var_t::global:
                            os << "[GLOBAL] ";
                            break;
                        default:
                            break;
                    }
                    os << n->node->data._string << std::endl;
                }
                break;
            case s_expression:
                break;
            case s_unop:
                os << "unop"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_unop_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "op: " << lexer_string(n->op->data._op)
                       << " " << "[" << n->op->line << ":"
                       << n->op->column << ":"
                       << n->op->start << ":"
                       << n->op->end << "]" << std::endl;
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp" << std::endl;
                    print(n->exp, level + 2, os);
                }
                break;
            case s_sinop:
                os << "sinop"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_sinop_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp" << std::endl;
                    print(n->exp, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "op: " << lexer_string(n->op->data._op)
                       << " " << "[" << n->op->line << ":"
                       << n->op->column << ":"
                       << n->op->start << ":"
                       << n->op->end << "]" << std::endl;
                }
                break;
            case s_binop:
                os << "binop"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_binop_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp1" << std::endl;
                    print(n->exp1, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "op: " << lexer_string(n->op->data._op)
                       << " " << "[" << n->op->line << ":"
                       << n->op->column << ":"
                       << n->op->start << ":"
                       << n->op->end << "]" << std::endl;
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp2" << std::endl;
                    print(n->exp2, level + 2, os);
                }
                break;
            case s_triop:
                os << "binop"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_triop_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp1" << std::endl;
                    print(n->exp1, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "op1: " << lexer_string(n->op1->data._op)
                       << " " << "[" << n->op1->line << ":"
                       << n->op1->column << ":"
                       << n->op1->start << ":"
                       << n->op1->end << "]" << std::endl;
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp2" << std::endl;
                    print(n->exp2, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "op2: " << lexer_string(n->op2->data._op)
                       << " " << "[" << n->op2->line << ":"
                       << n->op2->column << ":"
                       << n->op2->start << ":"
                       << n->op2->end << "]" << std::endl;
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "exp3" << std::endl;
                    print(n->exp3, level + 2, os);
                }
                break;
            case s_member_dot:
                os << "member_dot"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_member_dot_t>(node);
                    for (const auto &s : n->dots) {
                        os << std::setfill(' ') << std::setw(level + 1) << "";
                        os << "dot: " << s->data._identifier
                           << " " << "[" << s->line << ":"
                           << s->column << ":"
                           << s->start << ":"
                           << s->end << "]" << std::endl;
                    }
                }
                break;
            case s_expression_seq:
                os << "exp_seq"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_exp_seq_t>(node);
                    for (const auto &s : n->exps) {
                        print(s, level + 1, os);
                    }
                }
                break;
            case s_array:
                os << "array"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_array_t>(node);
                    for (const auto &s : n->exps) {
                        print(s, level + 1, os);
                    }
                }
                break;
            case s_object:
                os << "object"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_object_t>(node);
                    for (const auto &s : n->pairs) {
                        print(s, level + 1, os);
                    }
                }
                break;
            case s_object_pair:
                os << "pair"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_object_pair_t>(node);
                    print(n->key, level + 1, os);
                    print(n->value, level + 1, os);
                }
                break;
            case s_call_method:
                os << "call_method"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_call_method_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "obj" << std::endl;
                    print(n->obj, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "method: " << n->method->data._identifier
                       << " " << "[" << n->method->line << ":"
                       << n->method->column << ":"
                       << n->method->start << ":"
                       << n->method->end << "]" << std::endl;
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    if (!n->args.empty()) {
                        os << "args" << std::endl;
                        for (const auto &s : n->args) {
                            print(s, level + 2, os);
                        }
                    }
                }
                break;
            case s_call_function:
                os << "call_function"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_call_function_t>(node);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    os << "obj" << std::endl;
                    print(n->obj, level + 2, os);
                    os << std::setfill(' ') << std::setw(level + 1) << "";
                    if (!n->args.empty()) {
                        os << "args" << std::endl;
                        for (const auto &s : n->args) {
                            print(s, level + 2, os);
                        }
                    }
                }
                break;
            case s_ctrl:
                break;
            case s_statement:
                break;
            case s_statement_var:
                os << "statement_var"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_stmt_var_t>(node);
                    for (const auto &s : n->vars) {
                        print(s, level + 1, os);
                    }
                }
                break;
            case s_statement_exp:
                os << "statement_exp"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_stmt_exp_t>(node);
                    print(n->seq, level + 1, os);
                }
                break;
            case s_statement_return:
                os << "statement_return"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_stmt_return_t>(node);
                    if (n->seq)
                        print(n->seq, level + 1, os);
                }
                break;
            case s_block:
                os << "block"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                for (const auto &s : std::dynamic_pointer_cast<sym_block_t>(node)->stmts) {
                    print(s, level + 1, os);
                }
                break;
            case s_code:
                os << "code"
                   << " " << "[" << node->line << ":"
                   << node->column << ":"
                   << node->start << ":"
                   << node->end << "]" << std::endl;
                {
                    auto n = std::dynamic_pointer_cast<sym_code_t>(node);
                    if (n->name) {
                        os << std::setfill(' ') << std::setw(level + 1) << "";
                        os << "name: " << n->name->data._identifier
                           << " " << "[" << n->name->line << ":"
                           << n->name->column << ":"
                           << n->name->start << ":"
                           << n->name->end << "]" << std::endl;
                    }
                    if (!n->args.empty()) {
                        os << std::setfill(' ') << std::setw(level + 1) << "";
                        os << "args" << std::endl;
                        for (const auto &s : n->args) {
                            os << std::setfill(' ') << std::setw(level + 2) << "";
                            os << s->data._identifier
                               << " " << "[" << s->line << ":"
                               << s->column << ":"
                               << s->start << ":"
                               << s->end << "]" << std::endl;
                        }
                    }
                    print(n->body, level + 1, os);
                }
                break;
            default:
                break;
        }
    }

    void cjsgen::emit(ast_node_index *idx, ins_t i) {
        if (idx)
            codes.back()->codes.push_back({idx->line, idx->column, idx->start, idx->end, i, 0, 0, 0});
        else
            codes.back()->codes.push_back({0, 0, 0, 0, i, 0, 0, 0});
    }

    void cjsgen::emit(ast_node_index *idx, ins_t i, int a) {
        if (idx)
            codes.back()->codes.push_back({idx->line, idx->column, idx->start, idx->end, i, 1, a, 0});
        else
            codes.back()->codes.push_back({0, 0, 0, 0, i, 1, a, 0});
    }

    void cjsgen::emit(ast_node_index *idx, ins_t i, int a, int b) {
        if (idx)
            codes.back()->codes.push_back({idx->line, idx->column, idx->start, idx->end, i, 2, a, b});
        else
            codes.back()->codes.push_back({0, 0, 0, 0, i, 2, a, b});
    }

    int cjsgen::code_length() const {
        return (int) codes.back()->codes.size();
    }

    void cjsgen::edit(int code, int idx, int value) {
        switch (idx) {
            case 0:
                codes.back()->codes.at(code).code = value;
                break;
            case 1:
                codes.back()->codes.at(code).op1 = value;
                break;
            case 2:
                codes.back()->codes.at(code).op2 = value;
                break;
            default:
                break;
        }
    }

    int cjsgen::load_number(double d) {
        return codes.back()->consts.get_number(d);
    }

    int cjsgen::load_string(const std::string &s, int type) {
        return codes.back()->consts.get_string(s, (cjs_consts::get_string_t) (type));
    }

    int cjsgen::push_function(std::shared_ptr<sym_code_t> code) {
        funcs.push_back(code);
        auto id = codes.back()->consts.get_function(code);
        codes.push_back(std::move(code));
        return id;
    }

    void cjsgen::pop_function() {
        codes.pop_back();
    }

    void cjsgen::enter(int type) {
        codes.back()->scopes.emplace_back();
        codes.back()->scopes.back().type = (cjs_scope_t) type;
    }

    void cjsgen::leave() {
        codes.back()->scopes.pop_back();
    }

    std::shared_ptr<sym_t> cjsgen::get_var(const std::string &name, int t) {
        auto type = (cjs_scope_query_t) t;
        switch (type) {
            case sq_local: {
                const auto &vars = codes.back()->scopes.back().vars;
                auto f = vars.find(name);
                if (f != vars.end()) {
                    return f->second.lock();
                }
                return nullptr;
            }
            case sq_local_func: {
                const auto &scope = codes.back()->scopes;
                for (auto s = scope.rbegin(); s != scope.rend(); s++) {
                    const auto &vars = s->vars;
                    auto f = vars.find(name);
                    if (f != vars.end()) {
                        return f->second.lock();
                    }
                }
                return nullptr;
            }
            case sq_all: {
                for (auto i = codes.rbegin() + 1; i != codes.rend(); i++) {
                    const auto &scope = (*i)->scopes;
                    for (auto s = scope.rbegin(); s != scope.rend(); s++) {
                        const auto &vars = s->vars;
                        auto f = vars.find(name);
                        if (f != vars.end()) {
                            return f->second.lock();
                        }
                    }
                }
                return nullptr;
            }
            default:
                break;
        }
        return nullptr;
    }

    void cjsgen::add_var(const std::string &name, std::shared_ptr<sym_t> id) {
        codes.back()->scopes.back().vars.insert({name, id});
    }

    void cjsgen::add_closure(std::shared_ptr<sym_var_id_t> c) {
        codes.back()->closure.push_back(std::move(c));
    }

    int cjsgen::get_func_level() const {
        return (int) codes.size();
    }

    std::string cjsgen::get_fullname(const std::string &name) const {
        if (codes.size() == 1)
            return name;
        std::stringstream ss;
        for (auto i = codes.begin() + 1; i != codes.end(); i++) {
            if ((*i)->name)
                ss << (*i)->name->data._identifier;
            else
                ss << LAMBDA_ID;
            ss << ".<locals>.";
        }
        ss << name;
        return ss.str();
    }

    void cjsgen::error(ast_node_index *idx, const std::string &str) const {
        std::stringstream ss;
        ss << "[" << idx->line << ":" << idx->column << "] ";
        ss << str;
        ss << " (" << text->substr(idx->start, idx->end - idx->start) << ")";
        throw cexception(ss.str());
    }

    void cjsgen::dump() const {
        fprintf(stdout, "--== Main Function ==--\n");
        dump(codes.front());
        for (const auto &c : funcs) {
            fprintf(stdout, "--== Function: \"%s\" ==--\n", c->fullname.c_str());
            dump(c);
        }
    }

    void cjsgen::dump(sym_code_t::ref code) const {
        code->consts.dump(text);
        auto idx = 0;
        std::vector<int> jumps;
        {
            std::set<int, std::greater<>> jumps_set;
            for (const auto &c : code->codes) {
                switch (c.code) {
                    case JUMP_IF_TRUE_OR_POP:
                    case JUMP_IF_FALSE_OR_POP:
                    case POP_JUMP_IF_TRUE:
                    case POP_JUMP_IF_FALSE:
                    case JUMP_ABSOLUTE:
                        jumps_set.insert(c.op1);
                        break;
                    case JUMP_FORWARD:
                        jumps_set.insert(idx + c.op1 + 2);
                        break;
                    default:
                        break;
                }
                idx++;
            }
            std::copy(jumps_set.begin(), jumps_set.end(), std::back_inserter(jumps));
        }
        idx = 0;
        char buf[256];
        for (auto &c : code->codes) {
            auto alt = text->substr(c.start, c.end - c.start);
            auto jmp = "  ";
            if (!jumps.empty() && jumps.back() == idx) {
                jmp = ">>";
                jumps.pop_back();
            }
            if (c.opnum == 0)
                snprintf(buf, sizeof(buf), "[%04d:%03d]  %s   %4d %-20s                   (%.100s)",
                         c.line, c.column, jmp, idx, ins_string(ins_t(c.code)),
                         c.line == 0 ? "..." : text->substr(c.start, c.end - c.start).c_str());
            else if (c.opnum == 1) {
                if (c.code == LOAD_CONST && c.line == 0) {
                    snprintf(buf, sizeof(buf), "[%04d:%03d]  %s   %4d %-20s %8d          (%.100s)",
                             c.line, c.column, jmp, idx, ins_string(ins_t(c.code)), c.op1,
                             code->consts.get_desc(c.op1).c_str());
                } else {
                    snprintf(buf, sizeof(buf), "[%04d:%03d]  %s   %4d %-20s %8d          (%.100s)",
                             c.line, c.column, jmp, idx, ins_string(ins_t(c.code)), c.op1,
                             c.line == 0 ? "..." : text->substr(c.start, c.end - c.start).c_str());
                }
            } else if (c.opnum == 2)
                snprintf(buf, sizeof(buf), "[%04d:%03d]  %s   %4d %-20s %8d %8d (%.100s)",
                         c.line, c.column, jmp, idx, ins_string(ins_t(c.code)), c.op1, c.op2,
                         c.line == 0 ? "..." : text->substr(c.start, c.end - c.start).c_str());
            fprintf(stdout, "C %s\n", buf);
            c.desc = buf;
            idx++;
        }
    }
}
