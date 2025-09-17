# Bit Manipulation Learning Game 🎮

An interactive CLI game to help you master bit manipulation concepts used in the checkpoint project!

## How to Play

### Compile

```bash
g++ -std=c++17 bit_manipulation_game.cpp -o bitgame
```

### Run

```bash
./bitgame
./bitgame --learn         # start in Learn mode
./bitgame --no-color      # disable ANSI colors
./bitgame --seed 123      # deterministic RNG
```

## Game Features

### Modes

- Learn: Guided, step-by-step with hints on demand
- Practice: Normal flow, scoring and streaks
- Review: Spaced practice focusing on weak areas
- Sandbox: Free-form conversions and shift visualizer

Keyboard shortcuts: `q` quit, `r` reference, `s` score

### 7 Challenge Types

1. **Hex to Binary** (10 points)
   - Convert hexadecimal values to binary
   - Example: 0xA5 → 10100101

2. **Binary to Hex** (10 points)
   - Convert binary values to hexadecimal
   - Example: 11001100 → CC

3. **Bit Shifting** (15 points)
   - Practice left shift (<<) and right shift (>>)
   - Example: 0x0F << 4 = 0xF0

4. **Bit Masking** (15 points)
   - Apply AND operations to extract bits
   - Example: 0xAB & 0xF0 = 0xA0

5. **Subnet Masks** (20 points)
   - Create CIDR subnet masks
   - Example: /24 → 0xFFFFFF00

6. **IP Matching** (25 points)
   - Check if two IPs are in the same subnet
   - Real-world networking practice!

7. **Bitwise Operators** (10 points)
   - Practice AND (&), OR (|), XOR (^)
   - Foundation for all bit manipulation

## Scoring System

- **Beginner**: 0-49 points
- **Intermediate**: 50-149 points
- **Advanced**: 150-299 points
- **Bit Master**: 300+ points

Track your streak of correct answers! 🔥

Mastery progression: as your streak grows, bit-width increases (8 → 16 → 32) and points scale. Hints give partial credit.

## Learning Tips

### Quick Conversions

- **Hex F = 1111** (all bits set)
- **Hex 0 = 0000** (no bits set)
- Each hex digit = 4 bits

### Remember

- **<<** multiplies by 2^n
- **>>** divides by 2^n
- **&** masks/filters bits
- **|** combines values

Diagnostic feedback: incorrect binary answers show a bit-diff (`^`) under mismatching positions. Subnet answers show masked networks in hex and dotted decimal and point to the first differing octet.

### Subnet Shortcuts

- **/8** = First byte matters (255.0.0.0)
- **/16** = First two bytes (255.255.0.0)
- **/24** = First three bytes (255.255.255.0)
- **/32** = Exact match (255.255.255.255)

## Connection to Checkpoint Project

This game teaches the exact concepts used in:

- `IPRange.cpp` - IP subnet matching
- `Token.cpp` - Hex encoding
- `Crypto.cpp` - Bitwise operations
- Network packet filtering

## Example Game Session

```
=== HEX TO BINARY CHALLENGE ===
Convert: 0x5A to binary
Your answer: 01011010
✓ Correct! Score: +10

=== SUBNET MASK CHALLENGE ===
Create mask for: /24 subnet
Your answer: FFFFFF00
✓ Correct! Mask: 0xFFFFFF00
Dotted decimal: 255.255.255.0
```

Learn mode example (Hex→Binary):

```
=== HEX TO BINARY CHALLENGE ===
Convert: 0xA5 to binary
Nibble 1 (A) → binary: 1010  ✓ Correct nibble!
Nibble 2 (5) → binary: 1011  Mismatch:
Your  : 1011
Expect: 0101
        ^  ^
Combine all nibbles → 10100101
Now enter the full binary (8 bits): 10100101
✓ Correct! Score: +8 (with hint/learn adjustments)
```

## Pro Tips

1. **Start with smaller numbers** to build intuition
2. **Draw bit patterns** on paper while learning
3. **Use the reference** (option 8) when stuck
4. **Practice daily** - even 5 minutes helps!

## Challenges Getting Harder?

That's intentional! The game adapts to teach you progressively:

- First master hex/binary conversion
- Then understand shifting
- Finally tackle real subnet problems

As your streak increases, the game raises bit-widths and points, and Review mode surfaces weak areas to reinforce learning.

## Need Help?

- Option 8 shows a quick reference
- Wrong answers show detailed explanations
- Each correct answer shows the breakdown

Happy bit manipulating! 🚀
