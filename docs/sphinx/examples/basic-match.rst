Basic Match
===========

Source file: ``examples/basic_match.cpp``.

.. code-block:: cpp

   #include <iostream>
   #include <polycpp/picomatch/picomatch.hpp>

   int main() {
       auto is_source = polycpp::picomatch::matcher("src/**/*.cpp");
       std::cout << std::boolalpha << is_source("src/lib/picomatch.cpp") << '\n';
       std::cout << std::boolalpha << is_source("README.md") << '\n';
   }

Expected output:

.. code-block:: text

   true
   false
