#include <fstream>
#include <sstream>
#include <iostream>
#include "cjs.h"

int main() {
    std::ifstream file("..\\test\\jquery.js");
    //std::ifstream file("..\\test\\jquery.min.js");
    //std::ifstream file("..\\test\\vue.js");
    //std::ifstream file("..\\test\\vue.min.js");
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto str = buffer.str();
        clib::cjs js;
        js.exec(str);
    }
    return 0;
}