//
// Project: clibjs
// Created by bajdcc
//

#include "cjsgen.h"
#include "cjsast.h"

namespace clib {

    cjsgen::cjsgen() {
        tmp.emplace_back();
        ast.emplace_back();
    }

    bool cjsgen::gen_code(ast_node *node) {
        gen_rec(node, 0);
        return false;
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
                auto children = gen_get_children(node->child);
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
        if (!tmp.back().empty()) {
            //assert(tmp.back().size() == 1);
            auto &top = tmp[tmp.size() - 2];
            for (auto &t : tmp.back()) {
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
    }
}
