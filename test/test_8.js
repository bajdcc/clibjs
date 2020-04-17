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
console.log([].slice.call(obj,0).slice(1));