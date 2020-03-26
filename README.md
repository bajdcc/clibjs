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
a = b = c, a += b -= c--;
a.b.c, a.b.c++;
a[1]["1"], a[1]["1"]++;
a = [1,2,[3+4],[]];
b = {1: "a", 'b': {c: c}, [2]: 1};
a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1);
(a>b?a:b).c(), !++a?b:c+1;
```

Output:

```javascript
C [#000] [NAME  ] a
C [#001] [NAME  ] b
C [#002] [NAME  ] c
C [#003] [NAME  ] d
C [#004] [NAME  ] e
C [#000] [NUMBER] 1.000000
C [#001] [NUMBER] 2.000000
C [#002] [NUMBER] 3.000000
C [#003] [STRING] "1"
C [#004] [NUMBER] 4.000000
C [#005] [STRING] "a"
C [#006] [STRING] 'b'
C [#007] [STRING] c
C [#008] [NUMBER] 5.000000
C [#009] [NUMBER] 6.000000
C [0001:017]          0 LOAD_CONST                  0          (1)
C [0001:005]          2 DUP_TOP                                (a)
C [0001:005]          3 STORE_NAME                  0          (a)
C [0001:009]          5 DUP_TOP                                (b)
C [0001:009]          6 STORE_NAME                  1          (b)
C [0001:013]          8 DUP_TOP                                (c)
C [0001:013]          9 STORE_NAME                  2          (c)
C [0002:009]         11 LOAD_NAME                   0          (a)
C [0002:014]         13 LOAD_NAME                   1          (b)
C [0002:018]         15 LOAD_NAME                   2          (c)
C [0002:014]         17 BINARY_MULTIPLY                        ((b * c))
C [0002:009]         18 BINARY_ADD                             (a + (b * c))
C [0002:005]         19 DUP_TOP                                (d)
C [0002:005]         20 STORE_NAME                  3          (d)
C [0003:009]         22 LOAD_CONST                  0          (1)
C [0003:009]         24 JUMP_IF_TRUE_OR_POP        32          (1 && 2 > 3)
C [0003:014]         26 LOAD_CONST                  1          (2)
C [0003:018]         28 LOAD_CONST                  2          (3)
C [0003:014]         30 COMPARE_OP                  4          (2 > 3)
C [0003:005]  >>     32 DUP_TOP                                (e)
C [0003:005]         33 STORE_NAME                  4          (e)
C [0004:001]         35 LOAD_NAME                   0          (a)
C [0004:005]         37 LOAD_NAME                   1          (b)
C [0004:001]         39 BINARY_ADD                             (a + b)
C [0004:001]         40 POP_TOP                                (a + b)
C [0004:008]         41 LOAD_NAME                   0          (a)
C [0004:012]         43 LOAD_NAME                   1          (b)
C [0004:008]         45 BINARY_SUBTRACT                        (a - b)
C [0004:001]         46 POP_TOP                                (a + b, a - b)
C [0005:001]         47 LOAD_CONST                  0          (1)
C [0005:009]         49 LOAD_NAME                   0          (a)
C [0005:009]         51 BINARY_DEC                             (--a)
C [0005:009]         52 DUP_TOP                                (--a)
C [0005:009]         53 DUP_TOP                                (a)
C [0005:009]         54 STORE_NAME                  0          (a)
C [0005:009]         56 DUP_TOP                                (--a--)
C [0005:009]         57 BINARY_DEC                             (--a--)
C [0005:009]         58 DUP_TOP                                (a)
C [0005:009]         59 STORE_NAME                  0          (a)
C [0005:009]         61 BINARY_DEC                             (--a)
C [0005:009]         62 DUP_TOP                                (--a)
C [0005:009]         63 DUP_TOP                                (a)
C [0005:009]         64 STORE_NAME                  0          (a)
C [0005:009]         66 DUP_TOP                                (--a--++)
C [0005:009]         67 BINARY_INC                             (--a--++)
C [0005:009]         68 DUP_TOP                                (a)
C [0005:009]         69 STORE_NAME                  0          (a)
C [0005:009]         71 BINARY_DEC                             (--a)
C [0005:009]         72 DUP_TOP                                (--a)
C [0005:009]         73 DUP_TOP                                (a)
C [0005:009]         74 STORE_NAME                  0          (a)
C [0005:001]         76 BINARY_ADD                             (1 + ++--a--++)
C [0005:001]         77 POP_TOP                                (1 + ++--a--++)
C [0006:009]         78 LOAD_NAME                   2          (c)
C [0006:005]         80 DUP_TOP                                (b)
C [0006:005]         81 STORE_NAME                  1          (b)
C [0006:001]         83 DUP_TOP                                (a)
C [0006:001]         84 STORE_NAME                  0          (a)
C [0006:001]         86 POP_TOP                                (a = b = c)
C [0006:012]         87 LOAD_NAME                   0          (a)
C [0006:017]         89 LOAD_NAME                   1          (b)
C [0006:022]         91 LOAD_NAME                   2          (c)
C [0006:022]         93 DUP_TOP                                (c--)
C [0006:022]         94 BINARY_DEC                             (c--)
C [0006:022]         95 DUP_TOP                                (c)
C [0006:022]         96 STORE_NAME                  2          (c)
C [0006:017]         98 BINARY_SUBTRACT                        (b -= c--)
C [0006:017]         99 DUP_TOP                                (b)
C [0006:017]        100 STORE_NAME                  1          (b)
C [0006:012]        102 BINARY_ADD                             (a += b -= c--)
C [0006:012]        103 DUP_TOP                                (a)
C [0006:012]        104 STORE_NAME                  0          (a)
C [0006:001]        106 POP_TOP                                (a = b = c, a += b -= c--)
C [0007:001]        107 LOAD_NAME                   0          (a)
C [0007:003]        109 LOAD_ATTR                   1          (b)
C [0007:005]        111 LOAD_ATTR                   2          (c)
C [0007:001]        113 POP_TOP                                (a.b.c)
C [0007:008]        114 LOAD_NAME                   0          (a)
C [0007:010]        116 LOAD_ATTR                   1          (b)
C [0007:012]        118 LOAD_ATTR                   2          (c)
C [0007:008]        120 DUP_TOP                                (a.b.c++)
C [0007:008]        121 BINARY_INC                             (a.b.c++)
C [0007:008]        122 LOAD_NAME                   0          (a)
C [0007:010]        124 LOAD_ATTR                   1          (b)
C [0007:012]        126 STORE_ATTR                  2          (c)
C [0007:001]        128 POP_TOP                                (a.b.c, a.b.c++)
C [0008:001]        129 LOAD_NAME                   0          (a)
C [0008:003]        131 LOAD_CONST                  0          ([1])
C [0008:003]        133 BINARY_SUBSCR                          ([1])
C [0008:006]        134 LOAD_CONST                  3          (["1"])
C [0008:006]        136 BINARY_SUBSCR                          (["1"])
C [0008:001]        137 POP_TOP                                (a[1]["1"])
C [0008:012]        138 LOAD_NAME                   0          (a)
C [0008:014]        140 LOAD_CONST                  0          ([1])
C [0008:014]        142 BINARY_SUBSCR                          ([1])
C [0008:017]        143 LOAD_CONST                  3          (["1"])
C [0008:017]        145 BINARY_SUBSCR                          (["1"])
C [0008:012]        146 DUP_TOP                                (a[1]["1"]++)
C [0008:012]        147 BINARY_INC                             (a[1]["1"]++)
C [0008:012]        148 LOAD_NAME                   0          (a)
C [0008:014]        150 LOAD_CONST                  0          ([1])
C [0008:014]        152 BINARY_SUBSCR                          ([1])
C [0008:017]        153 LOAD_CONST                  3          (["1"])
C [0008:017]        155 STORE_SUBSCR                           (["1"])
C [0008:001]        156 POP_TOP                                (a[1]["1"], a[1]["1"]++)
C [0009:006]        157 LOAD_CONST                  0          (1)
C [0009:008]        159 LOAD_CONST                  1          (2)
C [0009:011]        161 LOAD_CONST                  2          (3)
C [0009:013]        163 LOAD_CONST                  4          (4)
C [0009:011]        165 BINARY_ADD                             (3+4)
C [0009:010]        166 BUILD_LIST                  1          ([3+4])
C [0009:016]        168 BUILD_LIST                  0          ([])
C [0009:005]        170 BUILD_LIST                  4          ([1,2,[3+4],[]])
C [0009:001]        172 DUP_TOP                                (a)
C [0009:001]        173 STORE_NAME                  0          (a)
C [0009:001]        175 POP_TOP                                (a = [1,2,[3+4],[]])
C [0010:006]        176 LOAD_CONST                  0          (1)
C [0010:009]        178 LOAD_CONST                  5          ("a")
C [0010:014]        180 LOAD_CONST                  6          ('b')
C [0010:020]        182 LOAD_NAME                   2          (c)
C [0010:023]        184 LOAD_CONST                  7          (c)
C [0010:019]        186 BUILD_MAP                   1          ({c: c})
C [0010:028]        188 LOAD_CONST                  1          (2)
C [0010:032]        190 LOAD_CONST                  0          (1)
C [0010:005]        192 BUILD_MAP                   3          ({1: "a", 'b': {c: c}, [2]: 1})
C [0010:001]        194 DUP_TOP                                (b)
C [0010:001]        195 STORE_NAME                  1          (b)
C [0010:001]        197 POP_TOP                                (b = {1: "a", 'b': {c: c}, [2]: 1})
C [0011:001]        198 LOAD_NAME                   0          (a)
C [0011:003]        200 LOAD_METHOD                 1          (b)
C [0011:005]        202 LOAD_CONST                  0          (1)
C [0011:008]        204 LOAD_CONST                  1          (2)
C [0011:001]        206 CALL_METHOD                 2          (a.b(1, 2))
C [0011:011]        208 LOAD_METHOD                 2          (c)
C [0011:013]        210 LOAD_CONST                  1          (2)
C [0011:016]        212 LOAD_CONST                  0          (1)
C [0011:001]        214 CALL_METHOD                 2          (a.b(1, 2).c(2, 1))
C [0011:001]        216 POP_TOP                                (a.b(1, 2).c(2, 1))
C [0011:020]        217 LOAD_NAME                   0          (a)
C [0011:022]        219 LOAD_CONST                  2          (3)
C [0011:020]        221 CALL_FUNCTION               1          (a(3))
C [0011:025]        223 LOAD_CONST                  4          (4)
C [0011:020]        225 CALL_FUNCTION               1          (a(3)(4))
C [0011:020]        227 POP_TOP                                (a(3)(4))
C [0011:029]        228 LOAD_NAME                   0          (a)
C [0011:031]        230 LOAD_CONST                  8          ([5])
C [0011:031]        232 BINARY_SUBSCR                          ([5])
C [0011:034]        233 LOAD_CONST                  9          (6)
C [0011:029]        235 CALL_FUNCTION               1          (a[5](6))
C [0011:029]        237 POP_TOP                                (a[5](6))
C [0011:038]        238 LOAD_NAME                   0          (a)
C [0011:040]        240 LOAD_ATTR                   1          (b)
C [0011:042]        242 LOAD_METHOD                 2          (c)
C [0011:044]        244 LOAD_CONST                  0          (1)
C [0011:038]        246 CALL_METHOD                 1          (a.b.c(1))
C [0011:001]        248 POP_TOP                                (a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1))
C [0012:002]        249 LOAD_NAME                   0          (a)
C [0012:004]        251 LOAD_NAME                   1          (b)
C [0012:002]        253 COMPARE_OP                  4          (a>b)
C [0012:005]        255 POP_JUMP_IF_FALSE         261          (?)
C [0012:006]        257 LOAD_NAME                   0          (a)
C [0012:007]        259 JUMP_FORWARD                4          (:)
C [0012:008]  >>    261 LOAD_NAME                   1          (b)
C [0012:011]        263 LOAD_METHOD                 2          (c)
C [0012:002]  >>    265 CALL_METHOD                 0          ((a>b?a:b).c())
C [0012:002]        267 POP_TOP                                ((a>b?a:b).c())
C [0012:019]        268 LOAD_NAME                   0          (a)
C [0012:019]        270 BINARY_INC                             (++a)
C [0012:019]        271 DUP_TOP                                (++a)
C [0012:019]        272 DUP_TOP                                (a)
C [0012:019]        273 STORE_NAME                  0          (a)
C [0012:020]        275 POP_JUMP_IF_FALSE         281          (?)
C [0012:021]        277 LOAD_NAME                   1          (b)
C [0012:022]        279 JUMP_FORWARD                7          (:)
C [0012:023]  >>    281 LOAD_NAME                   2          (c)
C [0012:025]        283 LOAD_CONST                  0          (1)
C [0012:023]        285 BINARY_ADD                             (c+1)
C [0012:002]        286 POP_TOP                                ((a>b?a:b).c(), !++a?b:c+1)
```

## Grammar

See [grammars-v4/javascript](https://github.com/antlr/grammars-v4/blob/master/javascript/javascript/JavaScriptParser.g4).

```cpp
program = sourceElements;
variableStatement = _K_VAR + variableDeclarationList + eos;
variableDeclarationList = *(variableDeclarationList + ~_T_COMMA) + variableDeclaration;
variableDeclaration = assignable + *(~_T_ASSIGN + singleExpression);
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
parenthesizedExpression = _T_LPARAN + expressionSequence + _T_RPARAN;
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
memberIndexExpression = _T_LSQUARE + expressionSequence + _T_RSQUARE;
memberDotExpression = *_T_QUERY + ~_T_DOT + *_T_SHARP + identifierName;
argumentsExpression = arguments;
postIncrementExpression = _T_INC;
postDecreaseExpression = _T_DEC;
postfixExpression = *postfixExpression +
                    (_RULE_NO_LINE + memberIndexExpression
                     | memberDotExpression
                     | argumentsExpression
                     | _RULE_NO_LINE + postIncrementExpression
                     | _RULE_NO_LINE + postDecreaseExpression);
newExpression = _K_NEW + singleExpression + *arguments;
deleteExpression = _K_DELETE;
voidExpression = _K_VOID;
typeofExpression = _K_TYPEOF;
preIncrementExpression = _T_INC;
preDecreaseExpression = _T_DEC;
unaryPlusExpression = _T_ADD + *unaryPlusExpression;
unaryMinusExpression = _T_SUB + *unaryMinusExpression;
bitNotExpression = _T_BIT_NOT + *bitNotExpression;
notExpression = _T_LOG_NOT + *notExpression;
prefixExpression = *prefixExpression +
                   (newExpression
                    | deleteExpression
                    | voidExpression
                    | typeofExpression
                    | preIncrementExpression
                    | preDecreaseExpression
                    | unaryPlusExpression
                    | unaryMinusExpression
                    | bitNotExpression
                    | notExpression);
primaryExpression = *prefixExpression + functionExpression + *postfixExpression;
powerExpression = *(powerExpression + _T_POWER) + primaryExpression;
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
singleExpression = assignmentExpression;
literal = _K_NULL | _K_TRUE | _K_FALSE | _STRING | _REGEX | _NUMBER;
arrayLiteral = _T_LSQUARE + *elementList + _T_RSQUARE;
elementList = *(elementList + ~_T_COMMA) + arrayElement;
arrayElement = *_T_ELLIPSIS + singleExpression;
objectLiteral = _T_LBRACE + *propertyAssignments + *~_T_COMMA + _T_RBRACE;
identifierName = _ID | reservedWord;
reservedWord = keyword | _K_TRUE | _K_FALSE;
numericLiteral = _NUMBER;
assignable = _ID | arrayLiteral | objectLiteral;
arguments = ~_T_LPARAN + *argument + _T_RPARAN;
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