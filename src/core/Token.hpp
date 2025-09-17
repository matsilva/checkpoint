// LEARNING: #pragma once is a modern way to prevent header files from being included multiple times
// It's simpler than traditional include guards (#ifndef/#define/#endif)
#pragma once

// LEARNING: Standard library headers use angle brackets <>
// Your own headers use quotes ""
#include <string> // For std::string - C++'s safe string class (unlike C's char*)
#include <chrono> // For time handling - modern C++ way to work with time

// LEARNING: C++ classes combine data (member variables) and functions (methods)
// Convention: Class names often start with 'C' in some codebases
class CToken {
    // LEARNING: public section - accessible from outside the class
  public:
    // LEARNING: Constructor #1 - Creates a new token with given data
    // const& means "constant reference" - efficient way to pass strings without copying
    // std::chrono::system_clock::time_point is a type-safe way to represent time
    CToken(const std::string& fingerprint, std::chrono::system_clock::time_point issued);

    // LEARNING: Constructor #2 - Overloading: same name, different parameters
    // This one parses an existing token from a cookie string
    CToken(const std::string& cookie);

    // LEARNING: Member functions (methods) that retrieve data
    // 'const' at the end means these methods don't modify the object
    // This is called "const-correctness" - important C++ principle
    std::string                           tokenCookie() const;
    std::string                           fingerprint() const;
    bool                                  valid() const;
    std::chrono::system_clock::time_point issued() const;

    // LEARNING: private section - only accessible within the class
  private:
    // LEARNING: Helper method - internal functionality
    std::string getSigString();

    // LEARNING: Member variables (data stored in each object)
    // Convention: m_ prefix means "member variable"
    // Multiple variables can be declared on one line (same type)
    std::string                           m_sig, m_fingerprint, m_fullCookie;
    std::chrono::system_clock::time_point m_issued;

    // LEARNING: In-class initialization (C++11 feature)
    // Sets default value right in the declaration
    bool m_valid = false;
};
