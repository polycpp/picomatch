picomatch
=========

**C++20 glob matching for path-like strings.**

``polycpp/picomatch`` ports the core matching workflow from npm
``picomatch`` into a typed C++ API: reusable matchers, one-shot checks,
scan/parse diagnostics, ignore patterns, dotfile rules, braces, brackets,
globstars, practical extglobs, and explicit POSIX/Windows separator behavior.

.. code-block:: cpp

   #include <polycpp/picomatch/picomatch.hpp>

   auto is_source = polycpp::picomatch::matcher("src/**/*.cpp");
   bool matched = is_source("src/lib/picomatch.cpp");

.. grid:: 2

   .. grid-item-card:: Familiar glob behavior
      :margin: 1

      Match ``*``, ``?``, ``**``, braces, brackets, practical extglobs,
      dotfiles, basenames, and ignore patterns.

   .. grid-item-card:: Explicit runtime choices
      :margin: 1

      Set ``Options::windows`` or use POSIX helpers instead of relying on
      Node platform detection.

   .. grid-item-card:: Typed C++ API
      :margin: 1

      JavaScript closures and callback objects become ``Matcher``,
      ``Options``, and ``MatchResult``.

   .. grid-item-card:: Tested against upstream behavior
      :margin: 1

      The test suite adapts selected upstream cases for API basics,
      globstars, braces, brackets, dotfiles, extglobs, ignores, and slashes.

Getting started
---------------

.. code-block:: cmake

   FetchContent_Declare(
       polycpp_picomatch
       GIT_REPOSITORY https://github.com/polycpp/picomatch.git
       GIT_TAG        master
   )
   FetchContent_MakeAvailable(polycpp_picomatch)
   target_link_libraries(my_app PRIVATE polycpp::picomatch)

:doc:`Installation <getting-started/installation>` | :doc:`Quickstart <getting-started/quickstart>` | :doc:`Tutorials <tutorials/index>` | :doc:`API reference <api/index>`

.. toctree::
   :hidden:
   :caption: Getting started

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :hidden:
   :caption: Tutorials

   tutorials/index

.. toctree::
   :hidden:
   :caption: How-to guides

   guides/index

.. toctree::
   :hidden:
   :caption: API reference

   api/index

.. toctree::
   :hidden:
   :caption: Examples

   examples/index
