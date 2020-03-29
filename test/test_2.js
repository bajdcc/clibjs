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
function fun_a(b) {var a = 1;}
var f = function (b) {var b = 1;}
function (c) {var c = 1;}