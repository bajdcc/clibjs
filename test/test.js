function test(n) {
    console.log("============ TEST #" + n + " ============");
    sys.exec_file("test_" + n + ".js");
}

test(6);
test(8);
test(9);
return;