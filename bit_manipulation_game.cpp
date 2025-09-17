// Bit Manipulation Learning Game
// Compile: g++ -std=c++17 bit_manipulation_game.cpp -o bitgame
// Run: ./bitgame

#include <iostream>
#include <iomanip>
#include <string>
#include <random>
#include <vector>
#include <bitset>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <limits>

class BitManipulationGame {
  private:
    int          score  = 0;
    int          level  = 1;   // gates difficulty/bit width
    int          streak = 0;
    std::mt19937 rng;

    // Modes
    enum class Mode { Practice, Learn, Quiz, Review, Sandbox };
    Mode currentMode = Mode::Practice;

    // Settings
    bool useColor = true;
    int  bitWidth = 8; // 8 → 16 → 32 based on level
    bool showMicroLessons = true; // for learn mode

    // Per-category stats for Review mode
    struct Stats { int correct = 0; int wrong = 0; };
    Stats statsHexToBin, statsBinToHex, statsShift, statsMask, statsSubnetMask, statsIPMatch, statsBitwise;

    // Color codes for terminal
    const std::string RESET  = "\033[0m";
    const std::string GREEN  = "\033[32m";
    const std::string RED    = "\033[31m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE   = "\033[34m";
    const std::string CYAN   = "\033[36m";
    const std::string BOLD   = "\033[1m";

  public:
    BitManipulationGame() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}

    // Helpers
    std::string colorize(const std::string& code, const std::string& s) const {
        return useColor ? (code + s + RESET) : s;
    }

    static std::string to_hex(uint64_t v, int width) {
        std::ostringstream oss;
        oss << std::uppercase << std::hex << std::setfill('0') << std::setw(width) << v;
        return oss.str();
    }

    static std::string to_bin(uint64_t v, int width) {
        std::string out(width, '0');
        for (int i = 0; i < width; ++i) {
            int bit = (v >> (width - 1 - i)) & 1ULL;
            out[i] = bit ? '1' : '0';
        }
        return out;
    }

    static std::string trim_spaces_underscores(std::string s) {
        s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
        s.erase(std::remove(s.begin(), s.end(), '_'), s.end());
        return s;
    }

    static bool is_binary_string(const std::string& s) {
        return std::all_of(s.begin(), s.end(), [](char c){ return c=='0' || c=='1'; });
    }

    static int hex_char_to_nibble(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        c = std::toupper(static_cast<unsigned char>(c));
        if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
        return -1;
    }

    static std::string dotted(uint32_t v) {
        std::ostringstream oss;
        oss << ((v >> 24) & 0xFF) << "." << ((v >> 16) & 0xFF) << "." << ((v >> 8) & 0xFF) << "." << (v & 0xFF);
        return oss.str();
    }

    static void print_bit_diff(const std::string& user, const std::string& correct) {
        if (user.size() != correct.size()) return;
        std::cout << "Your  : " << user << "\n";
        std::cout << "Expect: " << correct << "\n";
        std::cout << "       ";
        for (size_t i = 0; i < user.size(); ++i) std::cout << (user[i] == correct[i] ? ' ' : '^');
        std::cout << "\n";
    }

    void update_level() {
        if (streak >= 10) level = 3; else if (streak >= 5) level = 2; else level = 1;
        bitWidth = (level == 1 ? 8 : (level == 2 ? 16 : 32));
    }

    void apply_args(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--no-color" || arg == "--mono" || arg == "--theme=mono") useColor = false;
            else if (arg == "--learn") currentMode = Mode::Learn;
            else if (arg == "--practice") currentMode = Mode::Practice;
            else if (arg == "--quiz") currentMode = Mode::Quiz;
            else if (arg == "--review") currentMode = Mode::Review;
            else if (arg == "--sandbox") currentMode = Mode::Sandbox;
            else if (arg == "--seed" && i + 1 < argc) { unsigned s = std::stoul(argv[++i]); rng.seed(s); }
        }
    }

    void run(int argc, char** argv) {
        apply_args(argc, argv);
        update_level();
        printWelcome();

        while (true) {
            printMenu();
            std::string choice;
            std::cin >> choice;
            if (!std::cin) return; // EOF

            if (choice == "q" || choice == "Q") {
                std::cout << "\n" << colorize(CYAN, "Thanks for playing! Final score: ") << score << "\n";
                return;
            }
            if (choice == "r" || choice == "R") { showReference(); continue; }
            if (choice == "s" || choice == "S") { showScore(); continue; }

            int c = -1;
            try { c = std::stoi(choice); } catch (...) { c = -1; }

            switch (c) {
                case 1: hexToBinaryChallenge(); break;
                case 2: binaryToHexChallenge(); break;
                case 3: bitShiftChallenge(); break;
                case 4: bitMaskChallenge(); break;
                case 5: subnetMaskChallenge(); break;
                case 6: ipMatchingChallenge(); break;
                case 7: bitwiseOperatorChallenge(); break;
                case 8: showReference(); break;
                case 9: showScore(); break;
                case 10: reviewMode(); break;
                case 11: sandboxMode(); break;
                case 0: std::cout << "\n" << colorize(CYAN, "Thanks for playing! Final score: ") << score << "\n"; return;
                default: std::cout << colorize(RED, "Invalid choice!") << "\n";
            }
            update_level();
        }
    }

  private:
    void printWelcome() {
        std::cout << (useColor ? (BOLD + CYAN) : std::string()) << "\n";
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║     BIT MANIPULATION LEARNING GAME     ║\n";
        std::cout << "║        Master Bits & Networking        ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        if (useColor) std::cout << RESET;
        std::cout << "\n";
        std::cout << "Learn bit manipulation through interactive challenges!\n";
        std::cout << "Modes: Practice, Learn, Review, Sandbox. Shortcuts: q/r/s.\n";
        std::cout << "Based on the concepts from IPRange.cpp\n\n";
    }

    void printMenu() {
        std::cout << "\n" << (useColor ? BOLD : std::string()) << "═══ MAIN MENU ═══";
        if (useColor) std::cout << RESET;
        std::cout << "\n";
        std::cout << "Mode: " << (currentMode==Mode::Learn?"Learn": currentMode==Mode::Practice?"Practice": currentMode==Mode::Quiz?"Quiz": currentMode==Mode::Review?"Review":"Sandbox")
                  << ", Level: " << level << ", Width: " << bitWidth << " bits\n";
        std::cout << "1. " << colorize(YELLOW, "Hex to Binary") << " (Convert hex values)\n";
        std::cout << "2. " << colorize(YELLOW, "Binary to Hex") << " (Convert binary values)\n";
        std::cout << "3. " << colorize(YELLOW, "Bit Shifting") << " (Practice << and >>)\n";
        std::cout << "4. " << colorize(YELLOW, "Bit Masking") << " (Practice AND operations)\n";
        std::cout << "5. " << colorize(YELLOW, "Subnet Masks") << " (Create CIDR masks)\n";
        std::cout << "6. " << colorize(YELLOW, "IP Matching") << " (Check if IPs are in same subnet)\n";
        std::cout << "7. " << colorize(YELLOW, "Bitwise Operators") << " (AND, OR, XOR, NOT)\n";
        std::cout << "8. " << colorize(BLUE, "Show Reference") << "\n";
        std::cout << "9. " << colorize(GREEN, "Show Score") << "\n";
        std::cout << "10. Review (spaced practice)\n";
        std::cout << "11. Sandbox (explore)\n";
        std::cout << "0. " << colorize(RED, "Exit") << "\n";
        std::cout << "\nChoice: ";
    }

    void hexToBinaryChallenge() {
        std::cout << "\n" << colorize(BOLD+YELLOW, "=== HEX TO BINARY CHALLENGE ===") << "\n";
        std::cout << "Convert the hexadecimal value to binary (" << bitWidth << " bits).\n\n";

        // Show quick reference
        std::cout << BLUE << "Quick Reference:" << RESET << "\n";
        std::cout << "┌───┬──────┬──────┐ ┌───┬──────┬──────┐\n";
        std::cout << "│Hex│Binary│ Dec  │ │Hex│Binary│ Dec  │\n";
        std::cout << "├───┼──────┼──────┤ ├───┼──────┼──────┤\n";
        std::cout << "│ 0 │ 0000 │  0   │ │ 8 │ 1000 │  8   │\n";
        std::cout << "│ 1 │ 0001 │  1   │ │ 9 │ 1001 │  9   │\n";
        std::cout << "│ 2 │ 0010 │  2   │ │ A │ 1010 │  10  │\n";
        std::cout << "│ 3 │ 0011 │  3   │ │ B │ 1011 │  11  │\n";
        std::cout << "│ 4 │ 0100 │  4   │ │ C │ 1100 │  12  │\n";
        std::cout << "│ 5 │ 0101 │  5   │ │ D │ 1101 │  13  │\n";
        std::cout << "│ 6 │ 0110 │  6   │ │ E │ 1110 │  14  │\n";
        std::cout << "│ 7 │ 0111 │  7   │ │ F │ 1111 │  15  │\n";
        std::cout << "└───┴──────┴──────┘ └───┴──────┴──────┘\n\n";

        uint32_t          maxVal = (bitWidth == 8 ? 0xFFu : bitWidth == 16 ? 0xFFFFu : 0xFFFFFFFFu);
        uint32_t          value = rng() % (maxVal + 1ULL);
        std::stringstream ss;
        ss << std::hex << std::uppercase << value;
        std::string hexStr = ss.str();
        int hexDigits = bitWidth / 4;
        if ((int)hexStr.length() < hexDigits) hexStr = std::string(hexDigits - hexStr.length(), '0') + hexStr;

        std::cout << "Convert: " << colorize(CYAN, "0x" + hexStr) << " to binary\n";
        std::cout << colorize(GREEN, "Tip: Each hex digit = 4 bits.") << "\n";

        bool usedHint = false;
        if (currentMode == Mode::Learn) {
            // Step-by-step per nibble
            std::vector<std::string> nibbleBins;
            for (int i = 0; i < hexDigits; ++i) {
                char hc = hexStr[i];
                int  n  = hex_char_to_nibble(hc);
                std::string correctNibble = to_bin(n, 4);
                std::cout << "Nibble " << i+1 << " of " << hexDigits << " (hex " << hc << ") → binary (4 bits): ";
                std::string nb; std::cin >> nb; nb = trim_spaces_underscores(nb);
                if (nb == "h" || nb == "H") { usedHint = true; std::cout << colorize(YELLOW, "Hint: ") << correctNibble << "\n"; std::cout << "Enter nibble again: "; std::cin >> nb; nb = trim_spaces_underscores(nb); }
                if (!is_binary_string(nb) || (int)nb.size()!=4) {
                    std::cout << colorize(RED, "Expected 4 binary digits. Showing correct nibble.") << "\n";
                    nb = correctNibble;
                }
                if (nb != correctNibble) {
                    std::cout << colorize(RED, "Mismatch:") << "\n";
                    print_bit_diff(nb, correctNibble);
                } else {
                    std::cout << colorize(GREEN, "✓ Correct nibble!") << "\n";
                }
                nibbleBins.push_back(correctNibble);
            }
            std::string combined;
            for (auto& nb: nibbleBins) combined += nb;
            std::cout << "Combine all nibbles → " << combined << "\n";
            std::cout << colorize(CYAN, "Now enter the full binary (" + std::to_string(bitWidth) + " bits): ");
        } else {
            std::cout << "Your answer (" << bitWidth << " bits, e.g., 1010...): ";
        }

        std::string answer;
        std::cin >> answer;
        answer = trim_spaces_underscores(answer);

        std::string correct = to_bin(value, bitWidth);

        if (answer == correct) {
            int pts = 10;
            if (level >= 2) pts += 5; if (level >= 3) pts += 5;
            if (usedHint) pts -= 2;
            if (pts < 1) pts = 1;
            score += pts;
            streak++;
            statsHexToBin.correct++;
            std::cout << colorize(GREEN, "✓ Correct! ");
            std::cout << "0x" << hexStr << " = " << correct << "\n";
            std::cout << "Score: +" << pts << " (Total: " << score << ")\n";

            // Show breakdown
            std::cout << colorize(CYAN, "Breakdown:") << " ";
            for (int i=0;i<hexDigits;++i) {
                int n = hex_char_to_nibble(hexStr[i]);
                std::cout << hexStr[i] << "=" << to_bin(n,4) << (i+1<hexDigits?", ":"\n");
            }
        } else {
            streak = 0;
            statsHexToBin.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: ") << correct << "\n";
            print_bit_diff(answer, correct);
            std::cout << "Remember: Each hex digit = 4 bits\n";
        }
    }

    void binaryToHexChallenge() {
        std::cout << "\n" << colorize(BOLD+YELLOW, "=== BINARY TO HEX CHALLENGE ===") << "\n";
        std::cout << "Convert the binary value to hexadecimal (" << bitWidth << " bits).\n\n";

        // Show quick reference
        std::cout << BLUE << "Quick Reference:" << RESET << "\n";
        std::cout << "┌──────┬───┬─────┐ ┌──────┬───┬─────┐\n";
        std::cout << "│Binary│Hex│ Dec │ │Binary│Hex│ Dec │\n";
        std::cout << "├──────┼───┼─────┤ ├──────┼───┼─────┤\n";
        std::cout << "│ 0000 │ 0 │  0  │ │ 1000 │ 8 │  8  │\n";
        std::cout << "│ 0001 │ 1 │  1  │ │ 1001 │ 9 │  9  │\n";
        std::cout << "│ 0010 │ 2 │  2  │ │ 1010 │ A │ 10  │\n";
        std::cout << "│ 0011 │ 3 │  3  │ │ 1011 │ B │ 11  │\n";
        std::cout << "│ 0100 │ 4 │  4  │ │ 1100 │ C │ 12  │\n";
        std::cout << "│ 0101 │ 5 │  5  │ │ 1101 │ D │ 13  │\n";
        std::cout << "│ 0110 │ 6 │  6  │ │ 1110 │ E │ 14  │\n";
        std::cout << "│ 0111 │ 7 │  7  │ │ 1111 │ F │ 15  │\n";
        std::cout << "└──────┴───┴─────┘ └──────┴───┴─────┘\n\n";

        uint32_t    maxVal = (bitWidth == 8 ? 0xFFu : bitWidth == 16 ? 0xFFFFu : 0xFFFFFFFFu);
        uint32_t    value  = rng() % (maxVal + 1ULL);
        std::string binary = to_bin(value, bitWidth);

        std::cout << "Convert: " << colorize(CYAN, binary) << " to hex\n";
        std::cout << colorize(GREEN, "Tip: Split into 4-bit groups.") << "\n";

        bool usedHint = false;
        if (currentMode == Mode::Learn) {
            int hexDigits = bitWidth/4;
            std::string hexOut;
            for (int i = 0; i < hexDigits; ++i) {
                std::string group = binary.substr(i*4,4);
                int nib = (group[0]-'0')*8 + (group[1]-'0')*4 + (group[2]-'0')*2 + (group[3]-'0');
                char corr = (nib<10)?('0'+nib):('A'+(nib-10));
                std::cout << "Nibble " << i+1 << " (" << group << ") → hex: ";
                std::string ans; std::cin >> ans; if (ans=="h"||ans=="H") {usedHint=true; std::cout << colorize(YELLOW, "Hint: ") << corr << "\n"; std::cout << "Enter nibble again: "; std::cin >> ans; }
                if (ans.size()!=1 || hex_char_to_nibble(ans[0])<0) { std::cout << colorize(RED, "Expected single hex digit. Using correct.") << "\n"; ans = std::string(1, corr);} 
                char up = std::toupper(static_cast<unsigned char>(ans[0]));
                if (up != corr) {
                    std::cout << colorize(RED, std::string("Mismatch: expected ") + corr) << "\n";
                } else {
                    std::cout << colorize(GREEN, "✓ Correct nibble!") << "\n";
                }
                hexOut.push_back(corr);
            }
            std::cout << "Combine → 0x" << hexOut << "\n";
            std::cout << colorize(CYAN, "Now enter the full hex (no 0x, " + std::to_string(bitWidth/4) + " digits): ");
        } else {
            std::cout << "Your answer (without 0x prefix): ";
        }

        std::string answer;
        std::cin >> answer;

        // Convert answer to uppercase
        std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

        std::stringstream ss;
        ss << std::hex << std::uppercase << value;
        std::string correct = ss.str();
        int width = bitWidth/4;
        if ((int)correct.length() < width)
            correct = std::string(width - correct.length(), '0') + correct;

        if (answer == correct) {
            int pts = 10; if (level>=2) pts+=5; if(level>=3) pts+=5; if (usedHint) pts-=2; if (pts<1) pts=1;
            score += pts;
            streak++;
            statsBinToHex.correct++;
            std::cout << colorize(GREEN, "✓ Correct! ");
            std::cout << binary << " = 0x" << correct << "\n";
            std::cout << "Score: +" << pts << " (Total: " << score << ")\n";
        } else {
            streak = 0;
            statsBinToHex.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: 0x") << correct << "\n";
            std::cout << "Groups: ";
            for (int i=0;i<bitWidth;i+=4) {
                std::cout << binary.substr(i,4) << (i+4<bitWidth?" ":"\n");
            }
        }
    }

    void bitShiftChallenge() {
        std::cout << "\n" << colorize(BOLD+YELLOW, "=== BIT SHIFT CHALLENGE ===") << "\n";

        // Show quick reference
        std::cout << BLUE << "Quick Reference:" << RESET << "\n";
        std::cout << "• value << n: Multiply value by 2^n\n";
        std::cout << "  Examples: 5 << 1 = 5×2 = 10\n";
        std::cout << "           5 << 2 = 5×4 = 20\n";
        std::cout << "           5 << 3 = 5×8 = 40\n";
        std::cout << "• value >> n: Divide value by 2^n\n";
        std::cout << "  Examples: 20 >> 1 = 20÷2 = 10\n";
        std::cout << "           20 >> 2 = 20÷4 = 5\n";
        std::cout << "           20 >> 3 = 20÷8 = 2\n\n";

        uint32_t maxVal = (bitWidth==8?0xFFu:bitWidth==16?0xFFFFu:0xFFFFFFFFu);
        uint32_t value  = (rng() % (std::min<uint64_t>(maxVal, 255) + 1ULL)) + 1; // keep visual small
        int  shift     = (rng() % std::min(8, bitWidth/2)) + 1;  // 1.. reasonable
        bool leftShift = rng() % 2;

        std::cout << "Calculate: " << colorize(CYAN, "0x" + to_hex(value, (bitWidth/4>=2?2:1)));
        std::cout << " " << (leftShift ? "<<" : ">>") << " " << std::dec << shift << "\n";

        std::cout << "Binary: " << to_bin(value, bitWidth);
        std::cout << " " << (leftShift ? "<<" : ">>") << " " << shift << "\n";

        if (currentMode == Mode::Learn) {
            // Simple ASCII step visualization
            std::string b = to_bin(value, bitWidth);
            std::cout << colorize(CYAN, "Visualize shift (press Enter):") << "\n";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            if (leftShift) {
                std::string frame = b;
                for (int s=1; s<=shift; ++s) {
                    frame = frame.substr(1) + '0';
                    std::cout << frame << "\n";
                }
            } else {
                std::string frame = b;
                for (int s=1; s<=shift; ++s) {
                    frame = '0' + frame.substr(0, frame.size()-1);
                    std::cout << frame << "\n";
                }
            }
        }

        if (leftShift) {
            uint64_t decimal_result = static_cast<uint64_t>(value) * (1ULL << shift);
            std::cout << colorize(GREEN, "Math: ") << "0x" << to_hex(value, (bitWidth/4>=2?2:1)) << " (" << std::dec << value << ") × " << (1ULL << shift) << " = " << decimal_result << "\n";
        } else {
            uint64_t decimal_result = static_cast<uint64_t>(value) / (1ULL << shift);
            std::cout << colorize(GREEN, "Math: ") << "0x" << to_hex(value, (bitWidth/4>=2?2:1)) << " (" << std::dec << value << ") ÷ " << (1ULL << shift) << " = " << decimal_result << "\n";
        }

        std::cout << colorize(CYAN, "Decimal→Hex: ");
        std::cout << "0-9 stay the same, ";
        std::cout << "10→A, 11→B, 12→C, 13→D, 14→E, 15→F, ";
        std::cout << "16→10, 32→20, 255→FF" << RESET << "\n";
        std::cout << "Your answer in hex (without 0x): ";

        std::string answer;
        std::cin >> answer;
        std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

        uint64_t result = leftShift ? (static_cast<uint64_t>(value) << shift) : (static_cast<uint64_t>(value) >> shift);
        // Keep within bitWidth
        if (leftShift) {
            uint64_t mask = (bitWidth==64)?~0ULL:((1ULL<<bitWidth)-1ULL);
            result &= mask;
        }

        std::stringstream ss;
        ss << std::hex << std::uppercase << result;
        std::string correct = ss.str();
        int hW = std::max(2, bitWidth/4);
        if ((int)correct.length() < hW) correct = std::string(hW - correct.length(), '0') + correct;

        if (answer == correct) {
            score += 15;
            streak++;
            std::cout << colorize(GREEN, "✓ Correct! Result: 0x") << correct << "\n";
            std::cout << "Binary result: " << to_bin(result, bitWidth) << "\n";
            std::cout << "Score: +" << 15 << " (Total: " << score << ")\n";
            statsShift.correct++;
        } else {
            streak = 0;
            statsShift.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: 0x") << correct << "\n";
            std::cout << "Binary result: " << to_bin(result, bitWidth) << "\n";
            if (leftShift) {
                std::cout << "Remember: << shifts bits left, filling with 0s on right\n";
            } else {
                std::cout << "Remember: >> shifts bits right, discarding bits that fall off\n";
            }
        }
    }

    void bitMaskChallenge() {
        std::cout << "\n" << BOLD << YELLOW << "=== BIT MASK CHALLENGE ===" << RESET << "\n";
        std::cout << "Apply the AND mask to extract specific bits.\n\n";

        int value = rng() % 256;
        int mask  = rng() % 256;

        std::cout << "Calculate: " << colorize(CYAN, "0x" + to_hex(value,2) + " & 0x" + to_hex(mask,2)) << "\n";

        std::cout << "Binary: " << to_bin(value, 8);
        std::cout << " & " << to_bin(mask, 8) << "\n";

        std::cout << "Your answer in hex (without 0x): ";

        std::string answer;
        std::cin >> answer;
        std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

        int               result = value & mask;
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << result;
        std::string correct = ss.str();

        if (answer == correct) {
            score += 15;
            streak++;
            std::cout << colorize(GREEN, "✓ Correct! Result: 0x") << correct << "\n";
            std::cout << "Binary: " << to_bin(result, 8) << "\n";
            std::cout << "Score: +" << 15 << " (Total: " << score << ")\n";
            statsMask.correct++;
        } else {
            streak = 0;
            statsMask.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: 0x") << correct << "\n";
            std::cout << "Binary: " << to_bin(result, 8) << "\n";
            std::cout << "Remember: AND keeps only bits where both are 1\n";
        }
    }

    void subnetMaskChallenge() {
        std::cout << "\n" << BOLD << YELLOW << "=== SUBNET MASK CHALLENGE ===" << RESET << "\n";
        std::cout << "Create the subnet mask for the given CIDR notation.\n\n";

        std::vector<int> cidrs = {8, 16, 24, 28, 30};
        int              cidr  = cidrs[rng() % cidrs.size()];

        std::cout << "Create mask for: " << colorize(CYAN, "/" + std::to_string(cidr)) << " subnet\n";
        std::cout << "Formula: 0xFFFFFFFF << (32 - " << cidr << ")\n";

        std::cout << "Your answer in hex (8 hex digits, e.g., FFFFFF00): ";

        std::string answer;
        std::cin >> answer;
        std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

        uint32_t          mask = 0xFFFFFFFF << (32 - cidr);
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << mask;
        std::string correct = ss.str();

        if (answer == correct) {
            score += 20;
            streak++;
            statsSubnetMask.correct++;
            std::cout << colorize(GREEN, "✓ Correct! Mask: 0x") << correct << "\n";

            // Show as dotted decimal
            std::cout << "Dotted decimal: ";
            std::cout << ((mask >> 24) & 0xFF) << ".";
            std::cout << ((mask >> 16) & 0xFF) << ".";
            std::cout << ((mask >> 8) & 0xFF) << ".";
            std::cout << (mask & 0xFF) << "\n";

            std::cout << "Binary: " << to_bin(mask, 32) << "\n";
            std::cout << "Score: +" << 20 << " (Total: " << score << ")\n";
        } else {
            streak = 0;
            statsSubnetMask.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: 0x") << correct << "\n";
            std::cout << "Binary: " << to_bin(mask, 32) << "\n";
            std::cout << "Calculation: shift 0xFFFFFFFF left by " << (32 - cidr) << " bits\n";
        }
    }

    void ipMatchingChallenge() {
        std::cout << "\n" << BOLD << YELLOW << "=== IP SUBNET MATCHING CHALLENGE ===" << RESET << "\n";
        std::cout << "Determine if two IPs are in the same subnet.\n\n";

        // Generate random IPs
        uint8_t base[4] = {static_cast<uint8_t>(192), static_cast<uint8_t>(168), static_cast<uint8_t>(rng() % 256), static_cast<uint8_t>(rng() % 256)};

        uint8_t ip1[4] = {base[0], base[1], base[2], static_cast<uint8_t>(rng() % 256)};
        uint8_t ip2[4] = {base[0], base[1], base[2], static_cast<uint8_t>(rng() % 256)};

        // Sometimes make them in different subnets
        if (rng() % 3 == 0) {
            ip2[2] = static_cast<uint8_t>(rng() % 256);
        }

        int cidr = 24; // Keep it simple for the game

        std::cout << "IP 1: " << (useColor?CYAN:"");
        std::cout << (int)ip1[0] << "." << (int)ip1[1] << "." << (int)ip1[2] << "." << (int)ip1[3];
        std::cout << "/" << cidr << (useColor?RESET:"") << "\n";

        std::cout << "IP 2: " << (useColor?CYAN:"");
        std::cout << (int)ip2[0] << "." << (int)ip2[1] << "." << (int)ip2[2] << "." << (int)ip2[3];
        if (useColor) std::cout << RESET; std::cout << "\n";

        std::cout << "\nAre they in the same /" << cidr << " subnet? (y/n): ";

        char answer;
        std::cin >> answer;

        uint32_t mask  = 0xFFFFFFFFu << (32 - cidr);
        uint32_t addr1 = ((uint32_t)ip1[0] << 24) | ((uint32_t)ip1[1] << 16) | ((uint32_t)ip1[2] << 8) | ip1[3];
        uint32_t addr2 = ((uint32_t)ip2[0] << 24) | ((uint32_t)ip2[1] << 16) | ((uint32_t)ip2[2] << 8) | ip2[3];

        bool     sameSubnet = (addr1 & mask) == (addr2 & mask);

        if ((answer == 'y' || answer == 'Y') == sameSubnet) {
            score += 25;
            streak++;
            statsIPMatch.correct++;
            std::cout << colorize(GREEN, "✓ Correct!") << "\n";

            // Show the calculation
            std::cout << "\nCalculation:\n";
            std::cout << "Mask: 0x" << to_hex(mask,8) << " (/" << std::dec << cidr << ")\n";
            std::cout << "IP1 & Mask = 0x" << to_hex((addr1 & mask),8) << " = " << dotted(addr1 & mask) << "\n";
            std::cout << "IP2 & Mask = 0x" << to_hex((addr2 & mask),8) << " = " << dotted(addr2 & mask) << "\n";

            if (sameSubnet) {
                std::cout << "Both masked to same network: " << dotted(addr1 & mask) << "\n";
            } else {
                std::cout << "Different networks after masking!\n";
                // Show first differing octet
                uint32_t n1 = addr1 & mask, n2 = addr2 & mask;
                int o1 = (n1>>24)&0xFF, o2=(n2>>24)&0xFF;
                int o1b=(n1>>16)&0xFF, o2b=(n2>>16)&0xFF;
                int o1c=(n1>>8)&0xFF,  o2c=(n2>>8)&0xFF;
                int o1d=n1&0xFF,       o2d=n2&0xFF;
                std::vector<std::pair<int,int>> oct={{o1,o2},{o1b,o2b},{o1c,o2c},{o1d,o2d}};
                for (size_t i=0;i<oct.size();++i) if (oct[i].first!=oct[i].second) { std::cout << "First differing octet: index "<< (i+1) << " → "<<oct[i].first<<" vs "<<oct[i].second<<"\n"; break; }
            }

            std::cout << "Score: +" << 25 << " (Total: " << score << ")\n";
        } else {
            streak = 0;
            statsIPMatch.wrong++;
            std::cout << colorize(RED, "✗ Wrong!") << "\n";
            std::cout << "They are " << (sameSubnet ? "in the SAME" : "in DIFFERENT") << " subnet(s)\n";
        }
    }

    void bitwiseOperatorChallenge() {
        std::cout << "\n" << BOLD << YELLOW << "=== BITWISE OPERATOR CHALLENGE ===" << RESET << "\n";

        int                      a = rng() % 16; // 0-15 (4 bits)
        int                      b = rng() % 16;

        std::vector<std::string> ops     = {"&", "|", "^"};
        int                      opIndex = rng() % ops.size();
        std::string              op      = ops[opIndex];

        std::cout << "Calculate: " << CYAN << "0x" << std::hex << std::uppercase << a;
        std::cout << " " << op << " 0x" << b << RESET << "\n";

        std::cout << "Binary: " << std::bitset<4>(a).to_string();
        std::cout << " " << op << " " << std::bitset<4>(b).to_string() << "\n";

        std::cout << "Your answer in hex (single digit): ";

        std::string answer;
        std::cin >> answer;
        std::transform(answer.begin(), answer.end(), answer.begin(), ::toupper);

        int result;
        if (op == "&")
            result = a & b;
        else if (op == "|")
            result = a | b;
        else
            result = a ^ b;

        std::stringstream ss;
        ss << std::hex << std::uppercase << result;
        std::string correct = ss.str();

        if (answer == correct) {
            score += 10;
            streak++;
            statsBitwise.correct++;
            std::cout << colorize(GREEN, "✓ Correct! Result: 0x") << correct << "\n";
            std::cout << "Binary: " << std::bitset<4>(result).to_string() << "\n";
            std::cout << "Score: +" << 10 << " (Total: " << score << ")\n";

            // Explain the operation
            if (op == "&") {
                std::cout << "AND: Results in 1 only when both bits are 1\n";
            } else if (op == "|") {
                std::cout << "OR: Results in 1 when at least one bit is 1\n";
            } else {
                std::cout << "XOR: Results in 1 when bits are different\n";
            }
        } else {
            streak = 0;
            statsBitwise.wrong++;
            std::cout << colorize(RED, "✗ Wrong! The correct answer is: 0x") << correct << "\n";
            std::cout << "Binary: " << std::bitset<4>(result).to_string() << "\n";
        }
    }

    void showReference() {
        std::cout << "\n" << BOLD << CYAN << "=== QUICK REFERENCE ===" << RESET << "\n\n";

        std::cout << YELLOW << "Hex to Binary:" << RESET << "\n";
        std::cout << "  0 = 0000   4 = 0100   8 = 1000   C = 1100\n";
        std::cout << "  1 = 0001   5 = 0101   9 = 1001   D = 1101\n";
        std::cout << "  2 = 0010   6 = 0110   A = 1010   E = 1110\n";
        std::cout << "  3 = 0011   7 = 0111   B = 1011   F = 1111\n\n";

        std::cout << YELLOW << "Bit Shifting:" << RESET << "\n";
        std::cout << "  << : Shift left (multiply by 2^n)\n";
        std::cout << "  >> : Shift right (divide by 2^n)\n\n";

        std::cout << YELLOW << "Bitwise Operations:" << RESET << "\n";
        std::cout << "  & (AND): Both must be 1 → 1\n";
        std::cout << "  | (OR):  At least one 1 → 1\n";
        std::cout << "  ^ (XOR): Different bits → 1\n";
        std::cout << "  ~ (NOT): Flip all bits\n\n";

        std::cout << YELLOW << "Common Subnet Masks:" << RESET << "\n";
        std::cout << "  /8  = 0xFF000000 = 255.0.0.0\n";
        std::cout << "  /16 = 0xFFFF0000 = 255.255.0.0\n";
        std::cout << "  /24 = 0xFFFFFF00 = 255.255.255.0\n";
        std::cout << "  /32 = 0xFFFFFFFF = 255.255.255.255\n\n";

        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    void showScore() {
        std::cout << "\n" << colorize(BOLD+GREEN, "=== YOUR PROGRESS ===") << "\n";
        std::cout << "Score: " << score << " points\n";
        std::cout << "Current Streak: " << streak << " correct answers\n";

        if (score < 50) {
            std::cout << "Level: " << colorize(YELLOW, "Beginner") << " - Keep practicing!\n";
        } else if (score < 150) {
            std::cout << "Level: " << colorize(BLUE, "Intermediate") << " - Good progress!\n";
        } else if (score < 300) {
            std::cout << "Level: " << colorize(CYAN, "Advanced") << " - Excellent work!\n";
        } else {
            std::cout << "Level: " << (useColor?GREEN+ BOLD:std::string()) << "Bit Master!" << (useColor?RESET:"") << " - Outstanding!\n";
        }

        if (streak >= 5) {
            if (useColor) std::cout << BOLD; std::cout << "🔥 You're on fire! " << streak << " in a row!"; if (useColor) std::cout << RESET; std::cout << "\n";
        }

        std::cout << "\nCategory accuracy (correct/wrong):\n";
        std::cout << "Hex→Bin " << statsHexToBin.correct << "/" << statsHexToBin.wrong
                  << ", Bin→Hex " << statsBinToHex.correct << "/" << statsBinToHex.wrong
                  << ", Shift " << statsShift.correct << "/" << statsShift.wrong << "\n";
        std::cout << "Mask " << statsMask.correct << "/" << statsMask.wrong
                  << ", SubnetMask " << statsSubnetMask.correct << "/" << statsSubnetMask.wrong
                  << ", IPMatch " << statsIPMatch.correct << "/" << statsIPMatch.wrong
                  << ", Bitwise " << statsBitwise.correct << "/" << statsBitwise.wrong << "\n";
    }

    void reviewMode() {
        // Pick weakest category and run that challenge
        struct Item { int wrong; int correct; int id; };
        std::vector<Item> items = {
            {statsHexToBin.wrong, statsHexToBin.correct, 1},
            {statsBinToHex.wrong, statsBinToHex.correct, 2},
            {statsShift.wrong, statsShift.correct, 3},
            {statsMask.wrong, statsMask.correct, 4},
            {statsSubnetMask.wrong, statsSubnetMask.correct, 5},
            {statsIPMatch.wrong, statsIPMatch.correct, 6},
            {statsBitwise.wrong, statsBitwise.correct, 7},
        };
        std::sort(items.begin(), items.end(), [](const Item& a, const Item& b){
            // higher wrong-first; then lower correct
            if (a.wrong != b.wrong) return a.wrong > b.wrong;
            return a.correct < b.correct;
        });
        int pick = items.front().id;
        std::cout << "\nReviewing weakest area...\n";
        switch (pick) {
            case 1: hexToBinaryChallenge(); break;
            case 2: binaryToHexChallenge(); break;
            case 3: bitShiftChallenge(); break;
            case 4: bitMaskChallenge(); break;
            case 5: subnetMaskChallenge(); break;
            case 6: ipMatchingChallenge(); break;
            case 7: bitwiseOperatorChallenge(); break;
        }
    }

    void sandboxMode() {
        std::cout << "\nSandbox: choose an action\n";
        std::cout << "1) Hex→Binary  2) Binary→Hex  3) Shift visualizer\n";
        std::cout << "Choice: ";
        std::string c; std::cin >> c;
        if (c == "1") {
            std::string hx; std::cout << "Enter hex (no 0x): "; std::cin >> hx; std::transform(hx.begin(), hx.end(), hx.begin(), ::toupper);
            uint64_t v = std::stoull(hx, nullptr, 16);
            int w = std::max<int>(8, ((int)hx.size())*4);
            std::cout << "Binary("<<w<<"): " << to_bin(v, w) << "\n";
        } else if (c == "2") {
            std::string b; std::cout << "Enter binary: "; std::cin >> b; b = trim_spaces_underscores(b);
            uint64_t v = 0; for (char ch: b) v = (v<<1) | (ch=='1');
            int w = ((b.size()+3)/4);
            std::cout << "Hex: 0x" << to_hex(v, std::max(1,w)) << "\n";
        } else if (c == "3") {
            std::string b; int s; char dir; std::cout << "Enter binary: "; std::cin >> b; b=trim_spaces_underscores(b); std::cout << "Shift count: "; std::cin >> s; std::cout << "Direction (L/R): "; std::cin >> dir;
            std::cout << colorize(CYAN, "Frames:") << "\n";
            if (dir=='L'||dir=='l') { for (int i=0;i<s;++i){ b = b.substr(1) + '0'; std::cout << b << "\n"; } }
            else { for (int i=0;i<s;++i){ b = '0' + b.substr(0,b.size()-1); std::cout << b << "\n"; } }
        }
    }
};

int main(int argc, char** argv) {
    BitManipulationGame game;
    game.run(argc, argv);
    return 0;
}
