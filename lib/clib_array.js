Array.prototype.push = function () {
    var len = this.length || 0;
    for (var i in Array.prototype.slice.call(arguments)) {
        if (arguments.hasOwnProperty(i))
            this[len++] = arguments[i];
    }
    this.length = len;
    return this.length;
};
Array.prototype.slice = function (n) {
    n = n || 0;
    var len = this.length || 0;
    if (len <= n)
        return Array();
    var arr = Array();
    var j = 0;
    for (var i = n; i < len; i++) {
        if (this.hasOwnProperty(i))
            arr[j] = this[i];
        j=j+1;
    }
    arr.length = j;
    return arr;
};
Array.prototype.concat = function () {
    var unbox = (arr, o) => {
        if (o === null || typeof o === "undefined")
            return arr.push(o);
        if (o instanceof Number)
            return arr.push(Number(o));
        if (o instanceof String)
            return arr.push(String(o));
        if (o instanceof Boolean)
            return arr.push(Boolean(o));
        if (o instanceof Function)
            return arr.push(Function(o));
        if (o instanceof Array)
            return Array.prototype.push.apply(arr, o);
        return arr.push(Function(o));
    };
    var unbox2 = (arr, o) => {
        if (o instanceof Array)
            return Array.prototype.push.apply(arr, o);
        return arr.push(o);
    };
    var arr = Array();
    if (!(this.length)) {
        // failed
    } else {
        unbox(arr, this);
    }
    for (var i in Array.prototype.slice.call(arguments)) {
        if (arguments.hasOwnProperty(i))
            unbox2(arr, arguments[i]);
    }
    return arr;
};
Array.prototype.map = function (f) {
    var arr = Array(this.length);
    for (var i in this) {
        arr[i] = f(this[i]);
    }
    return arr;
};
Array.prototype.filter = function (f) {
    var arr = Array();
    for (var i in this) {
        if (f(this[i]))
            arr.push(this[i]);
    }
    return arr;
};
return;