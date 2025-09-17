# 02 — Const Correctness

Goals
- Use const to express immutability and enable optimizations/safer APIs

Read
- src/core/Token.hpp/.cpp
- src/config/IPRange.hpp

Key Ideas
- Mark methods that do not modify the object as const
- Pass by const reference for large types; pass by value for small trivially copyable types
- Prefer constexpr where possible for compile-time constants

Try This
1) Review Token getters and helpers; mark them const if they don’t mutate members.
2) In IPRange.hpp, ensure parsing helpers don’t modify state; add const where appropriate.
3) Add const to function parameters that are inputs only (e.g., const std::string&).

Checklist
- [ ] Getters are const where appropriate
- [ ] Avoid unnecessary copies in function signatures
- [ ] No const_cast in codebase (except for API interop), or document why

