# 03 — Error Handling Strategy

Goals
- Choose and apply a consistent error handling approach across boundaries

Read
- src/core/Crypto.cpp/.hpp
- src/core/Token.cpp/.hpp
- src/core/Handler.cpp

Key Ideas
- At module boundaries, prefer status-return or expected-like types over exceptions
- Internals can use exceptions sparingly; translate them at boundaries
- Always return informative error messages (not just codes)

Try This
1) Add a small Result<T> or Expected<T> type (or use std::optional + error string) to Token parse/verify routines.
2) In Handler.cpp, translate failures to HTTP 4xx/5xx with JSON bodies that include a message and code.
3) Ensure Crypto failures don’t crash; return errors up the stack and log via Debug::log.

Checklist
- [ ] No unhandled exceptions escape request handlers
- [ ] Clear, structured error responses
- [ ] Unit-testable pure functions that signal errors without side effects

