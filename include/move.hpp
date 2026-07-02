#pragma once

#include <cstdint>

#include "types.hpp"

enum MoveFlag : uint16_t {
  FLAG_QUIET = 0,
  FLAG_DOUBLE_PAWN_PUSH = 1,
  FLAG_KING_CASTLE = 2,
  FLAG_QUEEN_CASTLE = 3,

  // All capture flags have the 2nd bit set
  FLAG_CAPTURE = 4,
  FLAG_EP_CAPTURE = 5,

  // All promotion flags have the 3rd bit set
  FLAG_PROMOTE_KNIGHT = 8,
  FLAG_PROMOTE_BISHOP = 9,
  FLAG_PROMOTE_ROOK = 10,
  FLAG_PROMOTE_QUEEN = 11,

  FLAG_PROMOTE_KNIGHT_CAPTURE = 12,
  FLAG_PROMOTE_BISHOP_CAPTURE = 13,
  FLAG_PROMOTE_ROOK_CAPTURE = 14,
  FLAG_PROMOTE_QUEEN_CAPTURE = 15
};

class Move {
 private:
  uint16_t data;  // 16 bits: [from(6) | to(6) | flags(4)]

 public:
  // --- Constructors ---

  constexpr Move() : data(0) {}

  constexpr Move(Square from, Square to, MoveFlag flags = FLAG_QUIET)
      : data{static_cast<uint16_t>((from << 10) | (to << 4) | flags)} {}

  explicit constexpr Move(uint16_t rawData) : data(rawData) {}

  // --- Getters ---
  constexpr Square getFrom() const {
    return static_cast<Square>((data >> 10) & 0x3F);  // Extract bits 10-15
  }

  constexpr Square getTo() const {
    return static_cast<Square>((data >> 4) & 0x3F);  // Extract bits 4-9
  }

  constexpr uint16_t getFlags() const {
    return data & 0x0F;  // Extract bits 0-3 (No shift needed!)
  }

  constexpr uint16_t getRaw() const { return data; }

  // --- Utilities ---

  // A move is a capture if the 3rd flag bit (value 4) is set.
  constexpr bool isCapture() const { return data & 4; }

  // A move is a promotion if the 4th flag bit (value 8) is set.
  constexpr bool isPromotion() const { return data & 8; }

  // Returns the PieceType of the promotion piece, assuming the move IS a
  // promotion
  constexpr PieceType getPromotionPieceType() const {
    // Isolate the lowest 2 bits of the flag and map them to Knight (1), Bishop
    // (2), etc.
    return static_cast<PieceType>((data & 3) + KNIGHT);
  }

  // --- Operators ---

  constexpr bool operator==(const Move& other) const {
    return data == other.data;
  }
  constexpr bool operator!=(const Move& other) const {
    return data != other.data;
  }
};

// Represents a null/invalid move (from A1 to A1 with quiet flag is equivalent to 0)
inline constexpr Move MOVE_NONE = Move();

// --- Move List ---
// A highly optimized, fixed-size container for move generation.
// This completely avoids heap allocations (std::vector) during the search hot path.

constexpr size_t MAX_MOVES = 256; // Theoretical maximum is ~218

class MoveList {
 private:
  std::array<Move, MAX_MOVES> moves{};
  size_t count{0};

 public:
  MoveList() = default;

  void push_back(Move move) {
    // In a release build, we skip boundary checking for maximum speed, 
    // as 256 is safely above the theoretical maximum of chess moves.
    moves[count++] = move;
  }

  size_t size() const { 
    return count; 
  }

  bool empty() const {
    return count == 0;
  }

  // Array access operators
  Move& operator[](size_t index) { return moves[index]; }
  const Move& operator[](size_t index) const { return moves[index]; }

  // C++ Iterator support allowing range-based loops: for(Move m : moveList) { ... }
  auto begin() { return moves.begin(); }
  auto end() { return moves.begin() + count; }
  auto begin() const { return moves.begin(); }
  auto end() const { return moves.begin() + count; }
};
