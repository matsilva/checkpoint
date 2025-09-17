# Checkpoint Learning Roadmap

Purpose: Learn modern C++ by making targeted, real improvements to this repo. Each unit links to actual code and includes concrete “Try this” tasks.

How to use

- Follow units in order. Each is 30–60 minutes.
- Keep the app running in Debug with sanitizers enabled.
- After each unit, record confidence in `progress.md` (1–5).

Units

1. RAII and Ownership (docs/learning/01-raii.md)
2. Const Correctness (docs/learning/02-const-correctness.md)
3. Error Handling Strategy (docs/learning/03-error-handling.md)
4. Formatting Helpers (docs/learning/04-formatting-helpers.md)
5. Bits, Shifts, and Masks (docs/learning/05-bits-shifts.md)
6. Pistache Basics (docs/learning/06-pistache-basics.md)
7. Concurrency & Synchronization (docs/learning/07-concurrency.md)
8. Modern C++ Pitfalls (docs/learning/08-modern-cpp-pitfalls.md)

Advanced Topics (pick based on interest)

- Testing strategy (unit/integration) and harness
- Performance & profiling (perf/valgrind, flamegraphs)
- Security basics (input validation, time-based attacks, crypto hygiene)
- Robust networking (timeouts, backpressure, retries)
- Observability (logging levels, structure, rotation, metrics)
- Build tooling & CI (presets, static analysis, packaging)

Build Tips

- Configure debug build with sanitizers:
  - cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  - cmake --build build
  - (Optional) add -fsanitize=address,undefined to CMake for learning builds
