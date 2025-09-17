#include "IPRange.hpp"

#include <algorithm>
#include <stdexcept>

#include "../debug/log.hpp"

CIP::CIP(const std::string& ip) {
    if (std::count(ip.begin(), ip.end(), '.') == 3)
        parseV4(ip);
    else if (std::count(ip.begin(), ip.end(), ':') >= 2)
        parseV6(ip);
    else
        Debug::die("Invalid IP: {}", ip);
}

void CIP::parseV4(const std::string& ip) {
    m_v6 = false;

    std::string_view curr;
    size_t           lastPos = 0;
    auto             advance = [&]() {
        size_t prev = lastPos ? lastPos + 1 : lastPos;
        lastPos     = ip.find('.', prev);

        if (lastPos == std::string::npos)
            curr = std::string_view{ip}.substr(prev);
        else
            curr = std::string_view{ip}.substr(prev, lastPos - prev);
    };

    for (size_t i = 0; i < 4; ++i) {
        advance();
        m_blocks.push_back(std::stoul(std::string{curr}));

        if (m_blocks.back() > 0xFF)
            Debug::die("Invalid ipv4 byte: {}", curr);
    }
}

void CIP::parseV6(const std::string& ip) {
    const auto COLONS = std::count(ip.begin(), ip.end(), ':');

    m_v6 = true;

    std::string_view curr;
    size_t           lastPos = 0;
    bool             first   = true;
    auto             advance = [&]() {
        size_t prev = !first ? lastPos + 1 : lastPos;
        lastPos     = ip.find(':', prev);

        if (lastPos == std::string::npos)
            curr = std::string_view{ip}.substr(prev);
        else
            curr = std::string_view{ip}.substr(prev, lastPos - prev);

        first = false;
    };

    for (size_t i = 0; i < 8; ++i) {
        advance();
        if (curr.empty()) {
            for (size_t j = 0; j < 8 - COLONS; ++j) {
                i++;
                m_blocks.push_back(0);
            }

            if (ip.find("::") == 0 || ip.rfind("::") == ip.length() - 2) {
                m_blocks.push_back(0);
                advance();
            } else
                i--;
            continue;
        } else
            m_blocks.push_back(std::stoul(std::string{curr}, nullptr, 16));

        if (m_blocks.back() > 0xFFFF)
            Debug::die("Invalid ipv6 byte: {}", curr);
    }
}

CIPRange::CIPRange(const std::string& range) {
    if (range.find('/') == std::string::npos)
        Debug::die("IP range {} has no subnet", range);

    m_subnet = std::stoul(range.substr(range.find('/') + 1));

    m_ip = CIP(range.substr(0, range.find('/')));
}

bool CIPRange::ipMatches(const CIP& ip) const {
    if (m_ip.m_v6 != ip.m_v6)
        return false;

    if (m_ip.m_v6)
        return ipMatchesV6(ip);
    return ipMatchesV4(ip);
}

// LEARNING: IPv4 CIDR subnet matching using bit manipulation
// Example: 192.168.1.0/24 means first 24 bits must match
bool CIPRange::ipMatchesV4(const CIP& ip) const {
    // LEARNING: Create subnet mask by bit shifting
    // 0xFFFFFFFF = 32 bits all set to 1: 11111111111111111111111111111111
    // << is LEFT SHIFT operator - shifts bits left, fills with 0s on right
    // Example: /24 subnet -> shift left by (32-24)=8 bits
    // Result: 11111111111111111111111100000000 = 0xFFFFFF00
    // This masks the first 24 bits (network portion)
    uint32_t rangeMask = 0xFFFFFFFF << (32 - m_subnet);

    // LEARNING: Combine 4 bytes into single 32-bit integer
    // IPv4: [byte0].[byte1].[byte2].[byte3] -> 32-bit integer
    // Example: 192.168.1.5
    //   byte0=192=0xC0, byte1=168=0xA8, byte2=1=0x01, byte3=5=0x05
    //
    // << 24 shifts byte0 to most significant position (bits 31-24)
    // << 16 shifts byte1 to bits 23-16
    // << 8  shifts byte2 to bits 15-8
    // << 0  keeps byte3 in bits 7-0 (no shift needed)
    //
    // Visual: [192      ][168      ][1        ][5        ]
    //         [11000000 ][10101000 ][00000001 ][00000101 ]
    //          ↑bits31-24 ↑bits23-16 ↑bits15-8  ↑bits7-0
    //
    // | is bitwise OR - combines all shifted bytes
    // Result: 0xC0A80105 = 3232235781 decimal
    uint32_t rangeIP = (((uint32_t)m_ip.m_blocks.at(0)) << 24) | // Move to bits 31-24
        (((uint32_t)m_ip.m_blocks.at(1)) << 16) |                // Move to bits 23-16
        (((uint32_t)m_ip.m_blocks.at(2)) << 8) |                 // Move to bits 15-8
        (((uint32_t)m_ip.m_blocks.at(3)) << 0);                  // Stay in bits 7-0

    // LEARNING: Convert incoming IP to 32-bit integer same way
    uint32_t incomingIP =
        (((uint32_t)ip.m_blocks.at(0)) << 24) | (((uint32_t)ip.m_blocks.at(1)) << 16) | (((uint32_t)ip.m_blocks.at(2)) << 8) | (((uint32_t)ip.m_blocks.at(3)) << 0);

    // LEARNING: Apply mask and compare
    // & is bitwise AND - keeps only bits where mask has 1s
    // Example: IP=192.168.1.5, Mask=/24 (0xFFFFFF00)
    //   IP & Mask = 0xC0A80105 & 0xFFFFFF00 = 0xC0A80100 (192.168.1.0)
    // This zeroes out the host portion, keeping only network portion
    // If both IPs have same network portion after masking, they're in same subnet
    return (rangeMask & rangeIP) == (rangeMask & incomingIP);
}

// LEARNING: IPv6 CIDR subnet matching - more complex due to 128-bit addresses
// IPv6 is 128 bits but we only have 64-bit integers, so we split into two parts
bool CIPRange::ipMatchesV6(const CIP& ip) const {
    // LEARNING: Create mask for LEFT half (first 64 bits) of IPv6
    // IPv6 has 8 blocks of 16 bits each: [block0:block1:block2:block3:block4:block5:block6:block7]
    // We split into: [block0:block1:block2:block3] and [block4:block5:block6:block7]
    //
    // Ternary operator: (condition ? true_value : false_value)
    // If subnet > 64: shift by 0 (all bits matter in left half)
    // If subnet <= 64: shift by (64 - subnet) to create partial mask
    // Example: /48 subnet -> shift left by (64-48)=16 bits
    // Result: 0xFFFFFFFFFFFF0000 (first 48 bits set)
    uint64_t rangeMaskLeft = 0xFFFFFFFFFFFFFFFF << (m_subnet > 64 ? 0 : 64 - m_subnet);

    // LEARNING: Combine first 4 blocks (16 bits each) into 64-bit integer
    // IPv6 block is 16 bits (0x0000 to 0xFFFF)
    // << 48 shifts block0 to bits 63-48 (most significant)
    // << 32 shifts block1 to bits 47-32
    // << 16 shifts block2 to bits 31-16
    // << 0  keeps block3 in bits 15-0
    //
    // Visual example: 2001:db8:85a3:0000:...
    // block0=0x2001 -> shift left 48 -> 0x2001000000000000
    // block1=0x0db8 -> shift left 32 -> 0x00000db800000000
    // block2=0x85a3 -> shift left 16 -> 0x0000000085a30000
    // block3=0x0000 -> shift left 0  -> 0x0000000000000000
    // OR together:                       0x20010db885a30000
    uint64_t rangeIPLeft = (((uint64_t)m_ip.m_blocks.at(0)) << 48) | // Bits 63-48
        (((uint64_t)m_ip.m_blocks.at(1)) << 32) |                    // Bits 47-32
        (((uint64_t)m_ip.m_blocks.at(2)) << 16) |                    // Bits 31-16
        (((uint64_t)m_ip.m_blocks.at(3)) << 0);                      // Bits 15-0

    uint64_t incomingIPLeft =
        (((uint64_t)ip.m_blocks.at(0)) << 48) | (((uint64_t)ip.m_blocks.at(1)) << 32) | (((uint64_t)ip.m_blocks.at(2)) << 16) | (((uint64_t)ip.m_blocks.at(3)) << 0);

    // LEARNING: Apply mask to left halves and compare
    // If they don't match, IPs are in different subnets
    if ((rangeMaskLeft & rangeIPLeft) != (rangeMaskLeft & incomingIPLeft))
        return false;

    // LEARNING: If subnet <= 64 bits, we're done (right half doesn't matter)
    if (m_subnet <= 64)
        return true;

    // LEARNING: Handle RIGHT half for subnets > 64 bits
    // For /96 subnet: we already checked 64 bits, need to check 32 more
    // Shift amount = 128 - 96 = 32 (to create mask for remaining bits)
    uint64_t rangeMaskRight = 0xFFFFFFFFFFFFFFFF << (128 - m_subnet);

    // LEARNING: Combine blocks 4-7 into second 64-bit integer
    uint64_t rangeIPRight = (((uint64_t)m_ip.m_blocks.at(4)) << 48) | // Bits 63-48 of right half
        (((uint64_t)m_ip.m_blocks.at(5)) << 32) |                     // Bits 47-32 of right half
        (((uint64_t)m_ip.m_blocks.at(6)) << 16) |                     // Bits 31-16 of right half
        (((uint64_t)m_ip.m_blocks.at(7)) << 0);                       // Bits 15-0 of right half

    uint64_t incomingIPRight =
        (((uint64_t)ip.m_blocks.at(4)) << 48) | (((uint64_t)ip.m_blocks.at(5)) << 32) | (((uint64_t)ip.m_blocks.at(6)) << 16) | (((uint64_t)ip.m_blocks.at(7)) << 0);

    // LEARNING: Apply mask to right halves and compare
    return (rangeMaskRight & rangeIPRight) == (rangeMaskRight & incomingIPRight);
}
