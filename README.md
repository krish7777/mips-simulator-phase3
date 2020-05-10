# MIPS SIMULATOR PHASE 2

MIPS simulator written in C++.

The project was done by Krishnendu Sudheesh(CS18B020) and Dinkar(CS18B010).

## Instructions to run

```bash
g++ mips-simulator.cpp
./a.exe
```

All the register values, memory values, no of instructions, no. of stalls, CPI etc. will be printed on the screen

### Mips Instructions Implemented

- ADD
- ADDI
- SUB
- LI
- LW
- SW
- LA
- LUI
- J
- BGT
- BLT
- BNE
- BEQ
- SLT

#### Some examples of the stalls considered

```bash
LW R1, R4
ADD R3, R2, R1
```

```bash
ADD R1, R2, R3
BEQ R1, R4, label
```

```bash
LW R1, 0(R2)
BEQ R1, R4, label
```

```bash
LW R1, 0(R2)
ADD R4, R5, R6
BEQ R1, R3, label
```

```bash
BEQ R1, R4, label
any instruction
```

## Phase-3 (Cache Implementation)

Two caches have been included, cache1 and cache2.

The cache size, associativity, block size, cache access time can be customized.

We have implemented a write through cache with LRU replacement policy.
