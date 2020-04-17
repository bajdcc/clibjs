function A(a, b) {
    this.a = a;
    this.b = b;
    this.c = function () {
        return this.a + ' ' + this.b;
    };
}
var d = new A('123', 12.3);
return d.c();