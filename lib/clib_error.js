Error.prototype.toString = function () {
    return this.name + ": " + this.message + ", stacktrace:\n" + this.stack;
};
ReferenceError = function (message) {
    this.name = "ReferenceError";
    this.message = message || "";
};
ReferenceError.prototype = new Error();
sys.builtin(ReferenceError);
return;