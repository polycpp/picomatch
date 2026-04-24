# Port Checklist

Use this checklist as a phase tracker. Update each item explicitly.

Legend:

- `[ ]` not started
- `[-]` in progress
- `[x]` done

## Intake

- [x] target repo scaffolded
- [x] GitHub repo created under `polycpp`
- [x] GitHub repo visibility verified private
- GitHub repo URL: https://github.com/polycpp/picomatch
- GitHub repo private: yes
- [x] baseline readiness passed
- [x] upstream repo cloned or updated locally
- [x] upstream revision recorded in `docs/research.md`

## Upstream Analysis

- [x] repo layout inspected
- [x] entry points identified
- [x] important implementation files listed
- [x] test directories identified
- [x] implementation risks recorded

## Existing Companion Alignment

- [x] existing companion repos inspected for conventions
- [x] CMake target and alias pattern aligned or deviation recorded
- [x] public header layout planned
- [x] detail/private header strategy planned
- [x] aggregator header strategy decided
- [x] example strategy recorded
- [x] README structure aligned with companion repos
- [x] deliberate deviations recorded in `docs/divergences.md`

## Dependency/API Analysis

- [x] dependency/API analysis tool run
- [x] direct dependencies classified
- [x] dependency ownership decisions recorded
- [x] dependency licenses classified
- [x] dependency license strategy recorded
- [x] GPL/AGPL/LGPL/MPL impacts reviewed
- [x] Node.js API usage reviewed
- [x] JavaScript API usage reviewed

## Scope

- [x] `v0` scope written in `docs/research.md`
- [x] deferred features listed in `docs/divergences.md`
- [x] `polycpp` module dependencies listed
- [x] missing `polycpp` primitives listed

## API Mapping

- [x] major public `v0` APIs mapped
- [x] direct vs adapted vs deferred status recorded
- [x] dynamic typing adaptations recorded

## Testing

- [x] unit test areas listed
- [x] integration test areas listed
- [x] upstream tests or fixtures to adapt listed
- [x] release-blocking behaviors listed

## Documentation Site

- [x] docs scaffold installed
- [x] docs build command recorded
- [x] GitHub Pages workflow present
- [x] `docs/build/` ignored
- [x] generated placeholder docs tracked for replacement before public release

## Pre-Coding Gate

- [x] `docs/research.md` is no longer placeholder-only
- [x] `docs/dependency-analysis.md` is no longer placeholder-only
- [x] `docs/api-mapping.md` is no longer placeholder-only
- [x] `docs/divergences.md` is no longer placeholder-only
- [x] `docs/test-plan.md` is no longer placeholder-only
- [x] strict readiness passed

## Implementation

- [x] implementation started after strict readiness
- [x] tests added as features are implemented
- [x] new divergences documented immediately

## Validation

- [x] targeted tests pass
- [x] README examples match actual code
- [x] remaining gaps are documented

## Public Release

- [x] production-grade quality confirmed
- [x] public documentation ready
- [x] generated docs placeholder pages replaced with real public documentation
- [x] `python3 docs/build.py` passes
- [x] third-party license notices complete
- [x] GitHub repo visibility changed to public
