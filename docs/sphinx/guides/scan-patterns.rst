Inspect Patterns With Scan
==========================

Use ``scan()`` when you need to understand how a glob separates fixed base
path from glob syntax.

.. code-block:: cpp

   auto result = polycpp::picomatch::scan("!./src/**/*.cpp");

   // result.prefix == "!./"
   // result.base == "src"
   // result.glob == "**/*.cpp"
   // result.negated == true
   // result.is_globstar == true

``ScanResult`` is intentionally typed and stable. It is not a byte-for-byte
copy of upstream's internal JavaScript state, but it carries the fields users
need for diagnostics and planning.
