# CLIBJS

JS-like script engine implemented by C++.

## Environment

- CLion, MinGW on Win7x64

## TODO

- [x] Lexer\(scanned js files in `test` dir\)
- [x] Parser\(Use LALR1 Parser with backtrace, see [clibparser](https://github.com/bajdcc/clibparser), Grammar: antlr/grammars-v4/javascript\)
- [x] Ast(\*Auto-generation* by LALR1 Parser\)
- [ ] Gen
- [ ] GC
- [ ] Env
- [ ] Interface

## Output

See [Output.txt](https://github.com/bajdcc/clibjs/blob/master/test/output.txt), for parsing  `test/test_1.js` file.