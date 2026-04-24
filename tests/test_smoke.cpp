#include <gtest/gtest.h>

#include <polycpp/picomatch/picomatch.hpp>

#include <string>
#include <vector>

using namespace polycpp::picomatch;

TEST(PicomatchCoreTest, MatchesLiteralsAndStars) {
    EXPECT_TRUE(is_match("a.js", "*.js"));
    EXPECT_FALSE(is_match("a/b.js", "*.js"));
    EXPECT_TRUE(is_match("a/b.js", "**/*.js"));
    EXPECT_TRUE(is_match("a/b/c.js", "**/*.js"));
    EXPECT_FALSE(is_match("src/index.js", "**/*.test.js"));
    EXPECT_TRUE(is_match("src/index.test.js", "**/*.test.js"));
}

TEST(PicomatchCoreTest, RespectsDotfileRules) {
    EXPECT_FALSE(is_match(".env", "*"));
    EXPECT_FALSE(is_match("src/.env", "**"));
    Options opts;
    opts.dot = true;
    EXPECT_TRUE(is_match(".env", "*", opts));
    EXPECT_TRUE(is_match("src/.env", "**", opts));
    EXPECT_TRUE(is_match(".env", ".*"));
}

TEST(PicomatchCoreTest, SupportsBasenameMatching) {
    EXPECT_FALSE(is_match("foo/bar.js", "*.js"));
    Options opts;
    opts.basename = true;
    EXPECT_TRUE(is_match("foo/bar.js", "*.js", opts));
    EXPECT_TRUE(match_base("foo/bar.js", "*.js"));
}

TEST(PicomatchCoreTest, SupportsWindowsSeparatorsExplicitly) {
    EXPECT_FALSE(is_match("a\\b", "a/*"));
    Options opts;
    opts.windows = true;
    EXPECT_TRUE(is_match("a\\b", "a/*", opts));
    EXPECT_TRUE(is_match("a/b", "a/*", opts));
}

TEST(PicomatchCoreTest, SupportsBracesAndRanges) {
    EXPECT_TRUE(is_match("a/b", "a/{b,c}"));
    EXPECT_TRUE(is_match("a/c", "a/{b,c}"));
    EXPECT_FALSE(is_match("a/d", "a/{b,c}"));
    EXPECT_TRUE(is_match("a/2", "a/{1..3}"));
    EXPECT_TRUE(is_match("a/b", "a/{a..c}"));
    EXPECT_FALSE(is_match("a/d", "a/{a..c}"));
}

TEST(PicomatchCoreTest, SupportsBracketsAndPosixClasses) {
    EXPECT_TRUE(is_match("file1.js", "file[0-9].js"));
    EXPECT_FALSE(is_match("filea.js", "file[0-9].js"));
    EXPECT_TRUE(is_match("filea.js", "file[!0-9].js"));
    EXPECT_TRUE(is_match("a", "[[:alpha:]]"));
    EXPECT_FALSE(is_match("1", "[[:alpha:]]"));
}

TEST(PicomatchCoreTest, SupportsPracticalExtglobs) {
    EXPECT_TRUE(is_match("foo.md", "*.@(js|md)"));
    EXPECT_FALSE(is_match("foo.css", "*.@(js|md)"));
    EXPECT_TRUE(is_match("file.cc", "*.c?(c)"));
    EXPECT_FALSE(is_match("file.ccc", "*.c?(c)"));
    EXPECT_TRUE(is_match("foooo", "f+(o)"));
    EXPECT_TRUE(is_match("foo.txt", "*.!(js)"));
    EXPECT_FALSE(is_match("foo.js", "*.!(js)"));
}

TEST(PicomatchCoreTest, SupportsMultiplePatternsAndIgnore) {
    EXPECT_TRUE(is_match("a.test.js", std::vector<std::string>{"*.js", "*.md"}));
    Options opts;
    opts.ignore = {"*.test.js"};
    EXPECT_FALSE(is_match("a.test.js", "*.js", opts));
    EXPECT_TRUE(is_match("a.js", "*.js", opts));
}

TEST(PicomatchCoreTest, InvokesCallbacksWithTypedResult) {
    Options opts;
    int results = 0;
    int matches = 0;
    int ignores = 0;
    opts.ignore = {"skip.*"};
    opts.on_result = [&](const MatchResult& result) {
        ++results;
        EXPECT_EQ(result.input, "skip.js");
    };
    opts.on_match = [&](const MatchResult&) { ++matches; };
    opts.on_ignore = [&](const MatchResult& result) {
        ++ignores;
        EXPECT_TRUE(result.ignored);
    };
    EXPECT_FALSE(is_match("skip.js", "*.js", opts));
    EXPECT_EQ(results, 1);
    EXPECT_EQ(matches, 0);
    EXPECT_EQ(ignores, 1);
}

TEST(PicomatchApiTest, ScansPatternShape) {
    auto result = scan("!./foo/*.js", Options{});
    EXPECT_EQ(result.prefix, "!./");
    EXPECT_EQ(result.input, "!./foo/*.js");
    EXPECT_EQ(result.start, 3u);
    EXPECT_EQ(result.base, "foo");
    EXPECT_EQ(result.glob, "*.js");
    EXPECT_TRUE(result.is_glob);
    EXPECT_FALSE(result.is_extglob);
    EXPECT_TRUE(result.negated);
    ASSERT_GE(result.tokens.size(), 3u);
    EXPECT_TRUE(result.tokens[0].is_prefix);
    EXPECT_EQ(result.parts, (std::vector<std::string>{"foo", "*.js"}));
}

TEST(PicomatchApiTest, ParseAndRegexSourceAreDiagnostic) {
    auto parsed = parse("a/{b,c}/*.js");
    EXPECT_FALSE(parsed.negated);
    EXPECT_EQ(parsed.expanded_patterns.size(), 2u);
    EXPECT_FALSE(parsed.output.empty());
    auto source = make_regex_source("*.js");
    EXPECT_NE(source.find("[^/]*"), std::string::npos);
}

TEST(PicomatchApiTest, PosixHelpersDisableWindowsSeparators) {
    Options opts;
    opts.windows = true;
    EXPECT_TRUE(is_match("a\\b", "a/*", opts));
    EXPECT_FALSE(posix_is_match("a\\b", "a/*", opts));
}

TEST(PicomatchSafetyTest, RejectsOverlongInput) {
    Options opts;
    opts.max_length = 8;
    EXPECT_THROW(is_match("0123456789", "*", opts), std::length_error);
}
