# 07 — Concurrency & Synchronization

Goals

- Understand thread safety in modern C++ (C++20) and Pistache’s threading model
- Implement a simple, safe concurrency feature in this codebase

Read

- src/core/Handler.hpp/.cpp — request handling and where work runs
- src/logging/TrafficLogger.\* — any shared state and IO
- src/helpers — utilities that may be called from multiple threads

Key Ideas

- Pistache typically runs multiple worker threads. Handlers may be invoked concurrently.
- Protect shared mutable state (maps, caches) with std::mutex/std::shared_mutex or make them thread-local.
- Prefer RAII locks (std::lock_guard, std::unique_lock). Minimize lock scope. Avoid locking in logging hot-paths where possible.
- Use std::atomic for counters/flags and memory ordering when needed (relaxed for counters, seq_cst when in doubt for learning).

Try This

1. Per-IP rate limiter (learning version):
   - Add a map<ip, bucket> storing {tokens, last_refill}.
   - Guard it with std::mutex or std::shared_mutex.
   - On request, refill based on elapsed time, then consume 1 token; if empty, return 429.
   - Start simple (global mutex) then explore sharding by ip hash.

2. Thread-safe logging buffer:
   - Add an in-memory ring buffer for recent logs (fixed-size vector) protected by a mutex.
   - Expose a debug route to dump last N entries.

3. Tune Pistache workers:
   - Make worker count configurable and document the effect.

Hazards & Tips

- Deadlocks: always acquire locks in a fixed order; prefer one lock per operation.
- Contention: keep critical sections short; consider shared_mutex for read-heavy maps.
- IO under lock: avoid if possible; copy to local then write.
- Data races: treat any mutable global/singleton as suspect.

Stretch

- Use std::scoped_lock for multi-lock operations.
- Experiment with lock-free atomic counters for request metrics.
- Add benchmarks (even simple time-based loops) to compare approaches.

Checklist

- [ ] No data races (enable TSAN if possible for experiments)
- [ ] Rate limiting correctness under concurrent load
- [ ] Logging remains responsive under concurrency
