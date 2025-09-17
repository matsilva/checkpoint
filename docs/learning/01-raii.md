# 01 — RAII and Ownership

Goals

- Understand Resource Acquisition Is Initialization (RAII)
- Remove raw-memory/resource pitfalls by using smart pointers or wrappers

Read

- src/core/Crypto.cpp, src/core/Crypto.hpp
- src/core/Token.cpp, src/core/Token.hpp

Key Ideas

- Prefer automatic storage or smart pointers (unique_ptr/shared_ptr) over new/delete
- Encapsulate C resources (OpenSSL handles, FILE\*, sockets) behind RAII wrappers

Try This

1. Audit CCrypto for raw resource lifetimes. If any API returns raw buffers or requires free(), wrap in a small struct with a custom deleter.
2. In Token.cpp, ensure no function returns a pointer to temporary data. Prefer std::string return values or views that outlive use.
3. Add a small RAII wrapper example to src/helpers/ (e.g., FileGuard with fclose in destructor), even if not yet used.

Checklist

- [ ] No new/delete in business code paths
- [ ] No leaks reported by ASan on shutdown
- [ ] Clear ownership boundaries in headers
