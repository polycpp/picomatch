#pragma once

/**
 * @file detail/aggregator.hpp
 * @brief Inline implementation include point for polycpp/picomatch.
 * @since 0.1.0
 */

#include <polycpp/picomatch/picomatch.hpp>

#include <algorithm>
#include <cctype>
#include <limits>
#include <set>
#include <stdexcept>
#include <string_view>
#include <unordered_set>

namespace polycpp::picomatch::detail {

inline bool is_separator(char c) {
    return c == '/';
}

inline bool at_segment_start(const std::string& input, std::size_t index) {
    return index == 0 || (index <= input.size() && input[index - 1] == '/');
}

inline bool span_contains_hidden_segment(const std::string& input, std::size_t begin, std::size_t end) {
    for (std::size_t i = begin; i < end && i < input.size(); ++i) {
        if (input[i] == '.' && at_segment_start(input, i)) return true;
    }
    return false;
}

inline std::string normalize_input(std::string value, const Options& options) {
    if (options.windows) {
        std::replace(value.begin(), value.end(), '\\', '/');
    }
    if (options.nocase) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
    }
    return value;
}

inline char normalize_char(char ch, const Options& options) {
    if (options.windows && ch == '\\') return '/';
    if (options.nocase) return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return ch;
}

inline std::string basename(std::string value) {
    if (value.empty()) return value;
    while (value.size() > 1 && value.back() == '/') value.pop_back();
    const auto pos = value.find_last_of('/');
    return pos == std::string::npos ? value : value.substr(pos + 1);
}

inline bool has_unescaped_slash(const std::string& pattern) {
    bool escaped = false;
    for (char ch : pattern) {
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '/') return true;
    }
    return false;
}

inline bool is_glob_char(char ch) {
    switch (ch) {
    case '*': case '?': case '[': case ']': case '{': case '}': case '(': case ')': case '!': case '+': case '@':
        return true;
    default:
        return false;
    }
}

inline bool has_glob_syntax(const std::string& pattern, const Options& options) {
    bool escaped = false;
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        const char ch = pattern[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '*' || ch == '?') return true;
        if (!options.nobrace && ch == '{') return true;
        if (!options.nobracket && ch == '[') return true;
        if (!(options.noextglob || options.noext) && (ch == '@' || ch == '+' || ch == '?' || ch == '*' || ch == '!') && i + 1 < pattern.size() && pattern[i + 1] == '(') return true;
    }
    return false;
}

inline std::vector<std::string> split_top_level(const std::string& input, char delimiter) {
    std::vector<std::string> parts;
    std::string current;
    int braces = 0;
    int brackets = 0;
    int parens = 0;
    bool escaped = false;

    for (char ch : input) {
        if (escaped) {
            current.push_back(ch);
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            current.push_back(ch);
            escaped = true;
            continue;
        }
        if (ch == '[') ++brackets;
        else if (ch == ']' && brackets > 0) --brackets;
        else if (brackets == 0) {
            if (ch == '{') ++braces;
            else if (ch == '}' && braces > 0) --braces;
            else if (ch == '(') ++parens;
            else if (ch == ')' && parens > 0) --parens;
        }
        if (ch == delimiter && braces == 0 && brackets == 0 && parens == 0) {
            parts.push_back(current);
            current.clear();
            continue;
        }
        current.push_back(ch);
    }
    parts.push_back(current);
    return parts;
}

inline std::optional<std::pair<std::size_t, std::size_t>> first_brace(const std::string& pattern) {
    bool escaped = false;
    int depth = 0;
    std::size_t start = std::string::npos;
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        const char ch = pattern[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '{') {
            if (depth == 0) start = i;
            ++depth;
        } else if (ch == '}' && depth > 0) {
            --depth;
            if (depth == 0 && start != std::string::npos) return std::make_pair(start, i);
        }
    }
    return std::nullopt;
}

inline std::vector<std::string> range_values(const std::string& body) {
    const auto pos = body.find("..");
    if (pos == std::string::npos) return {};
    const std::string left = body.substr(0, pos);
    const std::string right = body.substr(pos + 2);
    if (left.empty() || right.empty()) return {};

    std::vector<std::string> values;
    const auto push_numeric = [&]() -> bool {
        auto is_int = [](const std::string& s) {
            std::size_t i = (s.size() > 1 && s[0] == '-') ? 1 : 0;
            if (i >= s.size()) return false;
            for (; i < s.size(); ++i) if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
            return true;
        };
        if (!is_int(left) || !is_int(right)) return false;
        int a = std::stoi(left);
        int b = std::stoi(right);
        if (std::abs(b - a) > 1000) return false;
        const int step = a <= b ? 1 : -1;
        const bool padded = (left.size() > 1 && left[0] == '0') || (right.size() > 1 && right[0] == '0');
        const std::size_t width = std::max(left.size(), right.size());
        for (int v = a;; v += step) {
            std::string out = std::to_string(v);
            if (padded && out.size() < width) out.insert(out.begin(), width - out.size(), '0');
            values.push_back(out);
            if (v == b) break;
        }
        return true;
    };
    if (push_numeric()) return values;

    if (left.size() == 1 && right.size() == 1 && std::isalnum(static_cast<unsigned char>(left[0])) && std::isalnum(static_cast<unsigned char>(right[0]))) {
        char a = left[0];
        char b = right[0];
        const int step = a <= b ? 1 : -1;
        for (char ch = a;; ch = static_cast<char>(ch + step)) {
            values.emplace_back(1, ch);
            if (ch == b) break;
        }
    }
    return values;
}

inline std::vector<std::string> expand_braces_once(const std::string& pattern, bool nobrace) {
    if (nobrace) return {pattern};
    const auto brace = first_brace(pattern);
    if (!brace) return {pattern};
    const auto [open, close] = *brace;
    const std::string body = pattern.substr(open + 1, close - open - 1);
    std::vector<std::string> alternatives = range_values(body);
    if (alternatives.empty()) {
        alternatives = split_top_level(body, ',');
        if (alternatives.size() == 1) return {pattern};
    }

    std::vector<std::string> out;
    const std::string prefix = pattern.substr(0, open);
    const std::string suffix = pattern.substr(close + 1);
    for (const auto& alt : alternatives) out.push_back(prefix + alt + suffix);
    return out;
}

inline std::vector<std::string> expand_braces(const std::string& pattern, const Options& options) {
    std::vector<std::string> current{pattern};
    for (int depth = 0; depth < 16; ++depth) {
        bool changed = false;
        std::vector<std::string> next;
        for (const auto& item : current) {
            auto expanded = expand_braces_once(item, options.nobrace);
            if (expanded.size() != 1 || expanded[0] != item) changed = true;
            next.insert(next.end(), expanded.begin(), expanded.end());
            if (next.size() > 1000) throw std::runtime_error("picomatch brace expansion limit exceeded");
        }
        current = std::move(next);
        if (!changed) break;
    }
    return current;
}

inline bool find_matching_paren(const std::string& pattern, std::size_t open, std::size_t& close) {
    int depth = 0;
    int bracket = 0;
    bool escaped = false;
    for (std::size_t i = open; i < pattern.size(); ++i) {
        const char ch = pattern[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '[') ++bracket;
        else if (ch == ']' && bracket > 0) --bracket;
        else if (bracket == 0) {
            if (ch == '(') ++depth;
            else if (ch == ')') {
                --depth;
                if (depth == 0) {
                    close = i;
                    return true;
                }
            }
        }
    }
    return false;
}

inline bool class_has_dot(const std::string& body) {
    bool escaped = false;
    for (char ch : body) {
        if (escaped) {
            if (ch == '.') return true;
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '.') return true;
    }
    return false;
}

inline std::size_t find_class_close(const std::string& pattern, std::size_t open) {
    bool escaped = false;
    for (std::size_t i = open + 1; i < pattern.size(); ++i) {
        const char ch = pattern[i];
        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '[' && i + 1 < pattern.size() && pattern[i + 1] == ':') {
            const auto posix_end = pattern.find(":]", i + 2);
            if (posix_end != std::string::npos) {
                i = posix_end + 1;
                continue;
            }
        }
        if (ch == ']') return i;
    }
    return std::string::npos;
}

inline bool posix_class_match(const std::string& name, char ch) {
    const unsigned char c = static_cast<unsigned char>(ch);
    if (name == "alnum") return std::isalnum(c);
    if (name == "alpha") return std::isalpha(c);
    if (name == "ascii") return c <= 0x7f;
    if (name == "blank") return ch == ' ' || ch == '\t';
    if (name == "cntrl") return std::iscntrl(c);
    if (name == "digit") return std::isdigit(c);
    if (name == "graph") return std::isgraph(c);
    if (name == "lower") return std::islower(c);
    if (name == "print") return std::isprint(c);
    if (name == "punct") return std::ispunct(c);
    if (name == "space") return std::isspace(c);
    if (name == "upper") return std::isupper(c);
    if (name == "word") return std::isalnum(c) || ch == '_';
    if (name == "xdigit") return std::isxdigit(c);
    return false;
}

inline bool match_class_body(const std::string& body, char ch, bool& explicit_dot) {
    std::size_t i = 0;
    bool negated = false;
    if (i < body.size() && (body[i] == '!' || body[i] == '^')) {
        negated = true;
        ++i;
    }

    bool matched = false;
    for (; i < body.size(); ++i) {
        char current = body[i];
        if (current == '\\' && i + 1 < body.size()) current = body[++i];
        if (current == '.' ) explicit_dot = true;

        if (current == '[' && i + 1 < body.size() && body[i + 1] == ':') {
            const auto end = body.find(":]", i + 2);
            if (end != std::string::npos) {
                const std::string name = body.substr(i + 2, end - i - 2);
                if (posix_class_match(name, ch)) matched = true;
                i = end + 1;
                continue;
            }
        }

        if (i + 2 < body.size() && body[i + 1] == '-') {
            char right = body[i + 2];
            if (right == '\\' && i + 3 < body.size()) {
                right = body[i + 3];
                i += 1;
            }
            if (current <= ch && ch <= right) matched = true;
            if (right == '.') explicit_dot = true;
            i += 2;
            continue;
        }

        if (current == ch) matched = true;
    }
    return negated ? !matched : matched;
}

struct MatchContext {
    const Options& options;
    int depth = 0;
};

inline bool match_here(const std::string& pattern, std::size_t pi, const std::string& input, std::size_t si, MatchContext ctx);

inline bool match_entire(const std::string& pattern, const std::string& input, const Options& options, int depth = 0) {
    if (pattern.size() > options.max_length || input.size() > options.max_length) {
        throw std::length_error("picomatch input exceeds maximum length");
    }
    return match_here(pattern, 0, input, 0, MatchContext{options, depth});
}

inline bool extglob_alt_matches(const std::string& alt, const std::string& candidate, const Options& options, int depth) {
    if (depth > 32) return false;
    return match_entire(alt, candidate, options, depth + 1);
}

inline bool match_extglob(char op, const std::string& body, const std::string& rest, const std::string& input, std::size_t si, MatchContext ctx) {
    if (ctx.depth > 32) return false;
    const auto alts = split_top_level(body, '|');
    const std::size_t segment_end = input.find('/', si) == std::string::npos ? input.size() : input.find('/', si);

    const auto alt_matches_span = [&](std::size_t end) {
        const std::string candidate = input.substr(si, end - si);
        for (const auto& alt : alts) {
            if (extglob_alt_matches(alt, candidate, ctx.options, ctx.depth + 1)) return true;
        }
        return false;
    };

    const auto rest_matches = [&](std::size_t end) {
        return match_here(rest, 0, input, end, MatchContext{ctx.options, ctx.depth + 1});
    };

    if (op == '@') {
        for (std::size_t end = si; end <= segment_end; ++end) if (alt_matches_span(end) && rest_matches(end)) return true;
        return false;
    }
    if (op == '?') {
        if (rest_matches(si)) return true;
        for (std::size_t end = si; end <= segment_end; ++end) if (alt_matches_span(end) && rest_matches(end)) return true;
        return false;
    }
    if (op == '!') {
        for (std::size_t end = si; end <= segment_end; ++end) {
            if (!alt_matches_span(end) && rest_matches(end)) return true;
        }
        return false;
    }

    const bool require_one = op == '+';
    std::set<std::pair<std::size_t, int>> seen;
    std::function<bool(std::size_t, int)> repeat = [&](std::size_t pos, int count) -> bool {
        if (!seen.insert({pos, count}).second) return false;
        if ((!require_one || count > 0) && rest_matches(pos)) return true;
        for (std::size_t end = pos + 1; end <= segment_end; ++end) {
            const std::string candidate = input.substr(pos, end - pos);
            bool any = false;
            for (const auto& alt : alts) {
                if (extglob_alt_matches(alt, candidate, ctx.options, ctx.depth + 1)) {
                    any = true;
                    break;
                }
            }
            if (any && repeat(end, count + 1)) return true;
        }
        return false;
    };
    return repeat(si, 0);
}

inline bool match_here(const std::string& pattern, std::size_t pi, const std::string& input, std::size_t si, MatchContext ctx) {
    if (ctx.depth > 64) return false;
    if (pi == pattern.size()) return si == input.size();

    const char raw = pattern[pi];
    const char ch = normalize_char(raw, ctx.options);

    if (raw == '\\') {
        if (pi + 1 >= pattern.size()) return si < input.size() && input[si] == '\\' && match_here(pattern, pi + 1, input, si + 1, ctx);
        const char literal = normalize_char(pattern[pi + 1], ctx.options);
        return si < input.size() && input[si] == literal && match_here(pattern, pi + 2, input, si + 1, ctx);
    }

    if (!(ctx.options.noextglob || ctx.options.noext) && (raw == '@' || raw == '?' || raw == '*' || raw == '+' || raw == '!') && pi + 1 < pattern.size() && pattern[pi + 1] == '(') {
        if (!ctx.options.dot && si < input.size() && input[si] == '.' && at_segment_start(input, si)) return false;
        std::size_t close = 0;
        if (find_matching_paren(pattern, pi + 1, close)) {
            const std::string body = pattern.substr(pi + 2, close - pi - 2);
            const std::string rest = pattern.substr(close + 1);
            return match_extglob(raw, body, rest, input, si, ctx);
        }
    }

    if (raw == '*') {
        const bool globstar = !ctx.options.noglobstar && pi + 1 < pattern.size() && pattern[pi + 1] == '*';
        if (!ctx.options.dot && si < input.size() && input[si] == '.' && at_segment_start(input, si)) return false;
        const std::size_t next_pi = globstar ? pi + 2 : pi + 1;
        if (globstar) {
            for (std::size_t end = si; end <= input.size(); ++end) {
                if (!ctx.options.dot && span_contains_hidden_segment(input, si, end)) continue;
                if (match_here(pattern, next_pi, input, end, ctx)) return true;
            }
            return false;
        }
        const std::size_t segment_end = input.find('/', si) == std::string::npos ? input.size() : input.find('/', si);
        for (std::size_t end = si; end <= segment_end; ++end) {
            if (match_here(pattern, next_pi, input, end, ctx)) return true;
        }
        return false;
    }

    if (raw == '?') {
        if (si >= input.size() || is_separator(input[si])) return false;
        if (!ctx.options.dot && input[si] == '.' && at_segment_start(input, si)) return false;
        return match_here(pattern, pi + 1, input, si + 1, ctx);
    }

    if (!ctx.options.nobracket && raw == '[') {
        const auto close = find_class_close(pattern, pi);
        if (close != std::string::npos && close > pi + 1) {
            if (si >= input.size() || is_separator(input[si])) return false;
            const std::string body = pattern.substr(pi + 1, close - pi - 1);
            bool explicit_dot = class_has_dot(body);
            if (!ctx.options.dot && input[si] == '.' && at_segment_start(input, si) && !explicit_dot) return false;
            if (!match_class_body(body, input[si], explicit_dot)) return false;
            return match_here(pattern, close + 1, input, si + 1, ctx);
        }
    }

    if (si >= input.size()) return false;
    if (ch == '/') return is_separator(input[si]) && match_here(pattern, pi + 1, input, si + 1, ctx);
    return input[si] == ch && match_here(pattern, pi + 1, input, si + 1, ctx);
}

inline bool raw_match_one(std::string pattern, std::string input, Options options) {
    input = normalize_input(std::move(input), options);
    if (options.nocase) {
        std::transform(pattern.begin(), pattern.end(), pattern.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
    }

    bool negated = false;
    if (!options.nonegate && pattern.size() > 1 && pattern[0] == '!' && !(pattern.size() > 1 && pattern[1] == '(')) {
        negated = true;
        pattern.erase(pattern.begin());
        if (pattern.rfind("./", 0) == 0) pattern.erase(0, 2);
    }

    const bool use_basename = (options.basename || options.match_base) && !has_unescaped_slash(pattern);
    std::string target = use_basename ? basename(input) : input;

    bool matched = false;
    for (const auto& expanded : expand_braces(pattern, options)) {
        if (options.contains) {
            for (std::size_t start = 0; start <= target.size() && !matched; ++start) {
                for (std::size_t end = start; end <= target.size(); ++end) {
                    if (match_entire(expanded, target.substr(start, end - start), options)) {
                        matched = true;
                        break;
                    }
                }
            }
        } else if (match_entire(expanded, target, options)) {
            matched = true;
        }
        if (matched) break;
    }
    return negated ? !matched : matched;
}

inline bool any_match(const std::vector<std::string>& patterns, const std::string& input, const Options& options) {
    for (const auto& pattern : patterns) {
        if (raw_match_one(pattern, input, options)) return true;
    }
    return false;
}

inline std::string regex_escape(char ch) {
    static const std::string special = R"(\.^$|()[]{}*+?)";
    if (special.find(ch) != std::string::npos) return std::string("\\") + ch;
    return std::string(1, ch);
}

inline std::string regex_source_for_pattern(const std::string& pattern, const Options& options) {
    std::string out;
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        const char ch = pattern[i];
        if (ch == '\\' && i + 1 < pattern.size()) {
            out += regex_escape(pattern[++i]);
        } else if (ch == '*' && i + 1 < pattern.size() && pattern[i + 1] == '*' && !options.noglobstar) {
            out += ".*";
            ++i;
        } else if (ch == '*') {
            out += "[^/]*";
        } else if (ch == '?') {
            out += "[^/]";
        } else if (ch == '/') {
            out += options.windows ? R"([/\\])" : "/";
        } else {
            out += regex_escape(ch);
        }
    }
    return options.contains ? out : "^(?:" + out + ")$";
}

inline bool segment_has_globstar(const std::string& segment) {
    return segment.find("**") != std::string::npos;
}

} // namespace polycpp::picomatch::detail

namespace polycpp::picomatch {

inline Matcher::Matcher(std::string pattern, Options options)
    : patterns_{std::move(pattern)}, options_(std::move(options)) {}

inline Matcher::Matcher(std::vector<std::string> patterns, Options options)
    : patterns_(std::move(patterns)), options_(std::move(options)) {}

inline MatchResult Matcher::match(const std::string& input) const {
    MatchResult result;
    result.glob = patterns_.empty() ? std::string{} : patterns_.front();
    result.input = input;
    result.output = options_.format ? options_.format(input) : input;
    result.is_match = detail::any_match(patterns_, result.output, options_);
    if (options_.capture && result.is_match) result.captures.push_back(result.output);

    if (options_.on_result) options_.on_result(result);

    if (result.is_match && !options_.ignore.empty()) {
        Options ignore_options = options_;
        ignore_options.ignore.clear();
        ignore_options.on_match = nullptr;
        ignore_options.on_ignore = nullptr;
        ignore_options.on_result = nullptr;
        if (detail::any_match(options_.ignore, result.output, ignore_options)) {
            result.is_match = false;
            result.ignored = true;
            if (options_.on_ignore) options_.on_ignore(result);
            return result;
        }
    }

    if (result.is_match && options_.on_match) options_.on_match(result);
    return result;
}

inline bool Matcher::operator()(const std::string& input) const {
    return match(input).is_match;
}

inline Matcher matcher(std::string pattern, Options options) {
    return Matcher(std::move(pattern), std::move(options));
}

inline Matcher matcher(std::vector<std::string> patterns, Options options) {
    return Matcher(std::move(patterns), std::move(options));
}

inline Matcher posix_matcher(std::string pattern, Options options) {
    options.windows = false;
    return Matcher(std::move(pattern), std::move(options));
}

inline Matcher posix_matcher(std::vector<std::string> patterns, Options options) {
    options.windows = false;
    return Matcher(std::move(patterns), std::move(options));
}

inline bool is_match(const std::string& input, const std::string& pattern, const Options& options) {
    return matcher(pattern, options)(input);
}

inline bool is_match(const std::string& input, const std::vector<std::string>& patterns, const Options& options) {
    return matcher(patterns, options)(input);
}

inline bool posix_is_match(const std::string& input, const std::string& pattern, const Options& options) {
    return posix_matcher(pattern, options)(input);
}

inline bool posix_is_match(const std::string& input, const std::vector<std::string>& patterns, const Options& options) {
    return posix_matcher(patterns, options)(input);
}

inline MatchResult test(const std::string& input, const std::string& pattern, const Options& options) {
    return matcher(pattern, options).match(input);
}

inline bool match_base(const std::string& input, const std::string& pattern, const Options& options) {
    Options next = options;
    next.basename = true;
    return is_match(input, pattern, next);
}

inline ScanResult scan(const std::string& pattern, const Options& options) {
    ScanResult result;
    result.input = pattern;
    std::string work = pattern;
    std::size_t start = 0;
    if (!options.nonegate && work.rfind("!", 0) == 0 && work.rfind("!(", 0) != 0) {
        result.negated = true;
        result.prefix = "!";
        work.erase(work.begin());
        start = 1;
    }
    if (work.rfind("./", 0) == 0) {
        result.prefix += "./";
        work.erase(0, 2);
        start += 2;
    }
    result.start = start;

    std::string current;
    std::vector<std::string> segments;
    for (std::size_t i = 0; i < work.size(); ++i) {
        if (work[i] == '/') {
            result.slashes.push_back(start + i);
            segments.push_back(current);
            current.clear();
        } else {
            current.push_back(work[i]);
        }
    }
    segments.push_back(current);

    std::size_t first_glob = segments.size();
    std::size_t depth = 0;
    if (!result.prefix.empty()) {
        result.tokens.push_back(ScanToken{result.prefix, 0, false, false, true, result.negated});
    }
    for (std::size_t i = 0; i < segments.size(); ++i) {
        const auto& segment = segments[i];
        const bool seg_glob = detail::has_glob_syntax(segment, options);
        const bool seg_globstar = detail::segment_has_globstar(segment);
        if (seg_glob && first_glob == segments.size()) first_glob = i;
        result.is_glob = result.is_glob || seg_glob;
        result.is_globstar = result.is_globstar || seg_globstar;
        result.is_brace = result.is_brace || (!options.nobrace && segment.find('{') != std::string::npos);
        result.is_bracket = result.is_bracket || (!options.nobracket && segment.find('[') != std::string::npos);
        result.is_extglob = result.is_extglob || segment.find("@(") != std::string::npos || segment.find("+(") != std::string::npos || segment.find("?(") != std::string::npos || segment.find("*(") != std::string::npos || segment.find("!(") != std::string::npos;
        result.negated_extglob = result.negated_extglob || segment.rfind("!(", 0) == 0;
        result.tokens.push_back(ScanToken{segment, seg_globstar ? std::numeric_limits<std::size_t>::max() : 1, seg_glob, seg_globstar, false, false});
        if (seg_globstar) depth = std::numeric_limits<std::size_t>::max();
        else if (depth != std::numeric_limits<std::size_t>::max()) ++depth;
    }
    result.max_depth = depth;
    result.parts = segments;

    if (first_glob == segments.size()) {
        result.base = work;
        result.glob.clear();
    } else {
        for (std::size_t i = 0; i < first_glob; ++i) {
            if (!result.base.empty()) result.base.push_back('/');
            result.base += segments[i];
        }
        for (std::size_t i = first_glob; i < segments.size(); ++i) {
            if (!result.glob.empty()) result.glob.push_back('/');
            result.glob += segments[i];
        }
    }
    return result;
}

inline ParseResult parse(const std::string& pattern, const Options& options) {
    ParseResult result;
    result.input = pattern;
    result.negated = !options.nonegate && pattern.rfind("!", 0) == 0 && pattern.rfind("!(", 0) != 0;
    result.expanded_patterns = detail::expand_braces(pattern, options);
    result.output = make_regex_source(pattern, options);
    return result;
}

inline std::string make_regex_source(const std::string& pattern, const Options& options) {
    const auto expanded = detail::expand_braces(pattern, options);
    if (expanded.size() == 1) return detail::regex_source_for_pattern(expanded.front(), options);
    std::string joined;
    for (std::size_t i = 0; i < expanded.size(); ++i) {
        if (i) joined += '|';
        joined += detail::regex_source_for_pattern(expanded[i], Options{options});
    }
    return options.contains ? joined : "^(?:" + joined + ")$";
}

} // namespace polycpp::picomatch
