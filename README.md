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
C [0005:007]         51 BINARY_DEC                             (--a)
C [0005:007]         52 DUP_TOP                                (--a)
C [0005:009]         53 DUP_TOP                                (a)
C [0005:009]         54 STORE_NAME                  0          (a)
C [0005:005]         56 BINARY_INC                             (++--a)
C [0005:005]         57 DUP_TOP                                (++--a)
C [0005:009]         58 DUP_TOP                                (a)
C [0005:009]         59 STORE_NAME                  0          (a)
C [0005:007]         61 BINARY_DEC                             (--a)
C [0005:007]         62 DUP_TOP                                (--a)
C [0005:009]         63 DUP_TOP                                (a)
C [0005:009]         64 STORE_NAME                  0          (a)
C [0005:005]         66 DUP_TOP                                (++--a--)
C [0005:005]         67 BINARY_DEC                             (++--a--)
C [0005:009]         68 DUP_TOP                                (a)
C [0005:009]         69 STORE_NAME                  0          (a)
C [0005:007]         71 BINARY_DEC                             (--a)
C [0005:007]         72 DUP_TOP                                (--a)
C [0005:009]         73 DUP_TOP                                (a)
C [0005:009]         74 STORE_NAME                  0          (a)
C [0005:005]         76 BINARY_INC                             (++--a)
C [0005:005]         77 DUP_TOP                                (++--a)
C [0005:009]         78 DUP_TOP                                (a)
C [0005:009]         79 STORE_NAME                  0          (a)
C [0005:007]         81 BINARY_DEC                             (--a)
C [0005:007]         82 DUP_TOP                                (--a)
C [0005:009]         83 DUP_TOP                                (a)
C [0005:009]         84 STORE_NAME                  0          (a)
C [0005:005]         86 DUP_TOP                                (++--a--++)
C [0005:005]         87 BINARY_INC                             (++--a--++)
C [0005:009]         88 DUP_TOP                                (a)
C [0005:009]         89 STORE_NAME                  0          (a)
C [0005:007]         91 BINARY_DEC                             (--a)
C [0005:007]         92 DUP_TOP                                (--a)
C [0005:009]         93 DUP_TOP                                (a)
C [0005:009]         94 STORE_NAME                  0          (a)
C [0005:005]         96 BINARY_INC                             (++--a)
C [0005:005]         97 DUP_TOP                                (++--a)
C [0005:009]         98 DUP_TOP                                (a)
C [0005:009]         99 STORE_NAME                  0          (a)
C [0005:007]        101 BINARY_DEC                             (--a)
C [0005:007]        102 DUP_TOP                                (--a)
C [0005:009]        103 DUP_TOP                                (a)
C [0005:009]        104 STORE_NAME                  0          (a)
C [0005:001]        106 BINARY_ADD                             (1 + ++--a--++)
C [0005:001]        107 POP_TOP                                (1 + ++--a--++)
C [0006:009]        108 LOAD_NAME                   2          (c)
C [0006:005]        110 DUP_TOP                                (b)
C [0006:005]        111 STORE_NAME                  1          (b)
C [0006:001]        113 DUP_TOP                                (a)
C [0006:001]        114 STORE_NAME                  0          (a)
C [0006:001]        116 POP_TOP                                (a = b = c)
C [0006:012]        117 LOAD_NAME                   0          (a)
C [0006:017]        119 LOAD_NAME                   1          (b)
C [0006:022]        121 LOAD_NAME                   2          (c)
C [0006:022]        123 DUP_TOP                                (c--)
C [0006:022]        124 BINARY_DEC                             (c--)
C [0006:022]        125 DUP_TOP                                (c)
C [0006:022]        126 STORE_NAME                  2          (c)
C [0006:017]        128 BINARY_SUBTRACT                        (b -= c--)
C [0006:017]        129 DUP_TOP                                (b)
C [0006:017]        130 STORE_NAME                  1          (b)
C [0006:012]        132 BINARY_ADD                             (a += b -= c--)
C [0006:012]        133 DUP_TOP                                (a)
C [0006:012]        134 STORE_NAME                  0          (a)
C [0006:001]        136 POP_TOP                                (a = b = c, a += b -= c--)
C [0007:001]        137 LOAD_NAME                   0          (a)
C [0007:003]        139 LOAD_ATTR                   1          (b)
C [0007:005]        141 LOAD_ATTR                   2          (c)
C [0007:001]        143 POP_TOP                                (a.b.c)
C [0007:008]        144 LOAD_NAME                   0          (a)
C [0007:010]        146 LOAD_ATTR                   1          (b)
C [0007:012]        148 LOAD_ATTR                   2          (c)
C [0007:008]        150 DUP_TOP                                (a.b.c++)
C [0007:008]        151 BINARY_INC                             (a.b.c++)
C [0007:008]        152 LOAD_NAME                   0          (a)
C [0007:010]        154 LOAD_ATTR                   1          (b)
C [0007:012]        156 STORE_ATTR                  2          (c)
C [0007:001]        158 POP_TOP                                (a.b.c, a.b.c++)
C [0008:001]        159 LOAD_NAME                   0          (a)
C [0008:002]        161 LOAD_CONST                  0          ([1])
C [0008:002]        163 BINARY_SUBSCR                          ([1])
C [0008:005]        164 LOAD_CONST                  3          (["1"])
C [0008:005]        166 BINARY_SUBSCR                          (["1"])
C [0008:001]        167 POP_TOP                                (a[1]["1"])
C [0008:012]        168 LOAD_NAME                   0          (a)
C [0008:013]        170 LOAD_CONST                  0          ([1])
C [0008:013]        172 BINARY_SUBSCR                          ([1])
C [0008:016]        173 LOAD_CONST                  3          (["1"])
C [0008:016]        175 BINARY_SUBSCR                          (["1"])
C [0008:012]        176 DUP_TOP                                (a[1]["1"]++)
C [0008:012]        177 BINARY_INC                             (a[1]["1"]++)
C [0008:012]        178 LOAD_NAME                   0          (a)
C [0008:013]        180 LOAD_CONST                  0          ([1])
C [0008:013]        182 BINARY_SUBSCR                          ([1])
C [0008:016]        183 LOAD_CONST                  3          (["1"])
C [0008:016]        185 STORE_SUBSCR                           (["1"])
C [0008:001]        186 POP_TOP                                (a[1]["1"], a[1]["1"]++)
C [0009:006]        187 LOAD_CONST                  0          (1)
C [0009:008]        189 LOAD_CONST                  1          (2)
C [0009:011]        191 LOAD_CONST                  2          (3)
C [0009:013]        193 LOAD_CONST                  4          (4)
C [0009:011]        195 BINARY_ADD                             (3+4)
C [0009:010]        196 BUILD_LIST                  1          ([3+4])
C [0009:016]        198 BUILD_LIST                  0          ([])
C [0009:005]        200 BUILD_LIST                  4          ([1,2,[3+4],[]])
C [0009:001]        202 DUP_TOP                                (a)
C [0009:001]        203 STORE_NAME                  0          (a)
C [0009:001]        205 POP_TOP                                (a = [1,2,[3+4],[]])
C [0010:006]        206 LOAD_CONST                  0          (1)
C [0010:009]        208 LOAD_CONST                  5          ("a")
C [0010:014]        210 LOAD_CONST                  6          ('b')
C [0010:020]        212 LOAD_NAME                   2          (c)
C [0010:023]        214 LOAD_CONST                  7          (c)
C [0010:019]        216 BUILD_MAP                   1          ({c: c})
C [0010:028]        218 LOAD_CONST                  1          (2)
C [0010:032]        220 LOAD_CONST                  0          (1)
C [0010:005]        222 BUILD_MAP                   3          ({1: "a", 'b': {c: c}, [2]: 1})
C [0010:001]        224 DUP_TOP                                (b)
C [0010:001]        225 STORE_NAME                  1          (b)
C [0010:001]        227 POP_TOP                                (b = {1: "a", 'b': {c: c}, [2]: 1})
C [0011:001]        228 LOAD_NAME                   0          (a)
C [0011:003]        230 LOAD_METHOD                 1          (b)
C [0011:005]        232 LOAD_CONST                  0          (1)
C [0011:008]        234 LOAD_CONST                  1          (2)
C [0011:001]        236 CALL_METHOD                 2          (a.b(1, 2))
C [0011:011]        238 LOAD_METHOD                 2          (c)
C [0011:013]        240 LOAD_CONST                  1          (2)
C [0011:016]        242 LOAD_CONST                  0          (1)
C [0011:001]        244 CALL_METHOD                 2          (a.b(1, 2).c(2, 1))
C [0011:001]        246 POP_TOP                                (a.b(1, 2).c(2, 1))
C [0011:020]        247 LOAD_NAME                   0          (a)
C [0011:022]        249 LOAD_CONST                  2          (3)
C [0011:020]        251 CALL_FUNCTION               1          (a(3))
C [0011:025]        253 LOAD_CONST                  4          (4)
C [0011:020]        255 CALL_FUNCTION               1          (a(3)(4))
C [0011:020]        257 POP_TOP                                (a(3)(4))
C [0011:029]        258 LOAD_NAME                   0          (a)
C [0011:030]        260 LOAD_CONST                  8          ([5])
C [0011:030]        262 BINARY_SUBSCR                          ([5])
C [0011:034]        263 LOAD_CONST                  9          (6)
C [0011:029]        265 CALL_FUNCTION               1          (a[5](6))
C [0011:029]        267 POP_TOP                                (a[5](6))
C [0011:038]        268 LOAD_NAME                   0          (a)
C [0011:040]        270 LOAD_ATTR                   1          (b)
C [0011:042]        272 LOAD_METHOD                 2          (c)
C [0011:044]        274 LOAD_CONST                  0          (1)
C [0011:038]        276 CALL_METHOD                 1          (a.b.c(1))
C [0011:001]        278 POP_TOP                                (a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1))
C [0012:002]        279 LOAD_NAME                   0          (a)
C [0012:006]        281 LOAD_NAME                   1          (b)
C [0012:002]        283 COMPARE_OP                  4          (a > b)
C [0012:008]        285 POP_JUMP_IF_FALSE         291          (?)
C [0012:010]        287 LOAD_NAME                   0          (a)
C [0012:012]        289 JUMP_FORWARD                4          (:)
C [0012:014]  >>    291 LOAD_NAME                   1          (b)
C [0012:017]        293 LOAD_METHOD                 2          (c)
C [0012:002]  >>    295 CALL_METHOD                 0          ((a > b ? a : b).c())
C [0012:002]        297 POP_TOP                                ((a > b ? a : b).c())
C [0012:025]        298 LOAD_NAME                   0          (a)
C [0012:023]        300 BINARY_INC                             (++a)
C [0012:023]        301 DUP_TOP                                (++a)
C [0012:025]        302 DUP_TOP                                (a)
C [0012:025]        303 STORE_NAME                  0          (a)
C [0012:022]        305 UNARY_NOT                              (!++a)
C [0012:025]        306 DUP_TOP                                (a)
C [0012:025]        307 STORE_NAME                  0          (a)
C [0012:023]        309 BINARY_INC                             (++a)
C [0012:023]        310 DUP_TOP                                (++a)
C [0012:025]        311 DUP_TOP                                (a)
C [0012:025]        312 STORE_NAME                  0          (a)
C [0012:027]        314 POP_JUMP_IF_FALSE         320          (?)
C [0012:029]        316 LOAD_NAME                   1          (b)
C [0012:031]        318 JUMP_FORWARD                7          (:)
C [0012:033]  >>    320 LOAD_NAME                   2          (c)
C [0012:037]        322 LOAD_CONST                  0          (1)
C [0012:033]        324 BINARY_ADD                             (c + 1)
C [0012:002]        325 POP_TOP                                ((a > b ? a : b).c(), !++a ? b : c + 1)
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