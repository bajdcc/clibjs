Error.prototype.toString = function () {
    return this.__type__ + ": "+ this.message+ ", stacktrace:\n" + this.stack;
};
return;