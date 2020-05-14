Error.prototype.toString = function () {
    return this.name + ": "+ this.message+ ", stacktrace:\n" + this.stack;
};
return;