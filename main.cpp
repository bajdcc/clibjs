#include <fstream>
#include <sstream>
#include <iostream>
#include "cjs.h"

int main() {
    //std::ifstream file("..\\test\\jquery2.js");
    //std::ifstream file("..\\test\\jquery.min.js");
    //std::ifstream file("..\\test\\vue.js");
    //std::ifstream file("..\\test\\vue.min.js");
    const auto filename = R"(..\\test\\test_8.js)";
    char buf[256];
    snprintf(buf, sizeof(buf), "sys.exec_file(\"%s\");", filename);
    clib::cjs js;
    try {
        js.exec("<starter>", buf);
    } catch (const clib::cexception &e) {
        std::cout << e.message() << std::endl;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || input == "exit")
            break;
        try {
            js.exec("<stdin>", input);
        } catch (const clib::cexception &e) {
            std::cout << e.message() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}