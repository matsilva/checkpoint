// LEARNING: Include the header file for this class first
#include "Token.hpp"

// LEARNING: Include other headers needed for implementation
#include "Crypto.hpp" // For cryptographic operations

// LEARNING: External library - fmt provides Python-style string formatting
#include <fmt/format.h>

// LEARNING: Standard library for algorithms (std::count, etc.)
#include <algorithm>

// LEARNING: constexpr = compile-time constant, more efficient than #define
// const = the value can't change, uint64_t = unsigned 64-bit integer
constexpr const uint64_t TOKEN_VERSION = 1;

// LEARNING: Constructor implementation with initialization list
// Syntax: ClassName::ClassName(params) : member1(value1), member2(value2) { body }
// The part after : is called "member initializer list" - more efficient than assignment
CToken::CToken(const std::string& fingerprint, std::chrono::system_clock::time_point issued) : m_fingerprint(fingerprint), m_issued(issued) {
    // LEARNING: Call private member function to get formatted string
    std::string toSign = getSigString();

    // LEARNING: g_pCrypto is a global pointer (g_ prefix = global, p = pointer)
    // -> is used to access members through a pointer (vs . for direct access)
    m_sig = g_pCrypto->sign(toSign);

    // LEARNING: fmt::format is like Python's f-strings or C's printf but type-safe
    // {} are placeholders filled by the arguments
    m_fullCookie = fmt::format("{},{}", toSign, m_sig);

    // LEARNING: Set validity flag - this token was properly created
    m_valid = true;
}

// LEARNING: Second constructor - parses existing cookie string
// Only initializes m_fullCookie in the initializer list
CToken::CToken(const std::string& cookie) : m_fullCookie(cookie) {
    // LEARNING: Validate format - should have exactly 2 commas
    // std::count counts occurrences in a range [begin, end)
    // Early return pattern - exit early if validation fails
    if (std::count(cookie.begin(), cookie.end(), ',') != 2)
        return;

    // LEARNING: Check for dash (cookie.contains() is C++23, using find instead)
    // find() returns std::string::npos if not found
    if (cookie.find('-') == std::string::npos)
        return;

    // LEARNING: auto keyword - compiler deduces the type (here: size_t)
    // find() returns position of first occurrence
    auto dash = cookie.find('-');

    // LEARNING: Exception handling with try-catch
    // std::stoi converts string to integer, can throw if invalid
    try {
        // LEARNING: substr(start, length) extracts substring
        // Check if version matches expected TOKEN_VERSION
        if (std::stoi(cookie.substr(0, dash)) != TOKEN_VERSION)
            return;
    } catch (std::exception& e) {
        // LEARNING: Catch any standard exception and return (invalid cookie)
        // & means catch by reference (more efficient than by value)
        return;
    }

    // LEARNING: string_view is a lightweight, non-owning view of a string
    // More efficient than creating new string copies
    // {} is uniform initialization (C++11)
    std::string_view cookieData = std::string_view{cookie}.substr(dash + 1);

    // LEARNING: Finding delimiters to parse the cookie format
    auto firstComma = cookieData.find(',');
    auto lastComma  = cookieData.find_last_of(',');

    // LEARNING: Extract parts of the cookie
    // Note the aligned = signs for readability (code style)
    m_fingerprint = cookieData.substr(0, firstComma);
    m_sig         = cookieData.substr(lastComma + 1);

    // LEARNING: const auto - immutable variable with deduced type
    // Extract timestamp string (between the two commas)
    const auto tpStrMs = cookieData.substr(firstComma + 1, lastComma - firstComma - 1);

    // LEARNING: Parse timestamp from string to time_point
    try {
        // LEARNING: Complex type conversion chain:
        // 1. std::string{tpStrMs} - create string from string_view
        // 2. std::stoull - string to unsigned long long
        // 3. std::chrono::milliseconds() - create duration
        // 4. time_point() - construct time_point from duration
        m_issued = std::chrono::system_clock::time_point(std::chrono::milliseconds(std::stoull(std::string{tpStrMs})));
    } catch (std::exception& e) {
        // LEARNING: If parsing fails, return (leaves m_valid as false)
        return;
    }

    // LEARNING: Recreate the signature string to verify
    std::string toSign = getSigString();

    // LEARNING: Cryptographic verification - check if signature is valid
    // Sets m_valid based on whether the signature matches
    m_valid = g_pCrypto->verifySignature(toSign, m_sig);
}

// LEARNING: Getter methods - simple accessors that return member variables
// 'const' after function name = this method doesn't modify the object
std::string CToken::tokenCookie() const {
    return m_fullCookie;
}

std::string CToken::fingerprint() const {
    return m_fingerprint;
}

bool CToken::valid() const {
    return m_valid;
}

std::chrono::system_clock::time_point CToken::issued() const {
    return m_issued;
}

// LEARNING: Helper method to create consistent signature format
// Not const because it could be called during construction
std::string CToken::getSigString() {
    // LEARNING: Complex time conversion:
    // 1. time_since_epoch() - duration since Unix epoch (1970-01-01)
    // 2. duration_cast<milliseconds> - convert to milliseconds precision
    // 3. count() - get numeric value
    // Format: "VERSION-FINGERPRINT,TIMESTAMP_MS"
    return fmt::format("{}-{},{}", TOKEN_VERSION, m_fingerprint, std::chrono::duration_cast<std::chrono::milliseconds>(m_issued.time_since_epoch()).count());
}
