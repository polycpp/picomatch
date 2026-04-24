#pragma once

/**
 * @file picomatch.hpp
 * @brief Glob matching for path-like strings, adapted from npm picomatch.
 *
 * This companion library ports the core public matching surface of npm
 * picomatch to C++20. It provides reusable matchers, one-shot matching,
 * scan/parse diagnostics, ignore patterns, and explicit POSIX/Windows
 * separator handling.
 *
 * @see https://www.npmjs.com/package/picomatch
 * @since 0.1.0
 */

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace polycpp::picomatch {

namespace constants {
inline constexpr std::size_t max_length = 1024 * 64;
inline constexpr int default_max_extglob_recursion = 0;
} // namespace constants

struct MatchResult;

/**
 * @brief Options controlling glob matching behavior.
 * @since 0.1.0
 */
struct Options {
    bool dot = false;          ///< Allow wildcards to match dot-prefixed path segments.
    bool basename = false;     ///< Match against the basename when the pattern has no slash.
    bool match_base = false;   ///< Alias for basename, matching upstream option spelling.
    bool contains = false;     ///< Allow a pattern to match a substring of the input.
    bool nocase = false;       ///< Case-insensitive matching for ASCII input.
    bool windows = false;      ///< Treat both '/' and '\\' as path separators.
    bool nobrace = false;      ///< Treat brace syntax as literals.
    bool nobracket = false;    ///< Treat bracket classes as literals.
    bool noextglob = false;    ///< Treat extglob syntax as literals.
    bool noext = false;        ///< Alias for noextglob.
    bool noglobstar = false;   ///< Treat '**' as repeated single-segment stars.
    bool nonegate = false;     ///< Disable leading '!' negation.
    bool capture = false;      ///< Include the matched output in MatchResult::captures.
    std::size_t max_length = constants::max_length;
    int max_extglob_recursion = constants::default_max_extglob_recursion;
    std::vector<std::string> ignore;
    std::function<std::string(const std::string&)> format;
    std::function<void(const MatchResult&)> on_result;
    std::function<void(const MatchResult&)> on_match;
    std::function<void(const MatchResult&)> on_ignore;
};

/**
 * @brief Result object passed to callbacks and returned by test().
 * @since 0.1.0
 */
struct MatchResult {
    std::string glob;
    std::string input;
    std::string output;
    bool is_match = false;
    bool ignored = false;
    std::vector<std::string> captures;
};

/**
 * @brief Token returned by scan() when token collection is requested.
 * @since 0.1.0
 */
struct ScanToken {
    std::string value;
    std::size_t depth = 0;
    bool is_glob = false;
    bool is_globstar = false;
    bool is_prefix = false;
    bool negated = false;
};

/**
 * @brief Structural summary of a glob pattern.
 * @since 0.1.0
 */
struct ScanResult {
    std::string prefix;
    std::string input;
    std::size_t start = 0;
    std::string base;
    std::string glob;
    bool is_brace = false;
    bool is_bracket = false;
    bool is_glob = false;
    bool is_extglob = false;
    bool is_globstar = false;
    bool negated = false;
    bool negated_extglob = false;
    std::size_t max_depth = 0;
    std::vector<ScanToken> tokens;
    std::vector<std::size_t> slashes;
    std::vector<std::string> parts;
};

/**
 * @brief Parse summary for a glob pattern.
 * @since 0.1.0
 */
struct ParseResult {
    std::string input;
    bool negated = false;
    std::vector<std::string> expanded_patterns;
    std::string output;
};

/**
 * @brief Reusable matcher for one or more glob patterns.
 * @since 0.1.0
 */
class Matcher {
public:
    Matcher(std::string pattern, Options options = {});
    Matcher(std::vector<std::string> patterns, Options options = {});

    bool operator()(const std::string& input) const;
    MatchResult match(const std::string& input) const;

    const std::vector<std::string>& patterns() const noexcept { return patterns_; }
    const Options& options() const noexcept { return options_; }

private:
    std::vector<std::string> patterns_;
    Options options_;
};

/** @brief Create a reusable matcher for one pattern. */
Matcher matcher(std::string pattern, Options options = {});

/** @brief Create a reusable matcher for multiple patterns. */
Matcher matcher(std::vector<std::string> patterns, Options options = {});

/** @brief Create a reusable matcher with POSIX separator behavior forced on. */
Matcher posix_matcher(std::string pattern, Options options = {});

/** @brief Create a reusable multi-pattern matcher with POSIX separator behavior forced on. */
Matcher posix_matcher(std::vector<std::string> patterns, Options options = {});

/** @brief Return true when input matches pattern. */
bool is_match(const std::string& input, const std::string& pattern, const Options& options = {});

/** @brief Return true when input matches any pattern in patterns. */
bool is_match(const std::string& input, const std::vector<std::string>& patterns, const Options& options = {});

/** @brief Return true when input matches pattern with POSIX separator behavior. */
bool posix_is_match(const std::string& input, const std::string& pattern, const Options& options = {});

/** @brief Return true when input matches any pattern with POSIX separator behavior. */
bool posix_is_match(const std::string& input, const std::vector<std::string>& patterns, const Options& options = {});

/** @brief Match input and return a typed result object. */
MatchResult test(const std::string& input, const std::string& pattern, const Options& options = {});

/** @brief Match pattern against only the basename of input. */
bool match_base(const std::string& input, const std::string& pattern, const Options& options = {});

/** @brief Scan a pattern into base/glob metadata and feature flags. */
ScanResult scan(const std::string& pattern, const Options& options = {});

/** @brief Parse a pattern into a stable C++ summary. */
ParseResult parse(const std::string& pattern, const Options& options = {});

/** @brief Return a diagnostic regex-like source string for a supported pattern. */
std::string make_regex_source(const std::string& pattern, const Options& options = {});

} // namespace polycpp::picomatch

#include <polycpp/picomatch/detail/aggregator.hpp>
