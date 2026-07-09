#pragma once

#include "move.hpp"
#include "position.hpp"

// --- Architectural Design ---
// generatePseudoLegalMoves(pos, list) -> may include moves that leave own king in check
// isLegal(pos, move)                  -> applies move, checks king safety, unmakes
// generateLegalMoves(pos, list)       -> pseudo-legal generation + isLegal filter


// --- Leaper Move Generation ---

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
