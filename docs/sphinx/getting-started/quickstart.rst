Quickstart
==========

This example builds a reusable matcher for C++ source files and applies it to
several path-like strings.

Full example
------------

.. code-block:: cpp

   #include <iostream>
   #include <polycpp/picomatch/picomatch.hpp>

   int main() {
       auto is_source = polycpp::picomatch::matcher("src/**/*.cpp");

       std::cout << std::boolalpha;
       std::cout << is_source("src/lib/picomatch.cpp") << '\n';
       std::cout << is_source("README.md") << '\n';
   }

Compile it with the same CMake wiring from :doc:`installation`:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_PICOMATCH_BUILD_EXAMPLES=ON
   cmake --build build --target basic_match
   ./build/examples/basic_match

Expected output:

.. code-block:: text

   true
   false

What just happened
------------------

``matcher("src/**/*.cpp")`` created a reusable matcher. The ``**`` segment
matches through directories, while ``*.cpp`` still applies to the final path
segment.

Next steps
----------

- :doc:`../tutorials/match-source-files` shows a complete filtering workflow.
- :doc:`../guides/windows-separators` explains explicit Windows matching.
- :doc:`../api/index` lists every public type and function.
