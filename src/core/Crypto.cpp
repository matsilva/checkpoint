// LEARNING: Include own header first (best practice)
#include "Crypto.hpp"

// LEARNING: Project includes - use relative paths with ../
#include "../GlobalState.hpp"
#include "../config/Config.hpp"
#include "../debug/log.hpp"
#include "../helpers/FsUtils.hpp"

// LEARNING: Standard library includes
#include <filesystem> // C++17 filesystem operations
#include <vector>
#include <string_view>

// LEARNING: OpenSSL headers for different crypto operations
#include <openssl/evp.h> // High-level crypto functions
#include <openssl/pem.h> // PEM format (key storage format)
#include <openssl/err.h> // Error handling
#include <fmt/format.h>  // String formatting

// LEARNING: Compile-time constant for key filename
constexpr const char* KEY_FILENAME = "privateKey.key";

// LEARNING: Constructor - initializes crypto system
CCrypto::CCrypto() {
    // LEARNING: std::error_code - modern C++ error handling without exceptions
    std::error_code ec;

    // LEARNING: Check if private key file exists
    // filesystem::exists can set error_code instead of throwing
    if (!std::filesystem::exists(NFsUtils::dataDir() + "/" + KEY_FILENAME, ec) || ec) {
        Debug::log(LOG, "No private key, generating one.");

        // LEARNING: Generate new key if it doesn't exist
        if (!genKey()) {
            Debug::log(CRIT, "Couldn't generate a key.");
            Debug::die("Keygen failed"); // Fatal error - terminate program
        }
    } else {
        // LEARNING: C-style file operations (OpenSSL is a C library)
        // fopen returns FILE* pointer
        auto f = fopen((NFsUtils::dataDir() + "/" + KEY_FILENAME).c_str(), "r");

        // LEARNING: Read private key from PEM file
        // Parameters: file, pointer to store key, password callback, user data
        PEM_read_PrivateKey(f, &m_evpPkey, nullptr, nullptr);
        fclose(f); // Always close files!
    }

    // LEARNING: Validate that we have a key
    if (!m_evpPkey) {
        Debug::log(CRIT, "Couldn't read the key.");
        Debug::die("Key reading from openssl failed");
    }

    Debug::log(LOG, "Read private key");
}

// LEARNING: Destructor - cleans up resources (RAII pattern)
// Called automatically when object is destroyed
CCrypto::~CCrypto() {
    // LEARNING: Free OpenSSL resources if allocated
    // Always check pointer before freeing (defensive programming)
    if (m_evpPkey)
        EVP_PKEY_free(m_evpPkey); // OpenSSL's cleanup function
}

// LEARNING: Convert hex string to byte array
// Example: "48656c6c6f" -> [0x48, 0x65, 0x6c, 0x6c, 0x6f] ("Hello")
std::vector<uint8_t> CCrypto::toByteArr(const std::string_view& s) {
    std::vector<uint8_t> inAsHash;

    // LEARNING: reserve() pre-allocates memory for efficiency
    // Hex string has 2 chars per byte, so need half the size
    inAsHash.reserve(s.size() / 2);

    // LEARNING: Process hex string 2 characters at a time
    for (size_t i = 0; i < s.size(); i += 2) {
        // LEARNING: Convert 2 hex chars to one byte
        // substr creates substring, std::string{} converts string_view to string
        // stoi with base 16 converts hex string to integer
        uint8_t byte = std::stoi(std::string{s.substr(i, 2)}, nullptr, 16);

        // LEARNING: emplace_back constructs element in-place (efficient)
        // vs push_back which copies/moves
        inAsHash.emplace_back(byte);
    }
    return inAsHash;
}

// LEARNING: SHA-256 hash function - creates 256-bit (32-byte) hash
// Hash is one-way: can't recover original from hash
// Same input always produces same hash (deterministic)
std::string CCrypto::sha256(const std::string& in) {
    // LEARNING: Create message digest context (OpenSSL structure)
    // MD = Message Digest, CTX = Context
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
        return ""; // Failed to allocate

    // LEARNING: Initialize digest with SHA-256 algorithm
    // EVP_sha256() returns the SHA-256 algorithm structure
    if (!EVP_DigestInit(ctx, EVP_sha256())) {
        EVP_MD_CTX_free(ctx); // Clean up on error
        return "";
    }

    // LEARNING: Feed data to be hashed
    // c_str() gets C-style string (const char*)
    // Can call Update multiple times for large data
    if (!EVP_DigestUpdate(ctx, in.c_str(), in.size())) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    // LEARNING: Buffer for hash result
    // SHA-256 always produces 32 bytes (256 bits / 8)
    uint8_t buf[32];

    // LEARNING: Finalize hash and get result
    // Third parameter would receive hash length (we know it's 32)
    if (!EVP_DigestFinal(ctx, buf, nullptr)) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    // LEARNING: Convert binary hash to hex string
    std::stringstream ss;
    for (size_t i = 0; i < 32; ++i) {
        // LEARNING: {:02x} format - 2 digits, zero-padded, hexadecimal
        // Example: 0x0A becomes "0a", not just "a"
        ss << fmt::format("{:02x}", buf[i]);
    }

    // LEARNING: Free OpenSSL context (prevent memory leak)
    EVP_MD_CTX_free(ctx);

    return ss.str(); // Return hex string representation
}

// LEARNING: Generate a new cryptographic key pair
bool CCrypto::genKey() {
    // LEARNING: Create key generation context
    // ED25519 = modern elliptic curve algorithm (fast, secure, small keys)
    // Better than RSA for most uses - 32-byte keys vs 256+ for RSA
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);

    if (!ctx)
        return false;

    // LEARNING: Initialize key generation
    // Returns <= 0 on error (OpenSSL convention)
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    // LEARNING: Actually generate the key pair
    // Stores result in m_evpPkey (passed by reference with &)
    if (EVP_PKEY_keygen(ctx, &m_evpPkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    // LEARNING: Save private key to file in PEM format
    // PEM = Privacy Enhanced Mail (text format with -----BEGIN/END----- markers)
    auto f = fopen((NFsUtils::dataDir() + "/" + KEY_FILENAME).c_str(), "w");

    // LEARNING: Write private key
    // Parameters: file, key, cipher (null=no encryption), password, pw_len, callback, userdata
    // Note: In production, you'd encrypt the key with a password!
    PEM_write_PrivateKey(f, m_evpPkey, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(f);

    EVP_PKEY_CTX_free(ctx);

    return true;
}

std::string CCrypto::sign(const std::string& in) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
        return "";

    if (!EVP_DigestSignInit(ctx, nullptr, nullptr, nullptr, m_evpPkey)) {
        Debug::log(ERR, "CCrypto::sign: EVP_DigestSignInit: err {}", ERR_error_string(ERR_get_error(), nullptr));
        EVP_MD_CTX_free(ctx);
        return "";
    }

    size_t len = 0;

    if (!EVP_DigestSign(ctx, nullptr, &len, (const unsigned char*)in.c_str(), in.size())) {
        Debug::log(ERR, "CCrypto::sign: EVP_DigestSign: err {}", ERR_error_string(ERR_get_error(), nullptr));
        EVP_MD_CTX_free(ctx);
        return "";
    }

    if (len <= 0) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    std::vector<uint8_t> buf;
    buf.resize(len);

    if (!EVP_DigestSign(ctx, buf.data(), &len, (const unsigned char*)in.c_str(), in.size())) {
        Debug::log(ERR, "CCrypto::sign: EVP_DigestSign: err {}", ERR_error_string(ERR_get_error(), nullptr));
        EVP_MD_CTX_free(ctx);
        return "";
    }

    std::stringstream ss;
    for (size_t i = 0; i < buf.size(); ++i) {
        ss << fmt::format("{:02x}", buf[i]);
    }

    EVP_MD_CTX_free(ctx);

    return ss.str();
}

bool CCrypto::verifySignature(const std::string& in, const std::string& sig) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
        return false;

    if (!EVP_DigestVerifyInit(ctx, nullptr, nullptr, nullptr, m_evpPkey)) {
        Debug::log(ERR, "CCrypto::verifySignature: EVP_DigestVerifyInit: err {}", ERR_error_string(ERR_get_error(), nullptr));
        EVP_MD_CTX_free(ctx);
        return false;
    }

    auto sigAsArr = toByteArr(sig);

    int  ret = EVP_DigestVerify(ctx, sigAsArr.data(), sigAsArr.size(), (const unsigned char*)in.c_str(), in.size());

    if (ret == 1) {
        // match
        EVP_MD_CTX_free(ctx);
        return true;
    }

    if (ret == 0) {
        // no match
        EVP_MD_CTX_free(ctx);
        return false;
    }

    Debug::log(ERR, "CCrypto::verifySignature: EVP_DigestVerify: err {}", ERR_error_string(ERR_get_error(), nullptr));

    // invalid sig??
    EVP_MD_CTX_free(ctx);
    return false;
}
