#pragma once

#include "move.hpp"
#include "position.hpp"

// --- Architectural Design ---
// generatePseudoLegalMoves(pos, list) -> may include moves that leave own king
// in check isLegal(pos, move)                  -> applies move, checks king
// safety, unmakes generateLegalMoves(pos, list)       -> pseudo-legal
// generation + isLegal filter

template <Color Us>
inline void generateKnightMoves(const Position& pos, MoveList& list) {
  constexpr Color Them = ~Us;
  constexpr Piece Knight = (Us == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT;

  Bitboard knights = pos.getPieces(Knight);
  Bitboard ourPieces = pos.getPiecesByColor(Us);
  Bitboard theirPieces = pos.getPiecesByColor(Them);

  // Outer loop: Iterate over all knights of the given color
  while (knights) {
    Square from = popLsb(knights);

    // Look up precomputed attacks and instantly mask out friendly pieces
    Bitboard attacks = KNIGHT_ATTACKS[from] & ~ourPieces;

    // Inner loop: Serialize the valid attacks into moves
    while (attacks) {
      Square to = popLsb(attacks);

      // If the target square intersects with enemy pieces, it's a capture
      MoveFlag flag = (theirPieces & squareBB(to)) ? FLAG_CAPTURE : FLAG_QUIET;

      list.push_back(Move(from, to, flag));
    }
  }
}

template <Color Us>
inline void generateKingMoves(const Position& pos, MoveList& list) {
  constexpr Color Them = ~Us;
  constexpr Piece King = (Us == WHITE) ? WHITE_KING : BLACK_KING;

  // A valid chess position always has exactly one king.
  // We don't need a while loop here; just grab the single lowest set bit.
  Bitboard kingBB = pos.getPieces(King);
  Square from = lsb(kingBB);

  Bitboard attacks = KING_ATTACKS[from] & ~pos.getPiecesByColor(Us);
  Bitboard theirPieces = pos.getPiecesByColor(Them);

  while (attacks) {
    Square to = popLsb(attacks);
    MoveFlag flag = (theirPieces & squareBB(to)) ? FLAG_CAPTURE : FLAG_QUIET;
    list.push_back(Move(from, to, flag));
  }
}

inline void addPawnMove(MoveList& list, Square from, Square to,
                        bool isCapture) {
  const Rank rank = getRank(to);

  if (rank == RANK_1 || rank == RANK_8) {
    const uint16_t base =
        isCapture ? FLAG_PROMOTE_KNIGHT_CAPTURE : FLAG_PROMOTE_KNIGHT;
    list.push_back(Move(from, to, static_cast<MoveFlag>(base)));
    list.push_back(Move(from, to, static_cast<MoveFlag>(base + 1)));
    list.push_back(Move(from, to, static_cast<MoveFlag>(base + 2)));
    list.push_back(Move(from, to, static_cast<MoveFlag>(base + 3)));
  } else {
    list.push_back(Move(from, to, isCapture ? FLAG_CAPTURE : FLAG_QUIET));
  }
}

template <Color Us>
inline void generatePawnMoves(const Position& pos, MoveList& list) {
  constexpr Color Them = ~Us;
  constexpr Piece Pawn = (Us == WHITE) ? WHITE_PAWN : BLACK_PAWN;
  constexpr int dir = (Us == WHITE) ? 8 : -8;
  constexpr Bitboard rank36 = (Us == WHITE) ? RANK_BB[RANK_3] : RANK_BB[RANK_6];

  const Bitboard pawns = pos.getPieces(Pawn);
  const Bitboard enemy = pos.getPiecesByColor(Them);
  const Bitboard empty =
      ~(pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK));

  // Single and double pushes via bulk shifts
  Bitboard single = (Us == WHITE) ? (pawns << 8) : (pawns >> 8);
  single &= empty;

  Bitboard doublePush =
      (Us == WHITE) ? ((single & rank36) << 8) : ((single & rank36) >> 8);
  doublePush &= empty;

  Bitboard b = single;
  while (b) {
    Square to = popLsb(b);
    addPawnMove(list, static_cast<Square>(to - dir), to, false);
  }

  b = doublePush;
  while (b) {
    Square to = popLsb(b);
    list.push_back(
        Move(static_cast<Square>(to - (2 * dir)), to,
             FLAG_DOUBLE_PAWN_PUSH));  // Not using addPawnMove because double
                                       // push is never a promotion
  }

  // Diagonal captures
  Bitboard capPawns = pawns;
  while (capPawns) {
    Square from = popLsb(capPawns);
    Bitboard targets = PAWN_ATTACKS[Us][from] & enemy;
    while (targets) {
      addPawnMove(list, from, popLsb(targets), true);
    }
  }

  // En passant
  const Square ep = pos.getEnPassantSquare();
  if (ep != SQUARE_NONE) {
    Bitboard candidates = PAWN_ATTACKS[Them][ep] & pawns;
    while (candidates) {
      list.push_back(
          Move(popLsb(candidates), ep,
               FLAG_EP_CAPTURE));  // Not using addPawnMove because en passant
                                   // is also never a promotion
    }
  }
}

template <Color Us, PieceType Pt>
inline void generateSliderMoves(const Position& pos, MoveList& list) {
  constexpr Color Them = ~Us;
  constexpr Piece piece = (Us == WHITE) ? 
                          (Pt == ROOK ? WHITE_ROOK : (Pt == BISHOP ? WHITE_BISHOP : WHITE_QUEEN)) :
                          (Pt == ROOK ? BLACK_ROOK : (Pt == BISHOP ? BLACK_BISHOP : BLACK_QUEEN));

  const Bitboard occ = pos.getPiecesByColor(WHITE) | pos.getPiecesByColor(BLACK);
  const Bitboard enemy = pos.getPiecesByColor(Them);
  Bitboard sliders = pos.getPieces(piece);

  while (sliders) {
    Square from = popLsb(sliders);
    Bitboard attacks = 0;

    // Evaluated at compile-time, zero runtime branching overhead
    if constexpr (Pt == ROOK)        attacks = getRookAttacks(from, occ);
    else if constexpr (Pt == BISHOP) attacks = getBishopAttacks(from, occ);
    else if constexpr (Pt == QUEEN)  attacks = getQueenAttacks(from, occ);

    attacks &= ~pos.getPiecesByColor(Us);

    while (attacks) {
      Square to = popLsb(attacks);
      list.push_back(Move(from, to, (enemy & squareBB(to)) ? FLAG_CAPTURE : FLAG_QUIET));
    }
  }
}

// Global entry point for all sliding pieces
template <Color Us>
inline void generateSlidingMoves(const Position& pos, MoveList& list) {
  generateSliderMoves<Us, BISHOP>(pos, list);
  generateSliderMoves<Us, ROOK>(pos, list);
  generateSliderMoves<Us, QUEEN>(pos, list);
}
