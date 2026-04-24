# Dependency and JavaScript API Analysis

Run `scripts/analyze-upstream-js.py` after upstream intake, then consume `.tmp/dependency-analysis.json` to fill this file.

- package: picomatch
- package version: 4.0.4
- package root: `/data/work/lib/picomatch/.tmp/upstream/picomatch`
- analyzer json: `.tmp/dependency-analysis.json`
- include dev dependencies: no
- dependency source install used: yes
- companion root checked: `/data/work/lib`

## Package entry metadata

- main: index.js
- module: none declared
- types: none declared
- exports: none declared
- bin: none declared

## Direct dependencies

- none detected

## Dependency ownership decisions

There are no direct runtime dependencies in upstream `package.json`, so all v0 runtime behavior is implemented inside this repo. Existing companion repos are not required for dependency ownership.

| Package | Kind | Requested | Installed | License | License evidence | License impact | License strategy | Affects repo license | Deps | Source files | Node API calls | JS API calls | Recommendation | Rationale |
|---|---|---|---|---|---|---|---|---|---:|---:|---:|---:|---|---|

## License impact summary

- upstream package license: MIT from package.json license field
- repo license decision: MIT, matching the permissive upstream package license and existing companion repo convention
- GPL/AGPL dependencies: none
- LGPL/MPL dependencies: none
- permissive dependencies requiring notices: upstream `picomatch` MIT notice only
- dev/test-only dependencies excluded from shipped artifacts: upstream dev dependencies `eslint`, `fill-range`, `gulp-format-md`, `mocha`, and `nyc` are not vendored, linked, or shipped
- dependency license notices to add to `THIRD_PARTY_LICENSES.md`: upstream `picomatch` MIT notice and source URL

## Transitive dependency summary

No production dependency graph exists. Analyzer output has an empty `directDependencies` array and empty `directDependencyRows` array.

## Runtime API usage

### Target package

- entry points analyzed: `index.js`, reachable `lib/picomatch.js`, `lib/parse.js`, `lib/scan.js`, `lib/constants.js`, and `lib/utils.js`
- source files analyzed: six runtime JavaScript files
- external imports seen from target: only local files under `lib/`

### Node.js API usage

- `process.platform`: used by upstream OS detection; C++ v0 replaces this with explicit `Options::windows`

### JavaScript API usage

- String operations: `slice`, `replace`, `trim`, `includes`, `lastIndexOf`, `repeat`, `toLowerCase`
- Array operations: `push`, `pop`, `map`, `some`, `slice`, `indexOf`, `unshift`, `Array.isArray`
- Regex operations: `RegExp.prototype.exec`, `RegExp.prototype.test`
- Other built-ins: `Object.assign`, `Math.min`

## Porting decisions

- Use a deterministic C++ matcher engine instead of depending on JavaScript regex semantics.
- Represent options as a typed `Options` struct.
- Represent match information as `MatchResult` instead of an untyped JS object.
- Represent `scan()` output as `ScanResult` with booleans and token metadata.
- Keep range expansion built in for simple alphanumeric ranges; defer user-provided range expansion callbacks.
- Keep all dependency license scope in this repo because no runtime third-party dependencies are used.

## Analyzer warnings

- none
