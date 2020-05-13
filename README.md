# CLIBJS

JS-like script engine implemented by C++.

Reference: quickjs

## Features

- [x] **64-bit support**
- [x] Parsing `jquery.js` and `Vue.js` in 1s.
- [x] Translate javascript file into **Python Bytecode** temporarily.
- [x] Build VM.
- [x] Closure.
- [x] GC, cache unused object
- [x] Binop\(tested in `test_all.js`, **TEST PASSED**\), BinComp, Unary
- [x] Y-combinator recursion.\(tested in `test_6.js`\)
- [x] Prototype, Attribute, Method
- [x] New, Delete
- [x] Rest parameters\(`...`\), Rest array elements\(`[...[]]`\), Rest object elements\(`{...{}}`\)
- [ ] Base functions\(Such as `Number`, etc\)
- [x] `sys.exec(...)` supports relative path
- [ ] Try, catch, finally

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
- [ ] array\(`slice`, `concat`, `push`, `map`, `filter`, `reduce`, `fill`, `toString`\)

## Modules and API

- [ ] console\(`log`(builtin), `trace`(builtin)\)
- [ ] sys\(`exec_file`(builtin)\)

## Global Function

- [x] setTimeout, setInterval, clearTimeout, clearInterval

## Control Flow

- [x] if, else
- [x] while, do-while
- [x] for, for-in
- [x] switch
- [ ] try, catch, finally

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

```javascript
(function () {
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
undefined
1
2
3
4
```

Input: (**test/test_all.js**) -- **All type binop**

Output: (**test/test_all_output.txt**)

## Grammar

See [grammars-v4/javascript](https://github.com/antlr/grammars-v4/blob/master/javascript/javascript/JavaScriptParser.g4).

## LALR1 PDA Table

See [PDA.txt](https://github.com/bajdcc/clibjs/blob/master/test/PDA.txt).

## Output

See [Output.txt](https://github.com/bajdcc/clibjs/blob/master/test/output.txt), for parsing  `test/test_1.js` file.