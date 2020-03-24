# CLIBJS

JS-like script engine implemented by C++.

## Features

- [x] Parsing `jquery.js` and `Vue.js` in 1s.
- [ ] Translate javascript file into **Python Bytecode** temporarily.

## Environment

- CLion, MinGW on Win7x64
- C++14

## TODO

- [x] Lexer\(scanned js files in `test` dir\)
- [x] Parser\(Use LALR1 Parser with backtrace, see [clibparser](https://github.com/bajdcc/clibparser), Grammar: antlr/grammars-v4/javascript\)
- [x] Syntax Tree\(**Auto-generation** by LALR1 Parser\)
- [ ] AST\(On progress\)
- [x] IL Design\(**Use [Python Bytecode](https://github.com/python/cpython/blob/master/Include/opcode.h)** temporarily\)
- [ ] Gen\(On progress\)
- [ ] GC
- [ ] Env
- [ ] Interface

## TEST

Scanned js files successfully, ~~but spent more time~~ **BECAUSE IT IS BACKTRACE LALR1 PARSER**, now HARDCORE FIXED.

- [x] test/jquery.js
- [x] test/jquery.min.js\(~~parsing `for-in` spent much time~~ now it's ok\)
- [x] test/vue.js
- [x] test/vue.min.js

**Can adjust grammar priority when facing shift-conflicts by setting cost value for sorting.**

See `clib::cjsparser::gen` function.

```cpp
unit.adjust(&functionExpression, &anonymousFunction, e_shift, -1);
unit.adjust(&iterationStatement, &forInStatement, e_shift, -1);
unit.adjust(&iterationStatement, &forStatement, e_shift, 0, (void *) &pred_for);
unit.adjust(&inExpression, &inExpression, e_left_recursion, 0, (void *) &pred_in);
```

**FIX: Add manual LL(n)**

You can read ALL of the lexer words to decide whether you need to ALLOW/DELAY/REMOVE the PDA transition edge.

```cpp
pda_coll_pred cjsparser::pred_for(const cjslexer *lexer, int idx) {
    auto end = lexer->get_unit_size();
    auto find_in = false;
    for (auto i = idx + 1; i < end; i++) {
        const auto &U = lexer->get_unit(i);
        if (U.t == T_SEMI) {
            break;
        }
        if (U.t == K_IN) {
            find_in = true;
            break;
        }
    }
    return find_in ? p_DELAY : p_ALLOW;
}

pda_coll_pred cjsparser::pred_in(const cjslexer *lexer, int idx) {
    auto find_for = false;
    for (auto i = idx - 1; i >= 0; i--) {
        const auto &U = lexer->get_unit(i);
        if (U.t == T_LPARAN) {
            if (i > 0 && lexer->get_unit(i - 1).t == K_FOR) {
                find_for = true;
            }
            break;
        }
        if (U.t != ID) {
            break;
        }
    }
    return find_for ? p_REMOVE : p_ALLOW;
}
 ```
 
**FIX: Removing backtrace saved before**
 
 If runs to `var a = 1;`,it meets `;`, so removed backtrace array\(Indicating that AST checking before was correct\).

**FIX: Support no line terminator**
 
 Such as:
 ```cpp
 return
 a
 ```
 
 Grammar: `returnStatement = _K_RETURN + *(~_RULE_NO_LINE + expressionSequence) + *eos;`
 
 `RULE_NO_LINE` eats no lexer words but looks ahead for NO LINE WHITESPACE.

## AST and IL

Input: (**test/test_2.js**)

```javascript
var a = b = c = 1;
var d = a + (b * c);
var e = 1 && 2 > 3;
a + b, a - b;
1 + ++--a--++;
```

Output:

```
C [#000] [NAME  ] a
C [#001] [NAME  ] b
C [#002] [NAME  ] c
C [#003] [NAME  ] d
C [#004] [NAME  ] e
C [#000] [NUMBER] 1.000000
C [#001] [NUMBER] 2.000000
C [#002] [NUMBER] 3.000000
C [0001:017]          0 LOAD_CONST                  0          (1)
C [0001:005]          2 DUP_TOP                                (a)
C [0001:005]          3 STORE_NAME                  0          (a)
C [0001:009]          5 DUP_TOP                                (b)
C [0001:009]          6 STORE_NAME                  1          (b)
C [0001:013]          8 STORE_NAME                  2          (c)
C [0002:009]         10 LOAD_NAME                   0          (a)
C [0002:014]         12 LOAD_NAME                   1          (b)
C [0002:018]         14 LOAD_NAME                   2          (c)
C [0002:014]         16 BINARY_MULTIPLY                        ((b * c))
C [0002:009]         17 BINARY_ADD                             (a + (b * c))
C [0002:005]         18 STORE_NAME                  3          (d)
C [0003:009]         20 LOAD_CONST                  0          (1)
C [0003:009]         22 JUMP_IF_TRUE_OR_POP        30          (1 && 2 > 3)
C [0003:014]         24 LOAD_CONST                  1          (2)
C [0003:018]         26 LOAD_CONST                  2          (3)
C [0003:014]         28 COMPARE_OP                  4          (2 > 3)
C [0003:005]  >>     30 STORE_NAME                  4          (e)
C [0004:001]         32 LOAD_NAME                   0          (a)
C [0004:005]         34 LOAD_NAME                   1          (b)
C [0004:001]         36 BINARY_ADD                             (a + b)
C [0004:001]         37 POP_TOP                                (a + b)
C [0004:008]         38 LOAD_NAME                   0          (a)
C [0004:012]         40 LOAD_NAME                   1          (b)
C [0004:008]         42 BINARY_SUBTRACT                        (a - b)
C [0004:001]         43 POP_TOP                                (a + b, a - b)
C [0005:001]         44 LOAD_CONST                  0          (1)
C [0005:009]         46 LOAD_NAME                   0          (a)
C [0005:009]         48 BINARY_DEC                             (--a)
C [0005:009]         49 DUP_TOP                                (--a)
C [0005:009]         50 STORE_NAME                  0          (a)
C [0005:009]         52 DUP_TOP                                (--a--)
C [0005:009]         53 BINARY_DEC                             (--a--)
C [0005:009]         54 STORE_NAME                  0          (a)
C [0005:009]         56 DUP_TOP                                (--a--++)
C [0005:009]         57 BINARY_INC                             (--a--++)
C [0005:009]         58 STORE_NAME                  0          (a)
C [0005:001]         60 BINARY_ADD                             (1 + ++--a--++)
C [0005:001]         61 POP_TOP                                (1 + ++--a--++)
```

## Grammar

See [grammars-v4/javascript](https://github.com/antlr/grammars-v4/blob/master/javascript/javascript/JavaScriptParser.g4).

```cpp
program = sourceElements;
variableStatement = _K_VAR + variableDeclarationList + eos;
variableDeclarationList = *(variableDeclarationList + ~_T_COMMA) + variableDeclaration;
variableDeclaration = assignable + *(_T_ASSIGN + singleExpression);
emptyStatement = _T_SEMI;
expressionStatement = expressionSequence + eos;
ifStatement = _K_IF + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement + *(_K_ELSE + statement);
iterationStatement = doStatement | whileStatement | forStatement | forInStatement;
doStatement = _K_DO + statement + _K_WHILE + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + eos;
whileStatement = _K_WHILE + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement;
forStatement = _K_FOR + ~_T_LPARAN + *(expressionSequence | _K_VAR + variableDeclarationList) +
               _T_SEMI + *expressionSequence + _T_SEMI + *expressionSequence + ~_T_RPARAN + statement;
forInStatement = _K_FOR + ~_T_LPARAN + *(singleExpression | _K_VAR + variableDeclarationList) +
                 _K_IN + expressionSequence + ~_T_RPARAN + statement;
continueStatement = _K_CONTINUE + *(_RULE_NO_LINE + _ID) + eos;
breakStatement = _K_BREAK + *(_RULE_NO_LINE + _ID) + eos;
returnStatement = _K_RETURN + *(_RULE_NO_LINE + expressionSequence) + eos;
withStatement = _K_WITH + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement;
switchStatement = _K_SWITCH + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + caseBlock;
caseBlock = ~_T_LBRACE + *caseClauses + *(defaultClause + *caseClauses) + ~_T_RBRACE;
caseClauses = caseClause + *caseClauses;
caseClause = _K_CASE + expressionSequence + _T_COLON + *statementList;
defaultClause = _K_DEFAULT + _T_COLON + *statementList;
labelledStatement = _ID + _T_COLON + statement;
throwStatement = _K_THROW + _RULE_NO_LINE + expressionSequence + eos;
tryStatement = _K_TRY + block + (catchProduction + *finallyProduction | finallyProduction);
catchProduction = _K_CATCH + *(~_T_LPARAN + *assignable + ~_T_RPARAN) + block;
finallyProduction = _K_FINALLY + block;
debuggerStatement = _K_DEBUGGER + eos;
functionDeclaration = _K_FUNCTION + _ID + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                      ~_T_LBRACE + *functionBody + ~_T_RBRACE;
classDeclaration = _K_CLASS + _ID + classTail;
classTail = ~_T_LBRACE + classElements + ~_T_RBRACE;
classElements = *(classElements + ~_T_COMMA) + classElement;
classElement = methodDefinition
               | emptyStatement
               | *_T_SHARP + propertyName + _T_ASSIGN + singleExpression;
methodDefinition = *_T_SHARP + propertyName + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                   ~_T_LBRACE + *functionBody + ~_T_RBRACE;
formalParameterList = *(formalParameterList + ~_T_COMMA) + assignable;
formalParameterArg = assignable;
functionBody = *functionBody + sourceElements;
sourceElements = *sourceElements + statement;
sourceElement = statement;
statement = block
            | variableStatement
            | emptyStatement
            | classDeclaration
            | expressionStatement
            | ifStatement
            | iterationStatement
            | continueStatement
            | breakStatement
            | returnStatement
            | withStatement
            | labelledStatement
            | switchStatement
            | throwStatement
            | tryStatement
            | debuggerStatement
            | functionDeclaration;
block = ~_T_LBRACE + *statementList + ~_T_RBRACE;
statementList = *statementList + statement;
expressionStatement = expressionSequence + eos;
expressionSequence = *(expressionSequence + ~_T_COMMA) + singleExpression;
thisExpression = _K_THIS;
identifierExpression = _ID;
superExpression = _K_SUPER;
literalExpression = literal;
arrayLiteralExpression = arrayLiteral;
objectLiteralExpression = objectLiteral;
parenthesizedExpression = ~_T_LPARAN + expressionSequence + ~_T_RPARAN;
functionExpression = anonymousFunction
                     | classExpression
                     | thisExpression
                     | identifierExpression
                     | superExpression
                     | literalExpression
                     | arrayLiteralExpression
                     | objectLiteralExpression
                     | parenthesizedExpression;
classExpression = _K_CLASS + ~_ID + classTail;
memberIndexExpression = *memberIndexExpression + _T_LSQUARE + expressionSequence + _T_RSQUARE;
memberDotExpression = *memberDotExpression + *_T_QUERY + _T_DOT + *_T_SHARP + identifierName;
argumentsExpression = *argumentsExpression + arguments;
postIncrementExpression = *postIncrementExpression + _T_INC;
postDecreaseExpression = *postDecreaseExpression + _T_DEC;
postfixExpression = postfixExpression +
                    (_RULE_NO_LINE + memberIndexExpression
                     | memberDotExpression
                     | argumentsExpression
                     | _RULE_NO_LINE + postIncrementExpression
                     | _RULE_NO_LINE + postDecreaseExpression)
                    | functionExpression;
newExpression = _K_NEW + singleExpression + *arguments | postfixExpression;
deleteExpression = _K_DELETE + deleteExpression | newExpression;
voidExpression = _K_VOID + voidExpression | deleteExpression;
typeofExpression = _K_TYPEOF + typeofExpression | voidExpression;
preIncrementExpression = _T_INC + preIncrementExpression | typeofExpression;
preDecreaseExpression = _T_DEC + preDecreaseExpression | preIncrementExpression;
unaryPlusExpression = _T_ADD + unaryPlusExpression | preDecreaseExpression;
unaryMinusExpression = _T_SUB + unaryMinusExpression | unaryPlusExpression;
bitNotExpression = _T_BIT_NOT + bitNotExpression | unaryMinusExpression;
notExpression = _T_LOG_NOT + notExpression | bitNotExpression;
powerExpression = *(powerExpression + _T_POWER) + notExpression;
multiplicativeExpression = *(multiplicativeExpression + (_T_MUL | _T_DIV | _T_MOD)) + powerExpression;
additiveExpression = *(additiveExpression + (_T_ADD | _T_SUB)) + multiplicativeExpression;
coalesceExpression = *(coalesceExpression + (_T_COALESCE)) + additiveExpression;
bitShiftExpression = *(bitShiftExpression + (_T_LSHIFT | _T_RSHIFT | _T_URSHIFT)) + coalesceExpression;
relationalExpression = *(relationalExpression + (_T_LESS | _T_LESS_EQUAL | _T_GREATER | _T_GREATER_EQUAL)) +
                       bitShiftExpression;
instanceofExpression = *(instanceofExpression + _K_INSTANCEOF) + relationalExpression;
inExpression = *(inExpression + _K_IN) + instanceofExpression;
equalityExpression = *(equalityExpression + (_T_EQUAL | _T_NOT_EQUAL | _T_FEQUAL | _T_FNOT_EQUAL)) +
                     inExpression;
bitAndExpression = *(bitAndExpression + _T_BIT_AND) + equalityExpression;
bitXOrExpression = *(bitXOrExpression + _T_BIT_XOR) + bitAndExpression;
bitOrExpression = *(bitOrExpression + _T_BIT_OR) + bitXOrExpression;
logicalAndExpression = *(logicalAndExpression + _T_LOG_AND) + bitOrExpression;
logicalOrExpression = *(logicalOrExpression + _T_LOG_OR) + logicalAndExpression;
ternaryExpression = logicalOrExpression + *(_T_QUERY + singleExpression + _T_COLON + singleExpression);
assignmentOperatorExpression = *(assignmentOperatorExpression +
                                 (_T_ASSIGN_ADD | _T_ASSIGN_SUB | _T_ASSIGN_MUL | _T_ASSIGN_DIV |
                                  _T_ASSIGN_MOD | _T_ASSIGN_LSHIFT | _T_ASSIGN_RSHIFT | _T_ASSIGN_URSHIFT |
                                  _T_ASSIGN_AND | _T_ASSIGN_OR | _T_ASSIGN_XOR | _T_ASSIGN_POWER)) +
                               ternaryExpression;
assignmentExpression = *(assignmentExpression + _T_ASSIGN) + assignmentOperatorExpression;
singleExpression = *(singleExpression + ~_T_COMMA) + assignmentExpression;
literal = _K_NULL | _K_TRUE | _K_FALSE | _STRING | _REGEX | _NUMBER;
arrayLiteral = ~_T_LSQUARE + *elementList + ~_T_RSQUARE;
elementList = *(elementList + ~_T_COMMA) + arrayElement;
arrayElement = *_T_ELLIPSIS + singleExpression;
objectLiteral = ~_T_LBRACE + *propertyAssignments + *~_T_COMMA + ~_T_RBRACE;
identifierName = _ID | reservedWord;
reservedWord = keyword | _K_TRUE | _K_FALSE;
numericLiteral = _NUMBER;
assignable = _ID | arrayLiteral | objectLiteral;
arguments = ~_T_LPARAN + *argument + ~_T_RPARAN;
argument = *(argument + ~_T_COMMA) + *~_T_ELLIPSIS + (singleExpression | _ID);
propertyAssignments = *(propertyAssignments + ~_T_COMMA) + propertyAssignment;
propertyAssignment = propertyExpressionAssignment
                     | computedPropertyExpressionAssignment
                     | propertyShorthand;
propertyExpressionAssignment = propertyName + ~_T_COLON + singleExpression;
computedPropertyExpressionAssignment = ~_T_LPARAN + singleExpression + ~_T_RPARAN +
                                       ~_T_COLON + singleExpression;
propertyShorthand = *_T_ELLIPSIS + singleExpression;
propertyName = identifierName
               | _STRING
               | numericLiteral
               | ~_T_LSQUARE + singleExpression + ~_T_RSQUARE;
anonymousFunction = functionDecl
                    | anoymousFunctionDecl
                    | arrowFunction;
functionDecl = functionDeclaration;
anoymousFunctionDecl = _K_FUNCTION + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                       ~_T_LBRACE + *functionBody + ~_T_RBRACE;
arrowFunction = arrowFunctionParameters + ~_T_ARROW + arrowFunctionBody;
arrowFunctionParameters = _ID | ~_T_LPARAN + *formalParameterList + ~_T_RPARAN;
arrowFunctionBody = singleExpression | _T_LBRACE + *functionBody + _T_RBRACE;
eos = (~~_T_SEMI)((void *) &clear_bk) | _RULE_EOF | _RULE_LINE | _RULE_RBRACE;
keyword = _K_BREAK
          | _K_DO
          | _K_INSTANCEOF
          | _K_TYPEOF
          | _K_CASE
          | _K_ELSE
          | _K_NEW
          | _K_VAR
          | _K_CATCH
          | _K_FINALLY
          | _K_RETURN
          | _K_VOID
          | _K_CONTINUE
          | _K_FOR
          | _K_SWITCH
          | _K_WHILE
          | _K_DEBUGGER
          | _K_FUNCTION
          | _K_THIS
          | _K_WITH
          | _K_DEFAULT
          | _K_IF
          | _K_THROW
          | _K_DELETE
          | _K_IN
          | _K_TRY
          | _K_CLASS
          | _K_SUPER
          | _K_LET;
unit.adjust(&functionExpression, &anonymousFunction, e_shift, -1);
unit.adjust(&iterationStatement, &forInStatement, e_shift, -1);
unit.adjust(&iterationStatement, &forStatement, e_shift, 0, (void *) &pred_for);
unit.adjust(&inExpression, &inExpression, e_left_recursion, 0, (void *) &pred_in);
unit.gen(&program);
```

## LALR1 PDA Table

See [PDA.txt](https://github.com/bajdcc/clibjs/blob/master/test/PDA.txt).

## Output

See [Output.txt](https://github.com/bajdcc/clibjs/blob/master/test/output.txt), for parsing  `test/test_1.js` file.