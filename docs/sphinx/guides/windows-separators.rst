Use Windows Separators
======================

The upstream JavaScript entry point can inspect ``process.platform``. The C++
port does not perform runtime platform detection; choose separator behavior
explicitly.

.. code-block:: cpp

   polycpp::picomatch::Options opts;
   opts.windows = true;

   polycpp::picomatch::is_match("a\\b", "a/*", opts); // true
   polycpp::picomatch::is_match("a/b", "a/*", opts);  // true

For POSIX-only behavior, leave ``windows`` false or use the POSIX helpers:

.. code-block:: cpp

   polycpp::picomatch::posix_is_match("a\\b", "a/*"); // false
