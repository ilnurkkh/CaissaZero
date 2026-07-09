# CaissaZero

A high-performance, UCI-compliant chess engine built from scratch in modern C++23. 

Designed with a strict focus on zero-allocation data structures, branchless logic, and compile-time evaluation, CaissaZero serves as an exploration of advanced bitboard manipulation and low-latency systems programming.

## Current State: Phase 2 (Move Generation)
The engine is currently in active development, transitioning from core board representation to pseudo-legal move generation.

### Implemented Features:
* **Board Representation:** Little-Endian Rank-File (LERF) uint64_t Bitboards.
* **State Management:** Highly optimized `Position` class encapsulating mailbox arrays, occupancy bitboards, and game state (castling, en passant, clocks).
* **Zobrist Hashing:** Perfect, incremental 64-bit Zobrist hashing for transpositions.
* **Compile-Time Attack Tables:** `constexpr` generated attack masks for leapers and sliding rays.
* **Move Encoding:** Highly packed 16-bit `Move` structure (`[from:6][to:6][flags:4]`).
* **Zero-Allocation Containers:** Stack-allocated, fixed-size `MoveList` bounded to the theoretical maximum of chess moves (256) to eliminate heap overhead during search.
* **Branchless Generation:** Templated leaper move generation (`template <Color Us>`) utilizing hardware-optimized `popLsb` operations to eliminate runtime branch mispredictions.

## Build Instructions
This project requires a compiler with full C++23 support (e.g., GCC 13+ or Clang 16+) and hardware optimization enabled.

```bash
# Clone the repository
git clone [https://github.com/yourusername/CaissaZero.git](https://github.com/yourusername/CaissaZero.git)
cd CaissaZero/src

# Compile with aggressive optimizations
g++ -std=c++23 -O3 -march=native -flto *.cpp -o caissa
