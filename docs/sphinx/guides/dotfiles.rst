Match Dotfiles
==============

By default, wildcard tokens do not match dot-prefixed path segments.

.. code-block:: cpp

   polycpp::picomatch::is_match(".env", "*"); // false

Set ``Options::dot`` when dotfiles should be included:

.. code-block:: cpp

   polycpp::picomatch::Options opts;
   opts.dot = true;
   polycpp::picomatch::is_match(".env", "*", opts); // true

Patterns that explicitly start with a dot still match without ``Options::dot``:

.. code-block:: cpp

   polycpp::picomatch::is_match(".env", ".*"); // true
