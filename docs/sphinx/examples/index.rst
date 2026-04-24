Examples
========

Self-contained programs exercising the public API. Examples compile against
``polycpp::picomatch`` only; they do not include private headers.

.. toctree::
   :maxdepth: 1

   basic-match
   scan-pattern

Running an example
------------------

From the repository root:

.. code-block:: bash

   cmake -B build -G Ninja -DPOLYCPP_PICOMATCH_BUILD_EXAMPLES=ON
   cmake --build build --target basic_match
   ./build/examples/basic_match
