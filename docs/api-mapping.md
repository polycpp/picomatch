# API Mapping

| Upstream symbol | C++ symbol | Status | Notes |
|---|---|---|---|
| `picomatch(pattern, options)` | `polycpp::picomatch::matcher(pattern, options)` | adapted | Returns a typed `Matcher` object instead of a JavaScript closure. |
| `picomatch([patterns], options)` | `polycpp::picomatch::matcher(std::vector<std::string>, options)` | adapted | Multiple patterns are supported through an overload. |
| `picomatch.isMatch(str, patterns, options)` | `polycpp::picomatch::is_match(input, patterns, options)` | direct | One-shot boolean matching. |
| `picomatch.test(input, regex, options)` | `polycpp::picomatch::test(input, pattern, options)` | compatibility layer | Tests against compiled glob patterns, not JS `RegExp` objects. |
| `picomatch.matchBase(input, glob, options)` | `polycpp::picomatch::match_base(input, pattern, options)` | direct | Matches basename using the same options object. |
| `picomatch.scan(input, options)` | `polycpp::picomatch::scan(pattern, options)` | direct | Returns typed `ScanResult` and `ScanToken` values. |
| `picomatch.parse(pattern, options)` | `polycpp::picomatch::parse(pattern, options)` | adapted | Exposes pattern, expanded patterns, and generated matcher source metadata, not full upstream internal token state. |
| `picomatch.makeRe(pattern, options)` | `polycpp::picomatch::make_regex_source(pattern, options)` | adapted | Returns diagnostic regex-like source for supported patterns; matcher behavior does not depend on `std::regex`. |
| `picomatch.toRegex(source, options)` | no direct v0 equivalent | deferred | Direct JavaScript regex-source compilation is not portable to C++ regex engines. |
| `picomatch.constants` | `polycpp::picomatch::constants` | adapted | Expose relevant limits and POSIX class data as C++ constants. |
| `picomatch/posix` | `polycpp::picomatch::posix_matcher` and `posix_is_match` | compatibility layer | POSIX separator behavior is provided by explicit helpers. |
| `options.dot` | `Options::dot` | direct | Enables matching dot-prefixed path segments with wildcard tokens. |
| `options.basename` / `options.matchBase` | `Options::basename` / `Options::match_base` | direct | Both option names are represented for readability. |
| `options.windows` | `Options::windows` | direct | Explicit opt-in instead of upstream Node platform detection. |
| `options.ignore` | `Options::ignore` | direct | List of glob patterns used to suppress otherwise matching results. |
| `options.onMatch`, `onIgnore`, `onResult` | `Options::on_match`, `on_ignore`, `on_result` | adapted | `std::function<void(const MatchResult&)>` callbacks replace JS callback objects. |
| `options.expandRange` | no v0 custom callback | deferred | Simple built-in alphanumeric ranges are supported; custom callbacks are deferred. |
| full upstream parser state | no full v0 equivalent | deferred | Internal JS parser state is not exposed byte-for-byte. |
