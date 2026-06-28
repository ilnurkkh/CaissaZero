#include <iostream>
#include <string>

#include "position.hpp"
#include "types.hpp"

inline void printBB(Bitboard bb);

int main() {
  std::cout << "=== CAISSAZERO INITIALIZED ===\n\n";

  Position pos;

  // ---------------------------------------------------------
  // Test 1: Starting Position FEN
  // ---------------------------------------------------------
  pos.setFEN(std::string(STARTING_FEN));
  std::cout << "--- Starting Position Tests ---\n";

  int whitePawns = popCount(pos.getPieces(WHITE_PAWN));
  std::cout << "popcount(pieces[WHITE][PAWN]) = 8 : "
            << (whitePawns == 8 ? "PASS" : "FAIL") << "\n";

  Bitboard allPieces =
      pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  int totalOccupancy = popCount(allPieces);
  std::cout << "popcount(occupancy[BOTH]) = 32    : "
            << (totalOccupancy == 32 ? "PASS" : "FAIL") << "\n";

  Piece e1Piece = pos.getPieceAt(E1);
  std::cout << "piece_on(E1) = WHITE_KING         : "
            << (e1Piece == WHITE_KING ? "PASS" : "FAIL") << "\n";

  Piece e8Piece = pos.getPieceAt(E8);
  std::cout << "piece_on(E8) = BLACK_KING         : "
            << (e8Piece == BLACK_KING ? "PASS" : "FAIL") << "\n";

  Square epSquare = pos.getEnPassantSquare();
  std::cout << "ep_square = NO_SQ                 : "
            << (epSquare == SQUARE_NONE ? "PASS" : "FAIL") << "\n";

  CastlingRights cr = pos.getCastlingRights();
  bool allCastling = (cr == (WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE |
                             BLACK_QUEENSIDE));
  std::cout << "castling_rights = 0b1111          : "
            << (allCastling ? "PASS" : "FAIL") << "\n\n";

  // ---------------------------------------------------------
  // Test 2: Position 2 from Perft suite
  // ---------------------------------------------------------
  std::string fen2 =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
  pos.setFEN(fen2);
  std::cout << "--- Position 2 Tests ---\n";

  Bitboard allPieces2 =
      pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  int totalOccupancy2 = popCount(allPieces2);
  std::cout << "popcount(occupancy[BOTH]) = 32    : "
            << (totalOccupancy2 == 32 ? "PASS" : "FAIL") << "\n";

  CastlingRights cr2 = pos.getCastlingRights();
  bool allCastling2 = (cr2 == (WHITE_KINGSIDE | WHITE_QUEENSIDE |
                               BLACK_KINGSIDE | BLACK_QUEENSIDE));
  std::cout << "castling_rights = 0b1111          : "
            << (allCastling2 ? "PASS" : "FAIL") << "\n";

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
