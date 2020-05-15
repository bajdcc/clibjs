#include <fstream>
#include <sstream>
#include <iostream>
#include "cjs.h"

int main() {
    const auto filename = ROOT_DIR R"(test/test.js)";
    char buf[256];
    snprintf(buf, sizeof(buf), "sys.exec_file(\"%s\");", filename);
    clib::cjs js;
    js.exec("<starter>", buf);
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || input == "exit")
            break;
        js.exec("<stdin>", input);
    }
    return 0;
}