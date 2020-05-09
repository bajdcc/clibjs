# CLIBJS

JS-like script engine implemented by C++.

Reference: quickjs

## Features

- [x] Parsing `jquery.js` and `Vue.js` in 1s.
- [x] Translate javascript file into **Python Bytecode** temporarily.
- [x] Build VM.
- [x] Closure.
- [x] GC, cache unused object
- [x] Binop\(tested in `test_4.js`\), BinComp, Unary
- [x] Y-combinator recursion.\(tested in `test_6.js`\)
- [x] Prototype, Attribute, Method
- [x] New, Delete
- [x] Rest parameters\(`...`\), Rest array elements\(`[...[]]`\), Rest object elements\(`{...{}}`\)
- [ ] Base functions\(Such as `Number`, etc\)

## Environment

- CLion, MinGW on Win7x64
- C++14

## TODO

- [x] Lexer\(scanned js files in `test` dir\)
- [x] Parser\(Use LALR1 Parser with backtrace, see [clibparser](https://github.com/bajdcc/clibparser), Grammar: antlr/grammars-v4/javascript\)
- [x] Syntax Tree\(**Auto-generation** by LALR1 Parser\)
- [x] AST\(On progress\)
- [x] IL Design\(**Use [Python Bytecode](https://github.com/python/cpython/blob/master/Include/opcode.h)** temporarily\)
- [x] Gen\(On progress\)
- [x] GC
- [x] Runtime\(On progress\)
- [x] Prototype\(On progress\)
- [ ] Interface

## Prototype

- [ ] root\(`debug_dump(builtin)`\)
- [ ] object\(`hasOwnProperty`\)
- [ ] number
- [ ] boolean
- [ ] string
- [ ] function\(`call`(builtin), `apply`(builtin), `bind`\)
- [ ] array\(`slice`, `concat`, `push`, `map`, `filter`, `reduce`, `fill`\)

## Modules and API

- [ ] console\(`log`(builtin), `trace`(builtin)\)
- [ ] sys\(`exec_file`(builtin)\)

## Global Function

- [ ] setTimeout

## Control Flow

- [x] if, else
- [x] while, do-while
- [x] for, for-in
- [x] switch

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
a = [1, 2, [3 + 4], []];
b = {1: "a", 'b': {c: c}, [2]: 1};
a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1);
(a > b ? a : b).c(), !++a ? b : c + 1;
function fun_a(b) {var a = 1; function c(){var b = 1;}}
var f = function (b) {var b = 1;}
function (c) {return function (){return c++ +g;}}
```

Output:

See [Output2.txt](https://github.com/bajdcc/clibjs/blob/master/test/output2.txt), for parsing  `test/test_2.js` file.

Input: (**test/test_3.js**)

```javascript
var a = 1;
b = 2;
function add(a,b) {return a+b;}
return add(a, b);
```

Output:

See [Output3.txt](https://github.com/bajdcc/clibjs/blob/master/test/output3.txt), for parsing and running  `test/test_3.js` file.

Input: (**test/test_4.js**) -- Closure

```javascript
var a = 1, b = 2;
return (a => b => a+b)(a)(b);
```

Output:

See [Output4.txt](https://github.com/bajdcc/clibjs/blob/master/test/output4.txt), for parsing and running  `test/test_3.js` file.

Input: (**test/test_7.js**) -- Method and prototype

```javascript
console.log("a".debug_dump());
console.log(0 .debug_dump());
console.log({}.debug_dump());
console.log(null.debug_dump());
console.log(undefined.debug_dump());
console.log(this.debug_dump());
console.log(true.debug_dump());
console.log(false.debug_dump());
console.log(console.debug_dump());
```

Output:

```
Str: a, Type: string, Ptr: 00fa3928
Str: 0, Type: number, Ptr: 015074e0
Str: [object Object], Type: object, Ptr: 0158f79c
Str: null, Type: object, Ptr: 01507374
Str: undefined, Type: undefined, Ptr: 015073a4
Str: [object Object], Type: object, Ptr: 0158f004
Str: true, Type: boolean, Ptr: 015073d4
Str: false, Type: boolean, Ptr: 01507404
Str: [object Object], Type: object, Ptr: 015077f4
undefined
```

Input: (**test/test_8.js**) -- New

```javascript
function A(a, b) {
    this.a = a;
    this.b = b;
    this.c = function () {
        console.log(console.trace());
        return this.a + ' ' + this.b;
    };
}

var d = new A('123', 12.3);
console.log(d.c());

var obj = {0: 'a', 1: 'b', length: 2};
console.log([].slice.call(obj, 0).slice(1));

console.log([1].concat(1, [2], 3));
console.log([1, 2, 3, 4].map(x => x + 1).filter(x => x % 2 === 0));
console.log([1, 2, 3, 4].reduce((a, b) => a + b));
console.log([1, 2, 3, 4].reduce((a, b) => a + b, 1));
console.log([...[1, 2], ...[3, 4]].fill(5));
console.log.bind(null, 1, 2, 3)();
Array.prototype.concat.bind(0, 1, 2, 3)();
for (var i in {a: 1, ...{b: 2}}) console.log(i);
for (var i in [1,2]) console.log(i);
```

Output:

```
library loaded
3: (..\test\test_8.js:4:5) A.prototype.c
2: (..\test\test_8.js:1:1) <entry>
1: (<starter>) <entry>
123 12.3
[b]
[1, 1, 2, 3]
[2, 4]
10
11
[5, 5, 5, 5]
1 2 3
[0, 1, 2, 3]
a
b
0
1
```

Input: (**test/test_9.js**) -- setTimeout

```javascript(function () {
    var i = 0;
    console.log(i);
    setTimeout(function a() {
        if (++i > 4) return;
        console.log(i);
        setTimeout(a);
    });
})();
```

Output:

```
0
1
2
3
4
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
ifStatement = _K_IF + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement + *(~_K_ELSE + statement);
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
            | functionStatement;
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
newExpression = _K_NEW + singleExpression + *arguments;
functionExpression = anonymousFunction
                     | classExpression
                     | thisExpression
                     | identifierExpression
                     | superExpression
                     | literalExpression
                     | arrayLiteralExpression
                     | objectLiteralExpression
                     | parenthesizedExpression
                     | newExpression;
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
deleteExpression = _K_DELETE;
voidExpression = _K_VOID;
typeofExpression = _K_TYPEOF;
preIncrementExpression = _T_INC;
preDecreaseExpression = _T_DEC;
unaryPlusExpression = _T_ADD;
unaryMinusExpression = _T_SUB;
bitNotExpression = _T_BIT_NOT;
notExpression = _T_LOG_NOT;
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
literal = _K_NULL | _K_UNDEFINED | _K_TRUE | _K_FALSE | _STRING | _REGEX | _NUMBER;
commaList = *commaList + _T_COMMA;
arrayLiteral = _T_LSQUARE + *elementList + _T_RSQUARE;
elementList = *(elementList) + *commaList + arrayElement + *commaList;
arrayElement = *_T_ELLIPSIS + singleExpression;
objectLiteral = _T_LBRACE + *propertyAssignments + *~_T_COMMA + _T_RBRACE;
identifierName = _ID | reservedWord;
reservedWord = keyword | _K_TRUE | _K_FALSE;
numericLiteral = _NUMBER;
assignable = _ID | arrayLiteral | objectLiteral;
arguments = ~_T_LPARAN + *argument + _T_RPARAN;
argument = *(argument + ~_T_COMMA) + *~_T_ELLIPSIS + singleExpression;
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
functionStatement = anonymousFunction;
anonymousFunction = functionDecl
                    | anoymousFunctionDecl
                    | arrowFunction;
functionDecl = functionDeclaration;
functionDeclaration = _K_FUNCTION + _ID + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                      ~_T_LBRACE + *functionBody + _T_RBRACE;
anoymousFunctionDecl = _K_FUNCTION + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                       ~_T_LBRACE + *functionBody + _T_RBRACE;
arrowFunction = arrowFunctionParameters + ~_T_ARROW + arrowFunctionBody;
arrowFunctionParameters = _ID | _T_LPARAN + *formalParameterList + _T_RPARAN;
arrowFunctionBody = singleExpression | ~_T_LBRACE + *functionBody + _T_RBRACE;
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
Reference: quickjs

## Features

- [x] Parsing `jquery.js` and `Vue.js` in 1s.
- [x] Translate javascript file into **Python Bytecode** temporarily.
- [x] Build VM.
- [x] Closure.
- [x] GC, cache unused object
- [x] Binop\(tested in `test_4.js`\), BinComp, Unary
- [x] Y-combinator recursion.\(tested in `test_6.js`\)
- [x] Prototype, Attribute, Method
- [x] New.Target
- [ ] Base functions\(Such as `Number`, etc\)

## Environment

- CLion, MinGW on Win7x64
- C++14

## TODO

- [x] Lexer\(scanned js files in `test` dir\)
- [x] Parser\(Use LALR1 Parser with backtrace, see [clibparser](https://github.com/bajdcc/clibparser), Grammar: antlr/grammars-v4/javascript\)
- [x] Syntax Tree\(**Auto-generation** by LALR1 Parser\)
- [x] AST\(On progress\)
- [x] IL Design\(**Use [Python Bytecode](https://github.com/python/cpython/blob/master/Include/opcode.h)** temporarily\)
- [x] Gen\(On progress\)
- [x] GC\(On progress\)
- [x] Runtime\(On progress\)
- [x] Prototype\(On progress\)
- [ ] Interface

## Prototype

- [ ] root\(`debug_dump(builtin)`\)
- [ ] object\(`hasOwnProperty`\)
- [ ] number
- [ ] boolean
- [ ] string
- [ ] function\(`call`(builtin), `apply`(builtin)\)
- [ ] array\(`slice`, `concat`, `push`, `map`, `filter`, `reduce`, `fill`\)

## Control Flow

- [x] if
- [x] while
- [x] for
- [x] for-in

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
a = [1, 2, [3 + 4], []];
b = {1: "a", 'b': {c: c}, [2]: 1};
a.b(1, 2).c(2, 1), a(3)(4), a[5](6), a.b.c(1);
(a > b ? a : b).c(), !++a ? b : c + 1;
function fun_a(b) {var a = 1; function c(){var b = 1;}}
var f = function (b) {var b = 1;}
function (c) {return function (){return c++ +g;}}
```

Output:

See [Output2.txt](https://github.com/bajdcc/clibjs/blob/master/test/output2.txt), for parsing  `test/test_2.js` file.

Input: (**test/test_3.js**)

```javascript
var a = 1;
b = 2;
function add(a,b) {return a+b;}
return add(a, b);
```

Output:

See [Output3.txt](https://github.com/bajdcc/clibjs/blob/master/test/output3.txt), for parsing and running  `test/test_3.js` file.

Input: (**test/test_4.js**) -- Closure

```javascript
var a = 1, b = 2;
return (a => b => a+b)(a)(b);
```

Output:

See [Output4.txt](https://github.com/bajdcc/clibjs/blob/master/test/output4.txt), for parsing and running  `test/test_3.js` file.

Input: (**test/test_7.js**) -- Method and prototype

```javascript
console.log("a".debug_dump());
console.log(0 .debug_dump());
console.log({}.debug_dump());
console.log(null.debug_dump());
console.log(undefined.debug_dump());
console.log(this.debug_dump());
console.log(true.debug_dump());
console.log(false.debug_dump());
console.log(console.debug_dump());
```

Output:

```
Str: a, Type: string, Ptr: 00fa3928
Str: 0, Type: number, Ptr: 015074e0
Str: [object Object], Type: object, Ptr: 0158f79c
Str: null, Type: object, Ptr: 01507374
Str: undefined, Type: undefined, Ptr: 015073a4
Str: [object Object], Type: object, Ptr: 0158f004
Str: true, Type: boolean, Ptr: 015073d4
Str: false, Type: boolean, Ptr: 01507404
Str: [object Object], Type: object, Ptr: 015077f4
undefined
```

Input: (**test/test_8.js**) -- New target

```javascript
function A(a, b) {
    this.a = a;
    this.b = b;
    this.c = function () {
        return this.a + ' ' + this.b;
    };
}

var d = new A('123', 12.3);
console.log(d.c());

var obj = {0: 'a', 1: 'b', length: 2};
console.log([].slice.call(obj, 0).slice(1));

console.log([1].concat(1, [2], 3));
console.log([1, 2, 3, 4].map(x => x + 1).filter(x => x % 2 === 0));
console.log([1, 2, 3, 4].reduce((a, b) => a + b));
console.log([1, 2, 3, 4].reduce((a, b) => a + b, 1));
console.log([1, 2, 3, 4].fill(5));
```

Output:

```
library loaded
123 12.3
[b]
[1, 1, 2, 3]
[2, 4]
10
11
[5, 5, 5, 5]
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
ifStatement = _K_IF + ~_T_LPARAN + expressionSequence + ~_T_RPARAN + statement + *(~_K_ELSE + statement);
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
            | functionStatement;
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
newExpression = _K_NEW + singleExpression + *arguments;
functionExpression = anonymousFunction
                     | classExpression
                     | thisExpression
                     | identifierExpression
                     | superExpression
                     | literalExpression
                     | arrayLiteralExpression
                     | objectLiteralExpression
                     | parenthesizedExpression
                     | newExpression;
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
deleteExpression = _K_DELETE;
voidExpression = _K_VOID;
typeofExpression = _K_TYPEOF;
preIncrementExpression = _T_INC;
preDecreaseExpression = _T_DEC;
unaryPlusExpression = _T_ADD;
unaryMinusExpression = _T_SUB;
bitNotExpression = _T_BIT_NOT;
notExpression = _T_LOG_NOT;
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
literal = _K_NULL | _K_UNDEFINED | _K_TRUE | _K_FALSE | _STRING | _REGEX | _NUMBER;
commaList = *commaList + _T_COMMA;
arrayLiteral = _T_LSQUARE + *elementList + _T_RSQUARE;
elementList = *(elementList) + *commaList + arrayElement + *commaList;
arrayElement = *_T_ELLIPSIS + singleExpression;
objectLiteral = _T_LBRACE + *propertyAssignments + *~_T_COMMA + _T_RBRACE;
identifierName = _ID | reservedWord;
reservedWord = keyword | _K_TRUE | _K_FALSE;
numericLiteral = _NUMBER;
assignable = _ID | arrayLiteral | objectLiteral;
arguments = ~_T_LPARAN + *argument + _T_RPARAN;
argument = *(argument + ~_T_COMMA) + *~_T_ELLIPSIS + singleExpression;
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
functionStatement = anonymousFunction;
anonymousFunction = functionDecl
                    | anoymousFunctionDecl
                    | arrowFunction;
functionDecl = functionDeclaration;
functionDeclaration = _K_FUNCTION + _ID + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                      ~_T_LBRACE + *functionBody + _T_RBRACE;
anoymousFunctionDecl = _K_FUNCTION + ~_T_LPARAN + *formalParameterList + ~_T_RPARAN +
                       ~_T_LBRACE + *functionBody + _T_RBRACE;
arrowFunction = arrowFunctionParameters + ~_T_ARROW + arrowFunctionBody;
arrowFunctionParameters = _ID | _T_LPARAN + *formalParameterList + _T_RPARAN;
arrowFunctionBody = singleExpression | ~_T_LBRACE + *functionBody + _T_RBRACE;
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