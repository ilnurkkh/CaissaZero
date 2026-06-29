#include <iostream>
#include <string>

#include "attacks.hpp"
#include "position.hpp"
#include "types.hpp"

void printBB(Bitboard bb);

int main() {
  std::cout << "========================================\n";
  std::cout << "      CAISSA ZERO - TEST SUITE\n";
  std::cout << "========================================\n\n";

  // =========================================================
  // CHECKPOINT 1.1: Bitboard Basics
  // =========================================================
  std::cout << "=== CHECKPOINT 1.1 TESTING ===\n\n";

  std::cout << "--- square_bb(A1) (Should be bottom-left) ---\n";
  printBB(squareBB(A1));

  std::cout << "--- RANK_BB[RANK_4] (Should be 4th rank fully filled) ---\n";
  printBB(RANK_BB[RANK_4]);

  std::cout << "--- pop_lsb on MAIN_DIAGONAL_BB ---\n";
  Bitboard diag = MAIN_DIAGONAL_BB;
  Square firstSq = popLsb(diag);
  std::cout << "First pop yields A1               : "
            << (firstSq == A1 ? "PASS" : "FAIL") << "\n";

  int remainingPops = 0;
  while (diag) {
    popLsb(diag);
    remainingPops++;
  }
  std::cout << "Correctly shrinks until empty (7) : "
            << (remainingPops == 7 ? "PASS" : "FAIL") << "\n";

  std::cout << "popcount(RANK_BB[RANK_1]) == 8    : "
            << (popCount(RANK_BB[RANK_1]) == 8 ? "PASS" : "FAIL") << "\n\n";

  // =========================================================
  // CHECKPOINT 1.2: Position & FEN Parsing
  // =========================================================
  std::cout << "=== CHECKPOINT 1.2 TESTING ===\n\n";

  Position pos;

  // Test 1: Starting Position FEN
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

  // Test 2: Position 2 from Perft suite
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

  // =========================================================
  // CHECKPOINT 1.3: Attack Table Verification
  // =========================================================
  std::cout << "\n=== CHECKPOINT 1.3 TESTING ===\n\n";

  Bitboard nA1 = KNIGHT_ATTACKS[A1];
  bool nA1_pass =
      (popCount(nA1) == 2) && (nA1 == (squareBB(B3) | squareBB(C2)));
  std::cout << "KNIGHT_ATTACKS[A1] (2 bits: B3, C2)      : "
            << (nA1_pass ? "PASS" : "FAIL") << "\n";

  Bitboard nD4 = KNIGHT_ATTACKS[D4];
  bool nD4_pass = (popCount(nD4) == 8);
  std::cout << "KNIGHT_ATTACKS[D4] (8 bits)              : "
            << (nD4_pass ? "PASS" : "FAIL") << "\n";

  Bitboard nG1 = KNIGHT_ATTACKS[G1];
  bool nG1_pass = (popCount(nG1) == 3) &&
                  (nG1 == (squareBB(F3) | squareBB(H3) | squareBB(E2)));
  std::cout << "KNIGHT_ATTACKS[G1] (3 bits: F3, H3, E2)  : "
            << (nG1_pass ? "PASS" : "FAIL") << "\n";

  Bitboard kA1 = KING_ATTACKS[A1];
  bool kA1_pass = (popCount(kA1) == 3) &&
                  (kA1 == (squareBB(A2) | squareBB(B1) | squareBB(B2)));
  std::cout << "KING_ATTACKS[A1] (3 bits: A2, B1, B2)    : "
            << (kA1_pass ? "PASS" : "FAIL") << "\n";

  Bitboard pE4 = PAWN_ATTACKS[WHITE][E4];
  bool pE4_pass =
      (popCount(pE4) == 2) && (pE4 == (squareBB(D5) | squareBB(F5)));
  std::cout << "PAWN_ATTACKS[WHITE][E4] (2 bits: D5, F5) : "
            << (pE4_pass ? "PASS" : "FAIL") << "\n";

  Bitboard pA4 = PAWN_ATTACKS[WHITE][A4];
  bool pA4_pass = (popCount(pA4) == 1) && (pA4 == squareBB(B5));
  std::cout << "PAWN_ATTACKS[WHITE][A4] (1 bit: B5)      : "
            << (pA4_pass ? "PASS" : "FAIL") << "\n\n";

  std::cout << "ALL CHECKPOINTS COMPLETE.\n";
  return 0;
}

// A handy debugging function to print any bitboard as an 8x8 grid
void printBB(Bitboard bb) {
  for (int rank = RANK_8; rank >= RANK_1; rank--) {
    for (int file = FILE_A; file <= FILE_H; file++) {
      Square sq = makeSquare(static_cast<File>(file), static_cast<Rank>(rank));
      if (bb & squareBB(sq)) {
        std::cout << "1 ";
      } else {
        std::cout << ". ";
      }
    }
    std::cout << "  (Rank " << (rank + 1) << ")\n";
  }
  std::cout << "a b c d e f g h\n\n";
}
