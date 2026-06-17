#include <iostream>
#include "types.hpp"

inline void printBB(Bitboard bb);

int main() {
  // Testing the types and bitboard utilities
  std::cout << "CaissaZero Initialized!" << std::endl;

  std::cout << "Bitboard for square A1:\n";
  printBB(squareBB(A1));
  std::cout << "Bitboard for Rank 4:\n";
  printBB(RANK_BB[RANK_4]);
  std::cout << "Bitboard for File D:\n";
  printBB(FILE_BB[FILE_D]);
  std::cout << "Bitboard for Center Squares:\n";
  printBB(CENTER_SQUARES_BB);

  std::cout << "Number of squares in Rank 1: " << popCount(RANK_BB[RANK_1]) << std::endl;

  Bitboard diag = MAIN_DIAGONAL_BB;
  while(diag) {
    Square sq = popLsb(diag);
    std::cout << "Popped Square Index: " << static_cast<int>(sq) << std::endl;

    printBB(diag);
  }
}

// Utility function to print a bitboard in a human-readable format
inline void printBB(Bitboard bb) {
  for (int rank = 7; rank >= 0; --rank) {
    for (int file = 0; file < 8; ++file) {
      Square sq = makeSquare(static_cast<File>(file), static_cast<Rank>(rank));
      std::cout << ((bb & squareBB(sq)) ? '1' : '.') << ' ';
    }
    std::cout << '\n';
  }
  std::cout << '\n';
}
