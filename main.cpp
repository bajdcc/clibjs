#include <fstream>
#include <sstream>
#include <iostream>
#include "cjs.h"

int main() {
    //std::ifstream file("..\\test\\jquery2.js");
    //std::ifstream file("..\\test\\jquery.min.js");
    //std::ifstream file("..\\test\\vue.js");
    //std::ifstream file("..\\test\\vue.min.js");
    //std::ifstream file("..\\test\\test_1.js");
    std::ifstream file("..\\test\\test_2.js");
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto str = buffer.str();
        clib::cjs js;
        try {
            js.exec("output.txt", str);
        } catch (const clib::cexception &e) {
            std::cout << e.message() << std::endl;
        }
    }
    return 0;
}