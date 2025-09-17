# 05 — Bits, Shifts, and Masks

Goals
- Practice shifting, masking, and subnet logic as used in IPRange

Read
- src/config/IPRange.hpp
- src/helpers/RequestUtils.cpp (IP extraction)

Key Ideas
- Left shift (<<) multiplies by 2^n; right shift (>>) divides (unsigned behavior differs from signed)
- Masking (x & mask) extracts fields; OR (|) composes values

Try This
1) Implement functions in IPRange: contains(CIP), overlaps(CIPRange). Handle /32 and /0 edge cases.
2) Add helpers to present an IP/mask in dotted decimal and hex. Show first differing octet when not matching.
3) Write a small debug route that accepts ip/cidr and returns JSON with masked network, broadcast, first/last hosts.

Checklist
- [ ] Correct masking for /8, /16, /24, /32
- [ ] Works for boundary addresses (network, broadcast)
- [ ] Clear diagnostic output for mismatches

