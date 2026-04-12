# OLala — Claude Project Instructions

## Project Overview

OLala is a recursive descent parser framework in C++17 (LGPL v3). It provides composable grammar symbols, lookahead optimization, semantic value stacks, and Unicode input with multi-file source location tracking.

## Build & Test

CMake is in the CLion-bundled toolchain:
```
CMAKE=/home/staon/.local/share/JetBrains/Toolbox/apps/clion/bin/cmake/linux/x64/bin/cmake
```

```bash
# Configure (from project root)
$CMAKE -B cmake-build-debug

# Build
$CMAKE --build cmake-build-debug

# Run tests
cmake-build-debug/test/olala-test
```

Test framework: Google Test (fetched via FetchContent at build time). Only one test file currently: `test/symbolidentifier.cpp`.

## Project Layout

```
include/olala/    Public API headers
src/              Implementations
test/             Google Test tests
cmake/            Build utilities (addSources helper)
```

When adding a new class:
- Header in `include/olala/`, source in `src/`
- Add both to `CMakeLists.txt` via `addSources(olala ...)`

## Coding Conventions

**Naming:**
- Classes/types: `PascalCase`
- Methods: `camelCase`; private virtual dispatch hooks: `doMethodName()`
- Constants: `ALL_CAPS` (e.g. `EOS`)
- Namespace: `OLala` for public code, `Detail` for internal helpers

**File headers:** Every `.h` and `.cpp` starts with the standard LGPL copyright block (copyright `Ondrej Starek`).

**Header guards:** `OLALA_CLASSNAME_H`

**Includes:** Public API uses `#include <olala/header.h>` style.

**No copy:** Most classes delete copy ctor/assignment. Use `std::unique_ptr` / `std::shared_ptr`.

## Key Architecture

### Symbol Hierarchy (NVI pattern)
```
Symbol (abstract — public lookahead()/parse() delegate to doLookahead()/doParse())
  ├── SymbolTerminal       — lexical elements
  │   └── SymbolIdentifier — concrete C/C++ identifier
  └── SymbolNonTerminal    — composite rules
      ├── SymbolSequence   — all children in order
      ├── SymbolAlternation — first accepting child
      └── SymbolEpsilon    — zero-width production
```
Mix-in interfaces: `SymbolContainer` (add children), `SymbolActionable` (attach actions).

### Input pipeline
`InputStream` → `InputSequence` (PIMPL, stream stack, lookahead window) → `InputRange` → characters with `SourceLocation`.

`InputSequence::Impl` (defined only in `inputsequence.cpp`) holds: `stream_stack`, `window`, window offsets, skipper state.

### Source locations
`sourcelocation.h` / `sourcelocation.cpp` define `SourceLocation`, `SourceSpan`, `SourceRange` with no OLala dependencies — keep it that way.

`semanticstack.h` includes only `<olala/sourcelocation.h>` (not `inputsequence.h`) to avoid pulling in the heavy PIMPL header.

### Lookahead
Each symbol type has a `Detail::*LookaheadState` subclass in the `Detail` namespace. `lookahead()` returns a `LookaheadStatus` with a cached `LookaheadStatePtr`; `parse()` can accept that state to avoid re-parsing.

### Semantic stack
`SemanticStack` stores `(ValuePtr, SourceRange)` pairs. `Action = std::function<void(const ParserContext&)>` is attached via `SymbolActionable`.

### Adaptable
Type-safe downcasting without multiple inheritance: `adaptable.adaptTo<T>()` uses RTTI.

## Important Design Rules

- `inputsequence.h` must **not** include `<deque>`, `<vector>`, `<list>`, `<olala/symbolptr.h>` — those belong in `inputsequence.cpp` behind the Impl wall.
- `sourcelocation.h` must have **no** OLala dependencies.
- Private implementation helpers on `InputSequence` go inside `InputSequence::Impl` in the `.cpp`; only the four `do*` methods are declared as private members of `InputSequence` in the header.
- `pushStream()` must be called at a synchronization point (after a commit) — document this contract on any call site.
