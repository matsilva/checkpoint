# 06 — Pistache Basics (HTTP Server)

Goals
- Add simple routes and responses using Pistache

Read
- src/main.cpp
- src/core/Handler.hpp/.cpp

Key Ideas
- Router pattern: map HTTP verbs/paths to member functions
- Extracting headers, query params, and body
- Returning structured JSON responses (fmt/glaze)

Try This
1) Add routes:
   - GET /health → { status: "ok" }
   - POST /echo → echoes body and content-type
2) Add error helpers: return_bad_request(message), return_server_error(message)
3) Add a simple challenge route: GET /challenge → issues a challenge; POST /challenge → verifies token

Checklist
- [ ] Requests parsed safely, no crashes on malformed input
- [ ] Consistent JSON response envelope
- [ ] Clear logging for request and result

