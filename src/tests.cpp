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

// Renders Little-Endian Rank-File (LERF) bitboards for visual debugging
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

// Cross-verifies the 1D Mailbox array state against the 64-bit occupancy layers
void verifyPositionIntegrity(const Position& pos) {
  const Bitboard whiteOccupancy = pos.getPiecesByColor(WHITE);
  const Bitboard blackOccupancy = pos.getPiecesByColor(BLACK);
  const Bitboard allPieces = whiteOccupancy | blackOccupancy;

  assert(popCount(allPieces) == 32 && "Popcount mismatch");

  // Detects incremental hash drift by comparing against a full recomputation
  assert(pos.getHashKey() != 0ULL && "Zero hash detected");
  assert(pos.getHashKey() == pos.computeHashFromScratch() &&
         "Incremental hash desync");

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

  // Validates hardware LSB bit-scan ordering
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
  pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  assert(popCount(pos.getPieces(WHITE_PAWN)) == 8);

  Bitboard allPieces =
      pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  assert(popCount(allPieces) == 32);
  assert(pos.getPieceAt(E1) == WHITE_KING);
  assert(pos.getPieceAt(E8) == BLACK_KING);
  assert(pos.getEnPassantSquare() == SQUARE_NONE);
  assert(pos.getCastlingRights() ==
         (WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE));

  pos.setFEN(
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  allPieces = pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);

  assert(popCount(allPieces) == 32);
  assert(pos.getCastlingRights() ==
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
  assert(initialHash != 0ULL);
  assert(initialHash == pos.computeHashFromScratch());

  // Mutate state incrementally
  pos.removePiece(WHITE_PAWN, E2);
  pos.setPiece(WHITE_PAWN, E4);
  pos.setEnPassantSquare(E3);
  pos.toggleSideToMove();

  uint64_t moveHash = pos.getHashKey();
  assert(moveHash != initialHash);
  assert(moveHash == pos.computeHashFromScratch());

  // Simulates a full unmake to guarantee perfect XOR reversibility
  pos.toggleSideToMove();
  pos.setEnPassantSquare(SQUARE_NONE);
  pos.removePiece(WHITE_PAWN, E4);
  pos.setPiece(WHITE_PAWN, E2);

  assert(pos.getHashKey() == initialHash);

  std::cout << "-> Passed\n\n";
}

void testCheckpoint1_5() {
  std::cout << "[Checkpoint 1.5] Move Encoding\n";

  // Validates struct packing to prevent transposition table bloat
  assert(sizeof(Move) == 2 && "Move struct exceeds 16 bits");

  Move m_e2e4(E2, E4);
  assert(m_e2e4.getFrom() == E2);
  assert(m_e2e4.getTo() == E4);

  Move m_dpush(E2, E4, FLAG_DOUBLE_PAWN_PUSH);
  assert(m_dpush.getTo() == E4);

  Move m_promo(E7, E8, FLAG_PROMOTE_QUEEN);
  assert(m_promo.isPromotion());

  Move m_ep(D5, E6, FLAG_EP_CAPTURE);
  assert(m_ep.isCapture());

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

  pos.removePiece(WHITE_PAWN, E2);
  pos.setPiece(WHITE_PAWN, E4);
  pos.setEnPassantSquare(E3);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash1 = pos.getHashKey();
  assert(hash1 != hash0);

  pos.removePiece(BLACK_PAWN, E7);
  pos.setPiece(BLACK_PAWN, E5);
  pos.setEnPassantSquare(E6);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash2 = pos.getHashKey();
  assert(hash2 != hash1 && hash2 != hash0);

  pos.removePiece(WHITE_KNIGHT, G1);
  pos.setPiece(WHITE_KNIGHT, F3);
  pos.setEnPassantSquare(SQUARE_NONE);
  pos.toggleSideToMove();

  verifyPositionIntegrity(pos);
  uint64_t hash3 = pos.getHashKey();
  assert(hash3 != hash2 && hash3 != hash1 && hash3 != hash0);

  std::cout << "-> All Integration Assertions Passed\n\n";
}

void testCheckpoint2_2() {
  std::cout << "[Checkpoint 2.2] Leaper Move Generation (Kings & Knights)\n";

  Position pos;
  MoveList list;

  pos.setFEN("4k3/8/8/3N4/8/8/8/4K2N w - - 0 1");
  generateKnightMoves<WHITE>(pos, list);

  int d5Moves = 0;
  int h1Moves = 0;
  for (const Move& m : list) {
    if (m.getFrom() == D5) d5Moves++;
    if (m.getFrom() == H1) h1Moves++;
  }

  // Verifies corner mobility bounds to prevent off-board bit shift wrap-around
  assert(list.size() == 10 && "Knight mobility count mismatch");
  assert(d5Moves == 8 && "D5 Knight open mobility mismatch");
  assert(h1Moves == 2 && "H1 Knight corner mobility mismatch");

  MoveList kingList;
  generateKingMoves<WHITE>(pos, kingList);
  assert(kingList.size() == 5 && "Edge King mobility mismatch");

  pos.setFEN("8/2p5/8/3N4/8/4P3/8/8 w - - 0 1");
  MoveList combatList;
  generateKnightMoves<WHITE>(pos, combatList);

  bool captureFound = false;
  bool friendlyFireFound = false;

  for (const Move& m : combatList) {
    if (m.getTo() == E3) friendlyFireFound = true;
    if (m.getTo() == C7) {
      assert(m.isCapture() && "Capture flag missing on enemy target");
      captureFound = true;
    } else {
      assert(!m.isCapture() && "Capture flag incorrectly set on empty square");
    }
  }

  assert(!friendlyFireFound && "Generator failed to mask friendly pieces");
  assert(captureFound && "Generator failed to target enemy pieces");

  std::cout << "-> Passed\n\n";
}

void testCheckpoint2_3() {
  std::cout << "[Checkpoint 2.3] Pawn Move Generation\n";

  Position pos;
  MoveList list;

  pos.setFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  generatePawnMoves<WHITE>(pos, list);
  assert(list.size() == 16 && "StartPos pawn move count mismatch");

  list = MoveList();
  pos.setFEN("8/8/8/8/8/8/P7/8 w - - 0 1");
  generatePawnMoves<WHITE>(pos, list);
  assert(list.size() == 2 && "A2 pawn mobility mismatch");

  list = MoveList();
  pos.setFEN("8/P7/8/8/8/8/8/8 w - - 0 1");
  generatePawnMoves<WHITE>(pos, list);
  assert(list.size() == 4 && "A7 pawn did not yield exactly 4 promotions");
  for (const Move& m : list) {
    assert(m.isPromotion() && "Promotion flag missing");
    assert(m.getFlags() != FLAG_DOUBLE_PAWN_PUSH &&
           "Double push flag incorrectly set on rank 7");
  }

  list = MoveList();
  pos.setFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
  generatePawnMoves<WHITE>(pos, list);

  bool epFound = false;
  for (const Move& m : list) {
    if (m.getFrom() == E5 && m.getTo() == F6 &&
        m.getFlags() == FLAG_EP_CAPTURE) {
      epFound = true;
      break;
    }
  }
  assert(epFound && "Generator failed to identify en passant target");

  list = MoveList();
  pos.setFEN("8/8/8/8/p7/8/P7/8 w - - 0 1");
  generatePawnMoves<WHITE>(pos, list);
  assert(list.size() == 1 && "Generator incorrectly bypassed blocking piece");
  assert(list[0].getTo() == A3 &&
         "Only valid move should be single push to A3");

  list = MoveList();
  pos.setFEN("r1r5/1P6/8/8/8/8/8/8 w - - 0 1");
  generatePawnMoves<WHITE>(pos, list);

  // 4 push promos (B8) + 4 capture promos (A8) + 4 capture promos (C8)
  assert(list.size() == 12 && "Capture-promotion move count mismatch");

  int pushPromos = 0, capPromos = 0;
  for (const Move& m : list) {
    if (m.getTo() == B8) pushPromos++;
    if (m.getTo() == A8 || m.getTo() == C8) capPromos++;
  }
  assert(pushPromos == 4 && capPromos == 8 &&
         "Promotion flag distribution mismatch");

  std::cout << "-> Passed\n\n";
}

void testCheckpoint2_4() {
  std::cout << "[Checkpoint 2.4] Sliding Piece Ray-Casting\n";

  Position pos;
  MoveList list;

  pos.setFEN("8/8/8/3R4/8/8/8/8 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);
  assert(list.size() == 14 && "Lone rook open mobility mismatch");

  list = MoveList();
  pos.setFEN("8/8/8/3B4/8/8/8/8 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);
  assert(list.size() == 13 && "Lone bishop open mobility mismatch");

  // Validates early-exit on friendly blockers
  list = MoveList();
  pos.setFEN("8/3P4/8/3R4/8/8/8/8 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);

  bool d6Found = false, d7Found = false, d8Found = false;
  for (const Move& m : list) {
    if (m.getTo() == D6) d6Found = true;
    if (m.getTo() == D7) d7Found = true;
    if (m.getTo() == D8) d8Found = true;
  }

  assert(d6Found && "Rook failed to step before friendly blocker");
  assert(!d7Found && "Rook incorrectly targeted friendly piece");
  assert(!d8Found && "Rook incorrectly jumped over friendly piece");

  // Validates inclusive-exit on enemy captures
  list = MoveList();
  pos.setFEN("8/3p4/8/3R4/8/8/8/8 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);

  d6Found = d7Found = d8Found = false;
  bool captureFlagged = false;
  for (const Move& m : list) {
    if (m.getTo() == D6) d6Found = true;
    if (m.getTo() == D7) {
      d7Found = true;
      captureFlagged = m.isCapture();
    }
    if (m.getTo() == D8) d8Found = true;
  }

  assert(d6Found && "Rook failed to step before enemy blocker");
  assert(d7Found && captureFlagged && "Rook failed to capture enemy blocker");
  assert(!d8Found && "Rook incorrectly jumped over enemy blocker");

  // A1 Rook boxed in by friendly pawns on A2 and B1
  // Confirms the ray does not wrap horizontally across the A/H files
  list = MoveList();
  pos.setFEN("8/8/8/8/8/8/P7/RP6 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);
  assert(list.size() == 0 && "Corner rook generated illegal wraparound moves");

  list = MoveList();
  pos.setFEN("8/8/8/3Q4/8/8/8/8 w - - 0 1");
  generateSlidingMoves<WHITE>(pos, list);
  assert(list.size() == 27 && "Lone queen mobility mismatch (14 + 13)");

  std::cout << "-> Passed\n\n";
}

}  // namespace Tests
