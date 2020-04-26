Array.prototype.map = function (f) {
    var arr = Array(this.length);
    for (var i in this) {
        arr[i] = f(this[i]);
    }
    return arr;
};
return;