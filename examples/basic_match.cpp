#include <iostream>
#include <polycpp/picomatch/picomatch.hpp>

int main() {
    auto is_source = polycpp::picomatch::matcher("src/**/*.cpp");
    std::cout << std::boolalpha << is_source("src/lib/picomatch.cpp") << '\n';
    std::cout << std::boolalpha << is_source("README.md") << '\n';
}
