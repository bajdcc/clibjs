Function.prototype.bind = function (context, ...args) {
    return function () {
        return context.apply(context, args);
    };
};
return;