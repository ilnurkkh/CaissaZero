#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>

#include "attacks.hpp"
#include "position.hpp"
#include "types.hpp"
#include "move.hpp"

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
  
  std::cout << "CHECKPOINT 1.1 VERIFIED.\n";

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
  
  std::cout << "CHECKPOINT 1.2 VERIFIED.\n";

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

  std::cout << "CHECKPOINT 1.3 VERIFIED.\n";

  // =========================================================
    // CHECKPOINT 1.4: Zobrist Hashing Verification
    // =========================================================
    std::cout << "\n=== CHECKPOINT 1.4 TESTING ===\n\n";

    // Standard starting FEN
    pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    uint64_t initialHash = pos.getHashKey();
    uint64_t scratchHash = pos.computeHashFromScratch();

    std::cout << std::hex << std::uppercase;
    std::cout << "Starting Position Hash  : 0x" << initialHash << "\n";
    std::cout << "Computed from scratch   : 0x" << scratchHash << "\n";
    
    bool nonZero = (initialHash != 0ULL);
    bool startsMatch = (initialHash == scratchHash);
    std::cout << "Valid non-zero seed?    : " << (nonZero ? "PASS" : "FAIL") << "\n";
    std::cout << "Initial hashes match?   : " << (startsMatch ? "PASS" : "FAIL") << "\n\n";

    // HARD ASSERT: The hashes MUST match here
    assert(initialHash == scratchHash);

    // --- Simulate a Move: e2-e4 ---
    std::cout << "-> Simulating move: e2-e4\n";
    pos.removePiece(WHITE_PAWN, E2);
    pos.setPiece(WHITE_PAWN, E4);
    pos.setEnPassantSquare(E3); // e4 creates an EP target on e3
    pos.toggleSideToMove();     // Now it is Black's turn

    uint64_t moveHash = pos.getHashKey();
    uint64_t moveScratchHash = pos.computeHashFromScratch();

    std::cout << "Incremental update      : 0x" << moveHash << "\n";
    std::cout << "Computed from scratch   : 0x" << moveScratchHash << "\n";
    
    bool moveMatch = (moveHash == moveScratchHash);
    bool hashChanged = (moveHash != initialHash);
    std::cout << "Move hashes match?      : " << (moveMatch ? "PASS" : "FAIL") << "\n";
    std::cout << "Hash successfully changed?: " << (hashChanged ? "PASS" : "FAIL") << "\n\n";

    // HARD ASSERT: The hashes MUST match after an incremental update
    assert(moveHash == moveScratchHash);

    // --- Simulate Unmaking the Move ---
    std::cout << "-> Unmaking move: e2-e4\n";
    pos.toggleSideToMove();               // Back to White's turn
    pos.setEnPassantSquare(SQUARE_NONE);  // Clear the EP square
    pos.removePiece(WHITE_PAWN, E4);
    pos.setPiece(WHITE_PAWN, E2);

    uint64_t unmakeHash = pos.getHashKey();
    
    std::cout << "Restored Incremental    : 0x" << unmakeHash << "\n";
    bool unmakeMatch = (unmakeHash == initialHash);
    std::cout << "Perfectly restored?     : " << (unmakeMatch ? "PASS" : "FAIL") << "\n\n";

    // HARD ASSERT: The hash must identically return to the starting position hash
    assert(unmakeHash == initialHash);

    std::cout << std::dec; // Reset cout format back to decimal
    std::cout << "CHECKPOINT 1.4 VERIFIED.\n";

    // =========================================================
    // CHECKPOINT 1.5: Move Encoding Verification
    // =========================================================
    std::cout << "\n=== CHECKPOINT 1.5 TESTING ===\n\n";

    // 1. Verify size is strictly 2 bytes
    std::cout << "sizeof(Move) == 2 bytes  : " << (sizeof(Move) == 2 ? "PASS" : "FAIL") << "\n";
    assert(sizeof(Move) == 2);

    // 2. from_sq(make_move(E2, E4)) = E2
    Move m_e2e4(E2, E4);
    std::cout << "getFrom(E2-E4) == E2     : " << (m_e2e4.getFrom() == E2 ? "PASS" : "FAIL") << "\n";
    assert(m_e2e4.getFrom() == E2);

    // 3. to_sq(make_move(E2, E4, DOUBLE_PUSH)) = E4
    Move m_dpush(E2, E4, FLAG_DOUBLE_PAWN_PUSH);
    std::cout << "getTo(E2-E4, DBL) == E4  : " << (m_dpush.getTo() == E4 ? "PASS" : "FAIL") << "\n";
    assert(m_dpush.getTo() == E4);

    // 4. is_promotion(make_move(E7, E8, QUEEN_PROMO)) = true
    Move m_promo(E7, E8, FLAG_PROMOTE_QUEEN);
    std::cout << "isPromotion(Q_PROMO)     : " << (m_promo.isPromotion() ? "PASS" : "FAIL") << "\n";
    assert(m_promo.isPromotion());

    // 5. is_capture(make_move(D5, E6, EN_PASSANT)) = true
    Move m_ep(D5, E6, FLAG_EP_CAPTURE);
    std::cout << "isCapture(EN_PASSANT)    : " << (m_ep.isCapture() ? "PASS" : "FAIL") << "\n";
    assert(m_ep.isCapture());

    // 6. Round-Trip Fidelity: Promotion-Capture Move
    std::cout << "\n--- Round-Trip Fidelity Test (Promo-Capture) ---\n";
    Move m_promocap(E7, F8, FLAG_PROMOTE_QUEEN_CAPTURE);
    
    std::cout << "Move Encoded: E7 -> F8 (Queen Capture Promotion)\n";
    std::cout << "Expected From : 52 (E7) | Actual: " << +m_promocap.getFrom() << "\n";
    std::cout << "Expected To   : 61 (F8) | Actual: " << +m_promocap.getTo() << "\n";
    std::cout << "Expected Flag : 15      | Actual: " << m_promocap.getFlags() << "\n";
    std::cout << "Is Capture?   : Yes     | Actual: " << (m_promocap.isCapture() ? "Yes" : "No") << "\n";
    std::cout << "Is Promotion? : Yes     | Actual: " << (m_promocap.isPromotion() ? "Yes" : "No") << "\n";
    std::cout << "Promo Piece   : 4 (Q)   | Actual: " << +m_promocap.getPromotionPieceType() << "\n";

    // Strict round-trip assertions
    assert(m_promocap.getFrom() == E7);
    assert(m_promocap.getTo() == F8);
    assert(m_promocap.getFlags() == FLAG_PROMOTE_QUEEN_CAPTURE);
    assert(m_promocap.isCapture());
    assert(m_promocap.isPromotion());
    assert(m_promocap.getPromotionPieceType() == QUEEN);

    std::cout << "\nCHECKPOINT 1.5 VERIFIED.\n";
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
