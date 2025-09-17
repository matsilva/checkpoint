# Bit Manipulation Guide for IP Range Matching

## Understanding Hexadecimal (0xFFFFFFFF Breakdown)

### What is Hexadecimal?

Hexadecimal (hex) is base-16 number system using digits 0-9 and letters A-F.

- **0x** prefix indicates hexadecimal in C++
- Each hex digit represents 4 bits (half a byte)
- Two hex digits = 1 byte = 8 bits

### Hex Digit Values

```
Hex | Decimal | Binary
----|---------|--------
0   |    0    | 0000
1   |    1    | 0001
2   |    2    | 0010
3   |    3    | 0011
4   |    4    | 0100
5   |    5    | 0101
6   |    6    | 0110
7   |    7    | 0111
8   |    8    | 1000
9   |    9    | 1001
A   |   10    | 1010
B   |   11    | 1011
C   |   12    | 1100
D   |   13    | 1101
E   |   14    | 1110
F   |   15    | 1111  ← All bits set!
```

### Breaking Down 0xFFFFFFFF

```
0xFFFFFFFF = 8 F's = 8 × 4 bits = 32 bits total

Each F = 1111 in binary (all 4 bits set to 1)

0xFFFFFFFF breakdown:
F    F    F    F    F    F    F    F
1111 1111 1111 1111 1111 1111 1111 1111

This is 32 consecutive 1s = all bits set in a 32-bit integer
Decimal value: 4,294,967,295 (max value for unsigned 32-bit int)
```

### Common Hex Patterns

```
0xFF        = 1111 1111                 = 255 (all bits in 1 byte)
0xFFFF      = 1111 1111 1111 1111       = 65,535 (all bits in 2 bytes)
0xFFFFFFFF  = 32 ones                   = 4,294,967,295 (all bits in 4 bytes)
0xFFFFFFFFFFFFFFFF = 64 ones            = 18,446,744,073,709,551,615 (all bits in 8 bytes)

0x00        = 0000 0000                 = 0 (no bits set)
0x01        = 0000 0001                 = 1 (only rightmost bit set)
0x80        = 1000 0000                 = 128 (only leftmost bit of byte set)
0xAA        = 1010 1010                 = 170 (alternating bits)
0x55        = 0101 0101                 = 85 (opposite alternating)
```

### Why 0xFFFFFFFF for Subnet Masks?

When creating subnet masks, we start with all 1s and shift left:

```cpp
// For /24 subnet (24 network bits, 8 host bits):
uint32_t mask = 0xFFFFFFFF << (32 - 24);
              = 0xFFFFFFFF << 8;

Visual step-by-step:
1. Start:     1111 1111 1111 1111 1111 1111 1111 1111  (0xFFFFFFFF)
2. Shift << 8: Moves all bits 8 positions left
3. Result:    1111 1111 1111 1111 1111 1111 0000 0000  (0xFFFFFF00)
              ↑-------- network bits --------↑ ↑-host-↑

The 8 zeros on the right are where host bits go!
```

### More Examples with Different Subnets

```cpp
// /32 subnet (single host):
0xFFFFFFFF << (32 - 32) = 0xFFFFFFFF << 0 = 0xFFFFFFFF
Result: 1111 1111 1111 1111 1111 1111 1111 1111 (no host bits)

// /16 subnet:
0xFFFFFFFF << (32 - 16) = 0xFFFFFFFF << 16 = 0xFFFF0000
Result: 1111 1111 1111 1111 0000 0000 0000 0000
        ↑-- network 16 bits--↑ ↑-- host 16 bits--↑

// /8 subnet:
0xFFFFFFFF << (32 - 8) = 0xFFFFFFFF << 24 = 0xFF000000
Result: 1111 1111 0000 0000 0000 0000 0000 0000
        ↑network↑ ↑------ host 24 bits -------↑
```

### Reading Hex in Code

When you see hex in code, mentally convert:

```cpp
if (value & 0xFF000000)  // Checking if any of the top 8 bits are set
value = value | 0x80;    // Setting bit 7 (1000 0000)
value = value & 0xFFFFFFF0; // Clearing the bottom 4 bits (setting them to 0)
```

## Key Concepts

### 1. Bit Shifting Operators

#### Left Shift (`<<`)

Moves bits to the left, filling with zeros on the right.

```
5 << 2  means shift 5 (binary: 0101) left by 2 positions
Result: 20 (binary: 10100)

Visual:
  0101 (original)
 ↓
010100 (after << 2)
```

#### Right Shift (`>>`)

Moves bits to the right, discarding bits that fall off.

```
20 >> 2  means shift 20 (binary: 10100) right by 2 positions
Result: 5 (binary: 0101)

Visual:
10100 (original)
   ↓
00101 (after >> 2)
```

### 2. Bitwise Operators

#### AND (`&`)

Results in 1 only when both bits are 1.

```
  1010
& 1100
------
  1000
```

Used for: Masking (keeping only certain bits)

#### OR (`|`)

Results in 1 when at least one bit is 1.

```
  1010
| 1100
------
  1110
```

Used for: Combining values

#### XOR (`^`)

Results in 1 when bits are different.

```
  1010
^ 1100
------
  0110
```

#### NOT (`~`)

Flips all bits.

```
~1010 = 0101
```

## IPv4 Example: 192.168.1.5/24

### Step 1: Convert IP to 32-bit integer

```
192.168.1.5 → [192][168][1][5]
           → [0xC0][0xA8][0x01][0x05]
           → [11000000][10101000][00000001][00000101]

Combining with shifts:
192 << 24 = 0xC0000000  (11000000 00000000 00000000 00000000)
168 << 16 = 0x00A80000  (00000000 10101000 00000000 00000000)
  1 << 8  = 0x00000100  (00000000 00000000 00000001 00000000)
  5 << 0  = 0x00000005  (00000000 00000000 00000000 00000101)
         OR together:
            0xC0A80105  (11000000 10101000 00000001 00000101)
```

### Step 2: Create subnet mask for /24

```
/24 means first 24 bits are network, last 8 are host

Mask = 0xFFFFFFFF << (32 - 24)
     = 0xFFFFFFFF << 8
     = 0xFFFFFF00

Binary: 11111111 11111111 11111111 00000000
        ↑------- network -------↑ ↑- host -↑
```

### Step 3: Apply mask

```
IP & Mask = 0xC0A80105 & 0xFFFFFF00

  11000000 10101000 00000001 00000101  (IP)
& 11111111 11111111 11111111 00000000  (Mask)
---------------------------------------
  11000000 10101000 00000001 00000000  (Result: 192.168.1.0)
```

## IPv6 Example: 2001:db8:85a3::8a2e:370:7334/64

### IPv6 is 128 bits (8 blocks × 16 bits)

```
2001:0db8:85a3:0000:0000:8a2e:0370:7334

Split into two 64-bit halves:
Left:  [2001][0db8][85a3][0000]
Right: [0000][8a2e][0370][7334]
```

### Combining left half

```
0x2001 << 48 = 0x2001000000000000
0x0db8 << 32 = 0x00000db800000000
0x85a3 << 16 = 0x0000000085a30000
0x0000 << 0  = 0x0000000000000000
            OR: 0x20010db885a30000
```

## Common CIDR Subnet Masks

| CIDR | IPv4 Mask       | Binary Mask                         | Hosts |
| ---- | --------------- | ----------------------------------- | ----- |
| /8   | 255.0.0.0       | 11111111 00000000 00000000 00000000 | 16M   |
| /16  | 255.255.0.0     | 11111111 11111111 00000000 00000000 | 65K   |
| /24  | 255.255.255.0   | 11111111 11111111 11111111 00000000 | 256   |
| /32  | 255.255.255.255 | 11111111 11111111 11111111 11111111 | 1     |

## Practice Problems

1. **What does `0x0F << 4` equal?**
   - 0x0F = 15 = 0000 1111
   - Shift left 4: 1111 0000 = 0xF0 = 240

2. **What does `0xFF & 0x0F` equal?**
   - 0xFF = 1111 1111
   - 0x0F = 0000 1111
   - AND: 0000 1111 = 0x0F = 15

3. **How would you extract the third byte from 0xAABBCCDD?**
   - Shift right 8: 0x00AABBCC
   - AND with 0xFF: 0xCC

## Tips for Understanding

1. **Visualize in binary**: Convert to binary to see what's happening
2. **Use smaller numbers**: Practice with 8-bit values before 32/64-bit
3. **Draw it out**: Sketch the bits moving when shifting
4. **Remember the purpose**:
   - Shifting builds/extracts values
   - AND masks/filters bits
   - OR combines values

## Real-World Uses

1. **Network programming**: IP addresses, subnet masks
2. **Flags/Options**: Store multiple booleans in one integer
3. **Graphics**: RGB color manipulation
4. **Cryptography**: Hash functions, encryption
5. **Performance**: Bit operations are extremely fast

