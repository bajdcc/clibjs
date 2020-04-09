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
    //std::ifstream file("..\\test\\test_2.js");
    //std::ifstream file("..\\test\\test_3.js");
    std::ifstream file("..\\test\\test_4.js");
    clib::cjs js;
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto str = buffer.str();
        try {
            js.exec("output.txt", str);
        } catch (const clib::cexception &e) {
            std::cout << e.message() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() || input == "exit")
            break;
        try {
            js.exec("console.txt", input);
        } catch (const clib::cexception &e) {
            std::cout << e.message() << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}