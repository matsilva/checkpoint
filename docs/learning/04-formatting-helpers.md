# 04 — Formatting Helpers (Streams, fmt)

Goals
- Avoid stream manipulator state bugs
- Centralize formatting to keep output consistent

Read
- src/core/Token.cpp (fmt::format usage)
- src/logging/TrafficLogger.cpp

Key Ideas
- iostream state (std::hex, std::dec, setw, setfill) persists; reset or scope it
- Prefer helper functions for hex/binary formatting

Try This
1) Create helpers in a header (e.g., headers/Format.hpp):
   - to_hex(uint64_t v, int width) -> uppercase, zero-padded
   - to_bin(uint64_t v, int width)
2) Replace ad-hoc formatting in logging and handlers with the helpers.
3) Where using iostreams directly, wrap formatting in a small function to avoid state bleed.

Checklist
- [ ] No accidental hex/dec bleed between outputs
- [ ] Uniform padding for masks (2 for byte, 8 for 32-bit)
- [ ] Clear separation between formatting and business logic

