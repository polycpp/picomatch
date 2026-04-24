# Test Plan

## Unit tests

- Literal matching, escaping, and path separator handling.
- `*`, `?`, and `**` behavior across single and multiple path segments.
- Dotfile behavior with `Options::dot` disabled and enabled.
- Brace expansion for comma lists, empty alternatives, numeric ranges, and alphabetic ranges.
- Bracket classes, negated bracket classes, and selected POSIX bracket classes.
- Extglobs `@(...)`, `?(...)`, `*(...)`, `+(...)`, and practical `!(...)` cases.
- `scan()` base/glob split, prefix, negation, flags, tokens, slashes, and parts.
- `parse()` summary and `make_regex_source()` diagnostic output.
- Ignore patterns and callback invocation order.
- Invalid input and excessive pattern length errors.

## Integration tests

- Reusable matcher object against a list of source paths.
- Multiple-pattern matching with first-match behavior.
- Basename matching for paths with directories.
- POSIX default matching versus explicit Windows separator matching.
- Ignore pattern filtering in a realistic source-file selection workflow.
- README examples compiled and exercised through tests.

## Compatibility tests adapted from upstream

- `test/api.picomatch.js`: public API basics, matcher behavior, `isMatch`, `test`, and callbacks.
- `test/api.posix.js`: POSIX default and manual Windows override behavior.
- `test/api.scan.js`: scan result shape and feature flags.
- `test/braces.js`: comma braces, ranges, escaped braces, and braces with slashes.
- `test/brackets.js` and `test/posix-classes.js`: bracket class behavior.
- `test/dotfiles.js`: dotfile matching defaults and `dot` option behavior.
- `test/globstars.js`: recursive segment behavior for `**`.
- `test/extglobs.js`: practical extglob behavior for common user-facing cases.
- `test/options.ignore.js`, `test/options.onMatch.js`, `test/options.onResult.js`: ignore and callback behavior adapted to typed callbacks.
- `test/slashes-windows.js` and `test/slashes-posix.js`: separator behavior.
- `test/malicious.js`: pathological pattern safety and recursion guards.

## Release-blocking behaviors

- `*.js` must not match `a/b.js` without basename or globstar behavior.
- `**/*.js` must match nested JavaScript files.
- wildcard patterns must not match leading-dot segments unless `Options::dot` is true or the pattern explicitly starts with `.`.
- `Options::windows` must treat `\` and `/` as separators.
- `Options::ignore` must suppress otherwise matching paths.
- basic braces, brackets, globstars, and extglobs must match the selected upstream compatibility cases.
- pathological extglob patterns must terminate without unbounded recursion.
- public headers must compile standalone through the test target.
