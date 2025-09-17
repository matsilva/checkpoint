#pragma once

// LEARNING: Standard library includes for common C++ types
#include <string>
#include <string_view>
#include <memory> // For smart pointers (unique_ptr, shared_ptr)
#include <vector> // Dynamic array container

// LEARNING: External library - OpenSSL for cryptographic operations
// EVP = EnVeloPe - OpenSSL's high-level cryptographic interface
#include <openssl/evp.h>

// LEARNING: Cryptography class handling signing, verification, and hashing
class CCrypto {
  public:
    // LEARNING: Constructor and Destructor
    // Constructor sets up the crypto keys
    CCrypto();
    // LEARNING: Destructor (~) - called when object is destroyed
    // Important for cleanup (RAII - Resource Acquisition Is Initialization)
    ~CCrypto();

    // LEARNING: Public cryptographic operations
    // SHA-256 hash function - creates fixed-size hash from input
    std::string sha256(const std::string& in);

    // LEARNING: Digital signature - proves authenticity using private key
    std::string sign(const std::string& in);

    // LEARNING: Signature verification - checks signature using public key
    bool verifySignature(const std::string& in, const std::string& sig);

  private:
    // LEARNING: OpenSSL key structure pointer
    // Raw pointer (not smart pointer) because OpenSSL is a C library
    // nullptr = C++11 null pointer (safer than NULL)
    EVP_PKEY* m_evpPkey = nullptr;

    // LEARNING: Private helper methods
    bool genKey();  // Generate new cryptographic key
    void readKey(); // Read existing key from file

    // LEARNING: Convert hex string to byte array
    // string_view = lightweight view without owning the string
    std::vector<uint8_t> toByteArr(const std::string_view& s);
};

// LEARNING: Global instance of CCrypto
// inline = definition in header (C++17 feature)
// unique_ptr = smart pointer that owns and manages the object
// Automatically deletes when goes out of scope (no memory leaks!)
// g_p prefix = global pointer
inline std::unique_ptr<CCrypto> g_pCrypto;