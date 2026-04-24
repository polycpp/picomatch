# polycpp/picomatch

C++20 companion port of [picomatch](https://www.npmjs.com/package/picomatch) for the [polycpp](https://github.com/enricohuang/polycpp) ecosystem.

`picomatch` matches path-like strings against glob patterns. This port focuses on the core matching workflow: reusable matchers, one-shot matching, scan/parse diagnostics, ignore patterns, dotfile handling, braces, brackets, globstars, practical extglobs, and explicit POSIX/Windows separator behavior.

## Status

Port version: `0.1.0`

Initial port based on upstream version: `4.0.4`

Compatibility note:

- This repo does not imply full parity with upstream `picomatch`.
- Implemented and deferred behavior is tracked in `docs/research.md`, `docs/api-mapping.md`, and `docs/divergences.md`.

Implemented:

- `polycpp::picomatch::matcher`
- `polycpp::picomatch::is_match`
- `polycpp::picomatch::test`
- `polycpp::picomatch::match_base`
- `polycpp::picomatch::scan`
- `polycpp::picomatch::parse`
- `polycpp::picomatch::make_regex_source`
- POSIX helper APIs
- typed `Options`, `MatchResult`, `ScanResult`, and `ParseResult`

Deferred:

- Direct JavaScript `RegExp` object interoperability.
- Byte-for-byte upstream regex-source parity.
- Full upstream internal parser-state parity.
- Custom JavaScript `expandRange` callback parity.
- Exact Bash parity for every deeply nested/pathological negated extglob combination.

Known divergences:

- `matcher()` returns a typed C++ object instead of a JavaScript function closure.
- Runtime platform detection is explicit through `Options::windows` or POSIX helpers.
- Callback hooks receive typed `MatchResult` objects.

## Prerequisites

- C++20 compiler
- CMake 3.20+
- Ninja recommended

## Build

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DPOLYCPP_PICOMATCH_BUILD_TESTS=ON
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

## Usage

```cpp
#include <iostream>
#include <polycpp/picomatch/picomatch.hpp>

int main() {
    auto is_source = polycpp::picomatch::matcher("src/**/*.cpp");

    std::cout << std::boolalpha;
    std::cout << is_source("src/lib/picomatch.cpp") << '\n'; // true
    std::cout << is_source("README.md") << '\n';            // false
}
```

Common options:

```cpp
polycpp::picomatch::Options opts;
opts.dot = true;       // allow wildcards to match .env-style segments
opts.basename = true;  // match only the basename when the glob has no slash
opts.windows = true;   // treat both '/' and '\\' as separators
opts.ignore = {"*.test.cpp"};

bool ok = polycpp::picomatch::is_match("src/main.cpp", "**/*.cpp", opts);
```

## API

- `matcher(pattern, options)` creates a reusable matcher object.
- `is_match(input, pattern, options)` performs one-shot matching.
- `test(input, pattern, options)` returns a typed `MatchResult`.
- `match_base(input, pattern, options)` matches against only the basename.
- `scan(pattern, options)` returns base/glob metadata and feature flags.
- `parse(pattern, options)` returns a stable C++ parse summary.
- `make_regex_source(pattern, options)` returns diagnostic regex-like source.

## License

MIT License. See [LICENSE](LICENSE) and [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md).
