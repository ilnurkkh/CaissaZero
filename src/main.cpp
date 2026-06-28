#include <iostream>
#include <string>

#include "position.hpp"
#include "types.hpp"

inline void printBB(Bitboard bb);

int main() {
  std::cout << "=== CAISSAZERO INITIALIZED ===\n\n";

  Position pos;

  // We convert string_view to std::string because setFEN expects a std::string
  pos.setFEN(std::string(STARTING_FEN));

  std::cout << "White Pieces (Should be solid on Ranks 1 and 2):\n";
  printBB(pos.getPiecesByColor(WHITE));

  std::cout << "All Knights (Should be on b1, g1, b8, g8):\n";
  printBB(pos.getPiecesByType(KNIGHT));

  std::cout << "All Pawns (Should be solid on Ranks 2 and 7):\n";
  printBB(pos.getPiecesByType(PAWN));

  return 0;
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
