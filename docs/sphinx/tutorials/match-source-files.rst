Match Source Files
==================

You will build a small source-file filter that includes C++ files under
``src/`` while excluding tests.

1. Create the options
---------------------

.. code-block:: cpp

   polycpp::picomatch::Options opts;
   opts.ignore = {"**/*.test.cpp"};

2. Build a reusable matcher
---------------------------

.. code-block:: cpp

   auto include_source = polycpp::picomatch::matcher("src/**/*.cpp", opts);

3. Apply it to paths
--------------------

.. code-block:: cpp

   include_source("src/app/main.cpp");      // true
   include_source("src/app/main.test.cpp"); // false
   include_source("README.md");             // false

Why this shape works
--------------------

The C++ API keeps the upstream matcher workflow but makes state explicit.
``Options`` stores ignore patterns and runtime choices, while ``Matcher`` is a
normal value object that can be reused in loops or stored in your own types.
