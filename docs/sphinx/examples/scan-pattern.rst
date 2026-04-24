Scan Pattern
============

Source file: ``examples/scan_pattern.cpp``.

.. code-block:: cpp

   #include <iostream>
   #include <polycpp/picomatch/picomatch.hpp>

   int main() {
       auto result = polycpp::picomatch::scan("!./src/**/*.cpp");
       std::cout << "base=" << result.base << '\n';
       std::cout << "glob=" << result.glob << '\n';
       std::cout << "negated=" << std::boolalpha << result.negated << '\n';
   }

Expected output:

.. code-block:: text

   base=src
   glob=**/*.cpp
   negated=true
