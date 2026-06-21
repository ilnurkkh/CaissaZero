#pragma once

#include <array>

#include "types.hpp"

class Position {
 private:
  std::array<Bitboard, PIECE_NONE> pieces{};
  std::array<Bitboard, COLOR_NONE> byColor{};
  std::array<Bitboard, PIECE_TYPE_NONE> byType{};

  Color sideToMove{};
  int halfMoveClock{0};
  int fullMoveNumber{1};
  Square enPassantSquare{SQUARE_NONE};
  CastlingRights castlingRights{static_cast<CastlingRights>(WHITE_KINGSIDE | WHITE_QUEENSIDE |
                                                          BLACK_KINGSIDE | BLACK_QUEENSIDE)};

 public:
  Position() = default;

  Bitboard getPieces(Piece piece) const {
    return pieces[piece];
  }  // Get bitboard for a specific piece
  Bitboard getPiecesByColor(Color color) const {
    return byColor[color];
  }  // Get bitboard for all pieces of a color
  Bitboard getPiecesByType(PieceType type) const {
    return byType[type];
  }  // Get bitboard for all pieces of a type
  Color getSideToMove() const {
    return sideToMove;
  }  // Get the side to move
  Square getEnPassantSquare() const {
    return enPassantSquare;
  }  // Get the en passant target square
  CastlingRights getCastlingRights() const {
    return castlingRights;
  }  // Get current castling rights

  void setPiece(Piece piece, Square sq) {
    Bitboard bb = squareBB(sq);
    pieces[piece] |= bb;
    byColor[static_cast<Color>(piece / 6)] |= bb;  // Set the bit for this color
    byType[static_cast<PieceType>(piece % 6)] |= bb;   // Set the bit for this piece type
  }

  void removePiece(Piece piece, Square sq) {
    Bitboard bb = squareBB(sq);
    pieces[piece] &= ~bb;
    byColor[static_cast<Color>(piece / 6)] &= ~bb;  // Clear the bit for this color
    byType[static_cast<PieceType>(piece % 6)] &= ~bb;   // Clear the bit for this piece type
  }
};
