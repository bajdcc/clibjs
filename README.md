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

Features:

- Unary, Sinop, Binop, Triop
- Member Index, Member Dot Function
- Array, Object
- Function, Lambda

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
(a > b ? a : b).c(), !++a ? b : c + 1;
function fun_a(b) {var a = 1;}
var b = function(b) {var b = 1;}
function(c) {var c = 1;}
```

Output:

```javascript
--== Main Function ==--
C [#000] [NAME  ] a
C [#001] [NAME  ] b
C [#002] [NAME  ] c
C [#003] [NAME  ] d
C [#004] [NAME  ] e
C [#005] [NAME  ] fun_a
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
C [#010] [FUNC  ] fun_a
C [#011] [STRING] fun_a
C [#012] [FUNC  ] [lambda]
C [#013] [STRING] <lambda>
C [#014] [FUNC  ] [lambda]
C [0001:017]  >>      0 LOAD_CONST                  0          (1)
C [0001:005]          2 DUP_TOP                                (a)
C [0001:005]          3 STORE_NAME                  0          (a)
C [0001:009]          5 DUP_TOP                                (b)
C [0001:009]          6 STORE_NAME                  1          (b)
C [0001:013]          8 DUP_TOP                                (c)
C [0001:013]          9 STORE_NAME                  2          (c)
C [0001:005]         11 POP_TOP                                (a = b = c = 1)
C [0002:009]         12 LOAD_NAME                   0          (a)
C [0002:014]         14 LOAD_NAME                   1          (b)
C [0002:018]         16 LOAD_NAME                   2          (c)
C [0002:014]         18 BINARY_MULTIPLY                        ((b * c))
C [0002:009]         19 BINARY_ADD                             (a + (b * c))
C [0002:005]         20 DUP_TOP                                (d)
C [0002:005]         21 STORE_NAME                  3          (d)
C [0002:005]         23 POP_TOP                                (d = a + (b * c))
C [0003:009]         24 LOAD_CONST                  0          (1)
C [0003:009]         26 JUMP_IF_TRUE_OR_POP         0          (1 && 2 > 3)
C [0003:014]         28 LOAD_CONST                  1          (2)
C [0003:018]         30 LOAD_CONST                  2          (3)
C [0003:014]         32 COMPARE_OP                  4          (2 > 3)
C [0003:005]         34 DUP_TOP                                (e)
C [0003:005]         35 STORE_NAME                  4          (e)
C [0003:005]         37 POP_TOP                                (e = 1 && 2 > 3)
C [0004:001]         38 LOAD_NAME                   0          (a)
C [0004:005]         40 LOAD_NAME                   1          (b)
C [0004:001]         42 BINARY_ADD                             (a + b)
C [0004:001]         43 POP_TOP                                (a + b)
C [0004:008]         44 LOAD_NAME                   0          (a)
C [0004:012]         46 LOAD_NAME                   1          (b)
C [0004:008]         48 BINARY_SUBTRACT                        (a - b)
C [0004:001]         49 POP_TOP                                (a + b, a - b)
C [0005:001]         50 LOAD_CONST                  0          (1)
C [0005:009]         52 LOAD_NAME                   0          (a)
C [0005:007]         54 BINARY_DEC                             (--a)
C [0005:007]         55 DUP_TOP                                (--a)
C [0005:009]         56 DUP_TOP                                (a)
C [0005:009]         57 STORE_NAME                  0          (a)
C [0005:005]         59 BINARY_INC                             (++--a)
C [0005:005]         60 DUP_TOP                                (++--a)
C [0005:009]         61 DUP_TOP                                (a)
C [0005:009]         62 STORE_NAME                  0          (a)
C [0005:007]         64 BINARY_DEC                             (--a)
C [0005:007]         65 DUP_TOP                                (--a)
C [0005:009]         66 DUP_TOP                                (a)
C [0005:009]         67 STORE_NAME                  0          (a)
C [0005:005]         69 DUP_TOP                                (++--a--)
C [0005:005]         70 BINARY_DEC                             (++--a--)
C [0005:009]         71 DUP_TOP                                (a)
C [0005:009]         72 STORE_NAME                  0          (a)
C [0005:007]         74 BINARY_DEC                             (--a)
C [0005:007]         75 DUP_TOP                                (--a)
C [0005:009]         76 DUP_TOP                                (a)
C [0005:009]         77 STORE_NAME                  0          (a)
C [0005:005]         79 BINARY_INC                             (++--a)
C [0005:005]         80 DUP_TOP                                (++--a)
C [0005:009]         81 DUP_TOP                                (a)
C [0005:009]         82 STORE_NAME                  0          (a)
C [0005:007]         84 BINARY_DEC                             (--a)
C [0005:007]         85 DUP_TOP                                (--a)
C [0005:009]         86 DUP_TOP                                (a)
C [0005:009]         87 STORE_NAME                  0          (a)
C [0005:005]         89 DUP_TOP                                (++--a--++)
C [0005:005]         90 BINARY_INC                             (++--a--++)
C [0005:009]         91 DUP_TOP                                (a)
C [0005:009]         92 STORE_NAME                  0          (a)
C [0005:007]         94 BINARY_DEC                             (--a)
C [0005:007]         95 DUP_TOP                                (--a)
C [0005:009]         96 DUP_TOP                                (a)
C [0005:009]         97 STORE_NAME                  0          (a)
C [0005:005]         99 BINARY_INC                             (++--a)
C [0005:005]        100 DUP_TOP                                (++--a)
C [0005:009]        101 DUP_TOP                                (a)
C [0005:009]        102 STORE_NAME                  0          (a)
C [0005:007]        104 BINARY_DEC                             (--a)
C [0005:007]        105 DUP_TOP                                (--a)
C [0005:009]        106 DUP_TOP                                (a)
C [0005:009]        107 STORE_NAME                  0          (a)
C [0005:001]        109 BINARY_ADD                             (1 + ++--a--++)
C [0005:001]        110 POP_TOP                                (1 + ++--a--++)
C [0006:009]        111 LOAD_NAME                   2          (c)
C [0006:005]        113 DUP_TOP                                (b)
C [0006:005]        114 STORE_NAME                  1          (b)
C [0006:001]        116 DUP_TOP                                (a)
C [0006:001]        117 STORE_NAME                  0          (a)
C [0006:001]        119 POP_TOP                                (a = b = c)
C [0006:012]        120 LOAD_NAME                   0          (a)
C [0006:017]        122 LOAD_NAME                   1          (b)
C [0006:022]        124 LOAD_NAME                   2          (c)
C [0006:022]        126 DUP_TOP                                (c--)
C [0006:022]        127 BINARY_DEC                             (c--)
C [0006:022]        128 DUP_TOP                                (c)
C [0006:022]        129 STORE_NAME                  2          (c)
C [0006:017]        131 BINARY_SUBTRACT                        (b -= c--)
C [0006:017]        132 DUP_TOP                                (b)
C [0006:017]        133 STORE_NAME                  1          (b)
C [0006:012]        135 BINARY_ADD                             (a += b -= c--)
C [0006:012]        136 DUP_TOP                                (a)
C [0006:012]        137 STORE_NAME                  0          (a)
C [0006:001]        139 POP_TOP                                (a = b = c, a += b -= c--)
C [0007:001]        140 LOAD_NAME                   0          (a)
C [0007:003]        142 LOAD_ATTR                   1          (b)
C [0007:005]        144 LOAD_ATTR                   2          (c)
C [0007:001]        146 POP_TOP                                (a.b.c)
C [0007:008]        147 LOAD_NAME                   0          (a)
C [0007:010]        149 LOAD_ATTR                   1          (b)
C [0007:012]        151 LOAD_ATTR                   2          (c)
C [0007:008]        153 DUP_TOP                                (a.b.c++)
C [0007:008]        154 BINARY_INC                             (a.b.c++)
C [0007:008]        155 LOAD_NAME                   0          (a)
C [0007:010]        157 LOAD_ATTR                   1          (b)
C [0007:012]        159 STORE_ATTR                  2          (c)
C [0007:001]        161 POP_TOP                                (a.b.c, a.b.c++)
C [0008:001]        162 LOAD_NAME                   0          (a)
C [0008:002]        164 LOAD_CONST                  0          ([1])
C [0008:002]        166 BINARY_SUBSCR                          ([1])
C [0008:005]        167 LOAD_CONST                  3          (["1"])
C [0008:005]        169 BINARY_SUBSCR                          (["1"])
C [0008:001]        170 POP_TOP                                (a[1]["1"])
C [0008:012]        171 LOAD_NAME                   0          (a)
C [0008:013]        173 LOAD_CONST                  0          ([1])
C [0008:013]        175 BINARY_SUBSCR                          ([1])
C [0008:016]        176 LOAD_CONST                  3          (["1"])
C [0008:016]        178 BINARY_SUBSCR                          (["1"])
C [0008:012]        179 DUP_TOP                                (a[1]["1"]++)
C [0008:012]        180 BINARY_INC                             (a[1]["1"]++)
C [0008:012]        181 LOAD_NAME                   0          (a)
C [0008:013]        183 LOAD_CONST                  0          ([1])
C [0008:013]        185 BINARY_SUBSCR                          ([1])
C [0008:016]        186 LOAD_CONST                  3          (["1"])
C [0008:016]        188 STORE_SUBSCR                           (["1"])
C [0008:001]        189 POP_TOP                                (a[1]["1"], a[1]["1"]++)
C [0009:006]        190 LOAD_CONST                  0          (1)
C [0009:008]        192 LOAD_CONST                  1          (2)
C [0009:011]        194 LOAD_CONST                  2          (3)
C [0009:013]        196 LOAD_CONST                  4          (4)
C [0009:011]        198 BINARY_ADD                             (3+4)
C [0009:010]        199 BUILD_LIST                  1          ([3+4])
C [0009:016]        201 BUILD_LIST                  0          ([])
C [0009:005]        203 BUILD_LIST                  4          ([1,2,[3+4],[]])
C [0009:001]        205 DUP_TOP                                (a)
C [0009:001]        206 STORE_NAME                  0          (a)
C [0009:001]        208 POP_TOP                                (a = [1,2,[3+4],[]])
C [0010:006]        209 LOAD_CONST                  0          (1)
C [0010:009]        211 LOAD_CONST                  5          ("a")
C [0010:014]        213 LOAD_CONST                  6          ('b')
C [0010:020]        215 LOAD_NAME                   2          (c)
C [0010:023]        217 LOAD_CONST                  7          (c)
C [0010:019]        219 BUILD_MAP                   1          ({c: c})
C [0010:028]        221 LOAD_CONST                  1          (2)
C [0010:032]        223 LOAD_CONST                  0          (1)
C [0010:005]        225 BUILD_MAP                   3          ({1: "a", 'b': {c: c}, [2]: 1})
C [0010:001]        227 DUP_TOP                                (b)
C [0010:001]        228 STORE_NAME                  1          (b)
C [0010:001]        230 POP_TOP                                (b = {1: "a", 'b': {c: c}, [2]: 1})
C [0011:001]        231 LOAD_NAME                   0          (a)
C [0011:003]        233 LOAD_METHOD                 1          (b)
C [0011:005]        235 LOAD_CONST                  0          (1)
C [0011:008]        237 LOAD_CONST                  1          (2)
C [0011:001]        239 CALL_METHOD                 2          (a.b(1, 2))
C [0011:011]        241 LOAD_METHOD                 2          (c)
C [0011:013]        243 LOAD_CONST                  1          (2)
C [0011:016]        245 LOAD_CONST                  0          (1)
C [0011:001]        247 CALL_METHOD                 2          (a.b(1, 2).c(2, 1))
C [0011:001]        249 POP_TOP                                (a.b(1, 2).c(2, 1))
C [0011:020]        250 LOAD_NAME                   0          (a)
C [0011:022]        252 LOAD_CONST                  2          (3)
C [0011:020]        254 CALL_FUNCTION               1          (a(3))
C [0011:025]        256 LOAD_CONST                  4          (4)
C [0011:020]        258 CALL_FUNCTION               1          (a(3)(4))
C [0011:020]        260 POP_TOP                                (a(3)(4))
C [0011:029]        261 LOAD_NAME                   0          (a)
C [0011:030]        263 LOAD_CONST                  8          ([5])
C [0011:030]        265 BINARY_SUBSCR                          ([5])
C [0011:034]        266 LOAD_CONST                  9          (6)
C [0011:029]        268 CALL_FUNCTION               1          (a[5](6))
C [0011:029]        270 POP_TOP                                (a[5](6))
C [0011:038]        271 LOAD_NAME                   0          (a)
C [0011:040]        273 LOAD_ATTR                   1          (b)
C [0011:042]        275 LOAD_METHOD                 2          (c)
C [0011:044]        277 LOAD_CONST                  0          (1)
C [0011:038]        279 CALL_METHOD                 1          (a.b.c(1))
C [0011:001]        281 POP_TOP                                (a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1))
C [0012:002]        282 LOAD_NAME                   0          (a)
C [0012:006]        284 LOAD_NAME                   1          (b)
C [0012:002]        286 COMPARE_OP                  4          (a > b)
C [0012:008]        288 POP_JUMP_IF_FALSE           0          (?)
C [0012:010]        290 LOAD_NAME                   0          (a)
C [0012:012]        292 JUMP_FORWARD                0          (:)
C [0012:014]  >>    294 LOAD_NAME                   1          (b)
C [0012:017]        296 LOAD_METHOD                 2          (c)
C [0012:002]        298 CALL_METHOD                 0          ((a > b ? a : b).c())
C [0012:002]        300 POP_TOP                                ((a > b ? a : b).c())
C [0012:025]        301 LOAD_NAME                   0          (a)
C [0012:023]        303 BINARY_INC                             (++a)
C [0012:023]        304 DUP_TOP                                (++a)
C [0012:025]        305 DUP_TOP                                (a)
C [0012:025]        306 STORE_NAME                  0          (a)
C [0012:022]        308 UNARY_NOT                              (!++a)
C [0012:025]        309 DUP_TOP                                (a)
C [0012:025]        310 STORE_NAME                  0          (a)
C [0012:023]        312 BINARY_INC                             (++a)
C [0012:023]        313 DUP_TOP                                (++a)
C [0012:025]        314 DUP_TOP                                (a)
C [0012:025]        315 STORE_NAME                  0          (a)
C [0012:027]        317 POP_JUMP_IF_FALSE           0          (?)
C [0012:029]        319 LOAD_NAME                   1          (b)
C [0012:031]        321 JUMP_FORWARD                0          (:)
C [0012:033]  >>    323 LOAD_NAME                   2          (c)
C [0012:037]        325 LOAD_CONST                  0          (1)
C [0012:033]        327 BINARY_ADD                             (c + 1)
C [0012:002]        328 POP_TOP                                ((a > b ? a : b).c(), !++a ? b : c + 1)
C [0013:010]        329 LOAD_CONST                 10          (fun_a)
C [0013:010]        331 LOAD_CONST                 11          (fun_a)
C [0013:001]        333 MAKE_FUNCTION                          (function fun_a(b) {var a = 1;})
C [0013:010]        334 STORE_NAME                  5          (fun_a)
C [0013:001]        336 POP_TOP                                (function fun_a(b) {var a = 1;})
C [0000:000]        337 LOAD_CONST                 12          (...)
C [0000:000]        339 LOAD_CONST                 13          (...)
C [0014:009]        341 MAKE_FUNCTION                          (function(b) {var b = 1;})
C [0014:005]        342 DUP_TOP                                (b)
C [0014:005]        343 STORE_NAME                  1          (b)
C [0014:005]        345 POP_TOP                                (b = function(b) {var b = 1;})
C [0000:000]        346 LOAD_CONST                 14          (...)
C [0000:000]        348 LOAD_CONST                 13          (...)
C [0015:001]        350 MAKE_FUNCTION                          (function(c) {var c = 1;})
C [0015:001]        351 POP_TOP                                (function(c) {var c = 1;})
--== Function: "fun_a" ==--
C [#000] [NAME  ] a
C [#000] [NUMBER] 1.000000
C [0013:028]          0 LOAD_CONST                  0          (1)
C [0013:024]          2 DUP_TOP                                (a)
C [0013:024]          3 STORE_NAME                  0          (a)
C [0013:024]          5 POP_TOP                                (a = 1)
--== Function: "[lambda]" ==--
C [#000] [NAME  ] b
C [#000] [NUMBER] 1.000000
C [0014:030]          0 LOAD_CONST                  0          (1)
C [0014:026]          2 DUP_TOP                                (b)
C [0014:026]          3 STORE_NAME                  0          (b)
C [0014:026]          5 POP_TOP                                (b = 1)
--== Function: "[lambda]" ==--
C [#000] [NAME  ] c
C [#000] [NUMBER] 1.000000
C [0015:022]          0 LOAD_CONST                  0          (1)
C [0015:018]          2 DUP_TOP                                (c)
C [0015:018]          3 STORE_NAME                  0          (c)
C [0015:018]          5 POP_TOP                                (c = 1)
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