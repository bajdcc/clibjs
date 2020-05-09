(function () {
    var i = 0;
    console.log(i);
    setTimeout(function a() {
        if (++i > 4) return;
        console.log(i);
        setTimeout(a, 1000);
    });
})();