#include "tests.hpp"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>

#include "attacks.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"

namespace Tests {

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

void printPosition(const Position& pos) {
  const std::string pieceChars = "PNBRQKpnbrqk";

  std::cout << "\n  +-----------------+\n";
  for (int rank = RANK_8; rank >= RANK_1; rank--) {
    std::cout << (rank + 1) << " | ";
    for (int file = FILE_A; file <= FILE_H; file++) {
      Square sq = makeSquare(static_cast<File>(file), static_cast<Rank>(rank));
      Piece p = pos.getPieceAt(sq);
      if (p == PIECE_NONE) {
        std::cout << ". ";
      } else {
        std::cout << pieceChars[p] << " ";
      }
    }
    std::cout << "|\n";
  }
  std::cout << "  +-----------------+\n";
  std::cout << "    a b c d e f g h\n\n";

  std::cout << "Zobrist Hash: 0x" << std::hex << std::uppercase
            << pos.getHashKey() << std::dec << "\n";
  std::cout << "Side to move: "
            << (pos.getSideToMove() == WHITE ? "White" : "Black") << "\n\n";
}

void verifyPositionIntegrity(const Position& pos) {
  Bitboard whiteOccupancy = pos.getPiecesByColor(WHITE);
  Bitboard blackOccupancy = pos.getPiecesByColor(BLACK);
  Bitboard allPieces = whiteOccupancy | blackOccupancy;

  // Validate piece count (applicable for our standard test sequences)
  assert(popCount(allPieces) == 32 &&
         "Popcount mismatch: pieces duplicated or deleted");

  // Validate Zobrist hash integrity against a full recomputation
  assert(pos.getHashKey() != 0ULL && "Hash is zero");
  assert(pos.getHashKey() == pos.computeHashFromScratch() &&
         "Incremental hash drifted from true state");

  // Cross-verify Mailbox array against Bitboard layers
  for (size_t sq = 0; sq < BOARD_SQUARE_COUNT; ++sq) {
    Square square = static_cast<Square>(sq);
    Piece p = pos.getPieceAt(square);

    if (p != PIECE_NONE) {
      assert((pos.getPieces(p) & squareBB(square)) != 0 &&
             "Piece bitboard desync");
      Color c = static_cast<Color>(p / 6);
      assert((pos.getPiecesByColor(c) & squareBB(square)) != 0 &&
             "Color bitboard desync");
      PieceType pt = static_cast<PieceType>(p % 6);
      assert((pos.getPiecesByType(pt) & squareBB(square)) != 0 &&
             "Type bitboard desync");
    } else {
      assert((allPieces & squareBB(square)) == 0 &&
             "Occupancy bitboard desync");
    }
  }
}

void testCheckpoint1_1() {
  std::cout << "[Checkpoint 1.1] Bitboard Basics\n";

  Bitboard diag = MAIN_DIAGONAL_BB;
  Square firstSq = popLsb(diag);
  assert(firstSq == A1);

  int remainingPops = 0;
  while (diag) {
    popLsb(diag);
    remainingPops++;
  }
  assert(remainingPops == 7);
  assert(popCount(RANK_BB[RANK_1]) == 8);

  std::cout << "-> Passed\n\n";
}

void testCheckpoint1_2() {
  std::cout << "[Checkpoint 1.2] FEN Parsing & Position State\n";

  Position pos;

  // Validate StartPos FEN initialization
  pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  assert(popCount(pos.getPieces(WHITE_PAWN)) == 8);
  Bitboard allPieces =
      pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  assert(popCount(allPieces) == 32);

  assert(pos.getPieceAt(E1) == WHITE_KING);
  assert(pos.getPieceAt(E8) == BLACK_KING);
  assert(pos.getEnPassantSquare() == SQUARE_NONE);

  CastlingRights cr = pos.getCastlingRights();
  assert(cr ==
         (WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE));

  // Validate intermediate test position
  pos.setFEN(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  allPieces = pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  assert(popCount(allPieces) == 32);
  cr = pos.getCastlingRights();
  assert(cr ==
         (WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE));

  std::cout << "-> Passed\n\n";
}

void testCheckpoint1_3() {
  std::cout << "[Checkpoint 1.3] Attack Tables\n";

  Bitboard nA1 = KNIGHT_ATTACKS[A1];
  assert(popCount(nA1) == 2 && nA1 == (squareBB(B3) | squareBB(C2)));

  Bitboard nD4 = KNIGHT_ATTACKS[D4];
  assert(popCount(nD4) == 8);

  Bitboard nG1 = KNIGHT_ATTACKS[G1];
  assert(popCount(nG1) == 3 &&
         nG1 == (squareBB(F3) | squareBB(H3) | squareBB(E2)));

  Bitboard kA1 = KING_ATTACKS[A1];
  assert(popCount(kA1) == 3 &&
         kA1 == (squareBB(A2) | squareBB(B1) | squareBB(B2)));

  Bitboard pE4 = PAWN_ATTACKS[WHITE][E4];
  assert(popCount(pE4) == 2 && pE4 == (squareBB(D5) | squareBB(F5)));

  Bitboard pA4 = PAWN_ATTACKS[WHITE][A4];
  assert(popCount(pA4) == 1 && pA4 == squareBB(B5));

  std::cout << "-> Passed\n\n";
}

void testCheckpoint1_4() {
  std::cout << "[Checkpoint 1.4] Zobrist Hash Synchronization\n";

  Position pos;
  pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  uint64_t initialHash = pos.getHashKey();
  uint64_t scratchHash = pos.computeHashFromScratch();

  assert(initialHash != 0ULL);
  assert(initialHash == scratchHash);

  // Simulate incremental board changes
  pos.removePiece(WHITE_PAWN, E2);
  pos.setPiece(WHITE_PAWN, E4);
  pos.setEnPassantSquare(E3);
  pos.toggleSideToMove();

  uint64_t moveHash = pos.getHashKey();
  uint64_t moveScratchHash = pos.computeHashFromScratch();

  assert(moveHash != initialHash);
  assert(moveHash == moveScratchHash);

  // Unmake the simulated move to verify perfect hash restoration
  pos.toggleSideToMove();
  pos.setEnPassantSquare(SQUARE_NONE);
  pos.removePiece(WHITE_PAWN, E4);
  pos.setPiece(WHITE_PAWN, E2);

  uint64_t unmakeHash = pos.getHashKey();
  assert(unmakeHash == initialHash);

  std::cout << "-> Passed\n\n";
}

void testCheckpoint1_5() {
  std::cout << "[Checkpoint 1.5] Move Encoding\n";

  // Validate structure size to prevent padding overhead
  assert(sizeof(Move) == 2);

  // Test quiet move extraction
  Move m_e2e4(E2, E4);
  assert(m_e2e4.getFrom() == E2);
  assert(m_e2e4.getTo() == E4);

  // Test flag preservation
  Move m_dpush(E2, E4, FLAG_DOUBLE_PAWN_PUSH);
  assert(m_dpush.getTo() == E4);

  Move m_promo(E7, E8, FLAG_PROMOTE_QUEEN);
  assert(m_promo.isPromotion());

  Move m_ep(D5, E6, FLAG_EP_CAPTURE);
  assert(m_ep.isCapture());

  // Validate complex round-trip encoding (Promotion Capture)
  Move m_promocap(E7, F8, FLAG_PROMOTE_QUEEN_CAPTURE);
  assert(m_promocap.getFrom() == E7);
  assert(m_promocap.getTo() == F8);
  assert(m_promocap.getFlags() == FLAG_PROMOTE_QUEEN_CAPTURE);
  assert(m_promocap.isCapture());
  assert(m_promocap.isPromotion());
  assert(m_promocap.getPromotionPieceType() == QUEEN);

  std::cout << "-> Passed\n\n";
}

void runPhase1ExitGate() {
  std::cout << "[Phase 1 Exit Gate] Board state simulation\n";

  Position pos;
  pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  verifyPositionIntegrity(pos);
  uint64_t hash0 = pos.getHashKey();

  // 1. e4
  pos.removePiece(WHITE_PAWN, E2);
  pos.setPiece(WHITE_PAWN, E4);
  pos.setEnPassantSquare(E3);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash1 = pos.getHashKey();
  assert(hash1 != hash0);

  // 1... e5
  pos.removePiece(BLACK_PAWN, E7);
  pos.setPiece(BLACK_PAWN, E5);
  pos.setEnPassantSquare(E6);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash2 = pos.getHashKey();
  assert(hash2 != hash1 && hash2 != hash0);

  // 2. Nf3
  pos.removePiece(WHITE_KNIGHT, G1);
  pos.setPiece(WHITE_KNIGHT, F3);
  pos.setEnPassantSquare(SQUARE_NONE);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash3 = pos.getHashKey();
  assert(hash3 != hash2 && hash3 != hash1 && hash3 != hash0);

  std::cout << "-> All Integration Assertions Passed\n\n\n";
}

void testCheckpoint2_2() {
  std::cout << "[Checkpoint 2.2] Leaper Move Generation (Kings & Knights)\n";

  Position pos;
  MoveList list;

  // --- Setup 1: Raw Mobility Counts ---
  pos.setFEN("4k3/8/8/3N4/8/8/8/4K2N w - - 0 1");

  generateKnightMoves<WHITE>(pos, list);
  
  int d5Moves = 0;
  int h1Moves = 0;
  for (const Move& m : list) {
    if (m.getFrom() == D5) d5Moves++;
    if (m.getFrom() == H1) h1Moves++;
  }
  
  assert(list.size() == 10 && "Total knight moves should be 10");
  assert(d5Moves == 8 && "D5 Knight should have exactly 8 moves");
  assert(h1Moves == 2 && "H1 Knight should have exactly 2 moves");

  MoveList kingList;
  generateKingMoves<WHITE>(pos, kingList);
  assert(kingList.size() == 5 && "E1 King on the edge should have exactly 5 moves");

  // --- Setup 2: Capture Flagging & Friendly-Fire Masking ---
  // We place a White Knight on D5. 
  // We place a White Pawn on E3 (Friendly piece, should be blocked).
  // We place a Black Pawn on C7 (Enemy piece, should be flagged CAPTURE).
  pos.setFEN("8/2p5/8/3N4/8/4P3/8/8 w - - 0 1");
  MoveList combatList;
  
  generateKnightMoves<WHITE>(pos, combatList);
  
  bool captureFound = false;
  bool friendlyFireFound = false;

  for (const Move& m : combatList) {
    // Check if the generator incorrectly allowed a move to the friendly E3 pawn
    if (m.getTo() == E3) {
      friendlyFireFound = true;
    }
    // Check if the move to the enemy C7 pawn is correctly flagged
    if (m.getTo() == C7) {
      assert(m.isCapture() && "Move to C7 should have FLAG_CAPTURE set");
      captureFound = true;
    } else {
      assert(!m.isCapture() && "Non-capture move has FLAG_CAPTURE set");
    }
  }

  assert(!friendlyFireFound && "Knight generated a move onto a friendly piece (E3)");
  assert(captureFound && "Knight failed to generate a capture move onto an enemy piece (C7)");

  std::cout << "-> Passed\n\n";
}

}  // namespace Tests
