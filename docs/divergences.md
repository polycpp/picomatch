# Divergences From Upstream

## Deferred Features

- Direct JavaScript `RegExp` object interoperability is deferred because C++ does not share JS regex object semantics.
- Byte-for-byte `makeRe()` output parity is deferred; `make_regex_source()` is diagnostic and matching is performed by the C++ matcher engine.
- Full `parse()` internal state parity is deferred; C++ v0 exposes a stable typed parse summary instead.
- Custom `expandRange` callback parity is deferred; simple built-in numeric and alphabetic brace ranges are supported.
- Exact Bash parity for deeply nested/pathological negated extglobs is deferred; v0 supports practical segment-level negated extglobs and documents this limit.
- Automatic Node platform detection is omitted; callers set `Options::windows` or use POSIX helpers explicitly.

## Deliberate Behavior Changes

- Matcher creation returns a `Matcher` object instead of a JavaScript function closure.
- Options are represented as a typed C++ `Options` struct.
- Callback hooks receive a typed `MatchResult` by const reference instead of a dynamic JavaScript object.
- Invalid input errors use C++ exceptions and `std::invalid_argument` / `polycpp::TypeError`-style behavior rather than JavaScript `TypeError` objects.
- Diagnostic regex source is not used as the executable matcher contract.

## Unsupported Runtime-Specific Features

- JavaScript object mutation and dynamic property attachment on returned matcher functions are not supported.
- JavaScript `navigator.platform` and `process.platform` auto-detection are not used.
- JavaScript-specific `RegExp` flags and match-array shapes are not exposed directly.
