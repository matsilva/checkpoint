# 08 — Modern C++ Pitfalls (and how to avoid them)

This unit highlights common mistakes when building real apps in C++ today and gives repo‑anchored drills to build muscle memory.

1) Lifetime & Dangling
- Pitfall: Returning references/pointers/string_view to temporaries or destroyed state.
- Spot it: Any function that returns `const char*`, `std::string_view`, or a reference built from locals.
- Try this: Audit Token/Crypto methods for any returned views; prefer returning `std::string` or ensure the storage outlives the view.
- Guardrails: RAII wrappers, ASan (already enabled in Debug), avoid `new`/manual free.

2) Copy/Move Semantics & Slicing
- Pitfall: Unintended expensive copies; missing move constructors; object slicing when passing by value to base.
- Spot it: Large structs passed/returned by value; polymorphic types without virtual destructors.
- Try this: Mark non‑copyable types (logger, crypto context) `= delete` copy/assign; add `noexcept` move where ownership transfers.
- Guardrails: Use `std::move` at ownership boundaries; prefer `unique_ptr` to clarify transfer.

3) Const‑Correctness & Hidden Mutation
- Pitfall: Methods not marked `const` limit usage; `mutable` abused to mutate state without synchronization.
- Spot it: Getters or helper methods that don’t modify state but aren’t const.
- Try this: From Unit 2, const‑qualify getters; ensure thread safety if `mutable` is necessary.
- Guardrails: clang‑tidy checks (modernize/pass‑by‑value, readability‑const‑return‑type).

4) Signedness, Narrowing, and UB Shifts
- Pitfall: Shifting signed ints; left‑shifting into/sign bits; implicit narrowing (e.g., `uint32_t` → `uint8_t`).
- Spot it: Bit operations using `int`; mixed signed/unsigned comparisons.
- Try this: In IP/mask code, use explicit `uintN_t`; cast before shifts; add helper `to_hex/to_bin` for consistent widths.
- Guardrails: `-Wconversion` (optional, noisy), UBSan; prefer unsigned for bitwise.

5) Iterator Invalidation & Ranges
- Pitfall: Modifying containers while iterating; keeping iterators past mutations.
- Spot it: Loops that erase/push_back on the same container.
- Try this: Replace with index‑based loops or use `erase(it++)` idiom; prefer `std::erase_if` where available.
- Guardrails: Add tests around erase/update behavior.

6) Exception Safety & RAII
- Pitfall: Resource leaks on exceptions; throwing in destructors; not meeting basic/strong exception guarantees.
- Spot it: Manual resource mgmt across code that can throw (fmt, allocations, file IO).
- Try this: Wrap resources (FILE*, sockets) in RAII; avoid throwing destructors; use scope guards for multi‑step ops.
- Guardrails: ASan/UBSan; design functions to be commit/rollback safe.

7) I/O Formatting & Locale
- Pitfall: iostream state bleed (std::hex persists), locale affecting parsing/printing.
- Spot it: `std::cout << std::hex` then later decimal output wrong; ad‑hoc padding.
- Try this: Implement `Format.hpp` helpers; switch logging/printing to helpers or fmt.
- Guardrails: Scoped formatting helpers; prefer `fmt::format` for clarity.

8) Time & Randomness
- Pitfall: Using `system_clock` for deltas (subject to jumps); weak seeding; bias in RNG use.
- Spot it: `std::chrono::system_clock::now()` in durations; `rng() % n` bias if n not power of two.
- Try this: Use `steady_clock` for durations/timeouts; use uniform_int_distribution for ranges.
- Guardrails: Wrap time/rng behind small utilities.

9) Concurrency Footguns
- Pitfall: Data races, deadlocks, double‑checked locking without atomics, misuse of condition variables.
- Spot it: Shared maps/caches without locks; static singletons without thread‑safe init.
- Try this: Implement per‑IP rate limiter (Unit 7); use `std::mutex`/`std::shared_mutex`; tiny ring buffer logger with lock.
- Guardrails: TSAN (if available); minimize lock scope; prefer immutable/shared‑nothing where possible.

10) C Interop (OpenSSL, libc)
- Pitfall: Leaking/duplicate frees; incorrect ownership; using stack addresses after return; struct padding/alignment.
- Spot it: Any `malloc/free`, `OPENSSL_*_free`, raw buffers.
- Try this: Create custom deleters and `unique_ptr<T, Deleter>`; centralize interop in thin wrappers.
- Guardrails: RAII everywhere at boundaries; ASan; avoid aliasing violations.

11) Encoding/Parsing
- Pitfall: Assuming ASCII; unsafe string concatenation; partial reads/writes; unbounded inputs.
- Spot it: Header parsers; logging messages constructed from untrusted input.
- Try this: Sanitize log fields; clamp sizes; validate headers strictly.
- Guardrails: Fuzz parsers with small fuzz harnesses.

Repo‑Anchored Drills
- Add headers/Format.hpp with `to_hex/to_bin` and migrate a couple of call sites.
- In Crypto/Token, identify any ownership handoffs and annotate with move semantics.
- Implement a thread‑safe per‑IP rate limiter (global mutex first, then shard).
- Switch RNG in the bit game and any server random use to `uniform_int_distribution`.

Checklists
- [ ] No dangling references/views from temporaries
- [ ] Unsigned types for bitwise; explicit casts for shifts
- [ ] Strong/basic exception safety where resources are involved
- [ ] Thread‑safe access to shared state; no data races
- [ ] Consistent formatting via helpers; no iostream state bleed

