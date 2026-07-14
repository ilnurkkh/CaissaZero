#pragma once

#include <array>

#include "types.hpp"

constexpr Bitboard maskKingAttacks(Square sq) {
  Bitboard king{squareBB(sq)};

  Bitboard w = (king >> 1) & NOT_H_FILE;
  Bitboard e = (king << 1) & NOT_A_FILE;
  Bitboard n = king << 8;
  Bitboard s = king >> 8;
  Bitboard nw = (king << 7) & NOT_H_FILE;
  Bitboard ne = (king << 9) & NOT_A_FILE;
  Bitboard sw = (king >> 9) & NOT_H_FILE;
  Bitboard se = (king >> 7) & NOT_A_FILE;

  return w | e | n | s | nw | ne | sw | se;
}

constexpr Bitboard maskKnightAttacks(Square sq) {
  Bitboard knight{squareBB(sq)};

  Bitboard nnw = (knight << 15) & NOT_H_FILE;
  Bitboard nne = (knight << 17) & NOT_A_FILE;
  Bitboard ssw = (knight >> 17) & NOT_H_FILE;
  Bitboard sse = (knight >> 15) & NOT_A_FILE;
  Bitboard wwn = (knight << 6) & NOT_GH_FILE;
  Bitboard wws = (knight >> 10) & NOT_GH_FILE;
  Bitboard een = (knight << 10) & NOT_AB_FILE;
  Bitboard ees = (knight >> 6) & NOT_AB_FILE;

  return nnw | nne | ssw | sse | wwn | wws | een | ees;
}

constexpr Bitboard maskPawnAttacks(Color color, Square sq) {
  Bitboard pawn{squareBB(sq)};
  Bitboard attacks{};

  if (color == WHITE) {
    // White pawns attack North-West (+7) and North-East (+9)
    attacks |= (pawn << 7) & NOT_H_FILE;
    attacks |= (pawn << 9) & NOT_A_FILE;
  } else {
    // Black pawns attack South-West (-9) and South-East (-7)
    attacks |= (pawn >> 9) & NOT_H_FILE;
    attacks |= (pawn >> 7) & NOT_A_FILE;
  }

  return attacks;
}

// Generates a bitboard of all squares strictly between sq1 and sq2 if they are
// aligned
constexpr Bitboard maskBetween(Square sq1, Square sq2) {
  if (sq1 == sq2) return 0ULL;

  int r1{getRank(sq1)};
  int f1{getFile(sq1)};
  int r2{getRank(sq2)};
  int f2{getFile(sq2)};

  int dr{r2 - r1};
  int df{f2 - f1};

  // Rays are only valid on shared ranks, files, or diagonals
  if (dr != 0 && df != 0 && (dr != df && dr != -df)) {
    return 0ULL;
  }

  auto sign = [](int val) { return (0 < val) - (val < 0); };
  int stepR = sign(dr);
  int stepF = sign(df);

  Bitboard bb{};
  int r{r1 + stepR};
  int f{f1 + stepF};

  while (r != r2 || f != f2) {
    bb |= squareBB(
        makeSquare(static_cast<File>(f),
                   static_cast<Rank>(r)));  // Add the square to the bitboard
    r += stepR;
    f += stepF;
  }

  return bb;
}

// Generates a bitboard representing the entire line/ray passing through sq1 and
// sq2
constexpr Bitboard maskLine(Square sq1, Square sq2) {
  if (sq1 == sq2) return 0ULL;

  int r1{getRank(sq1)};
  int f1{getFile(sq1)};
  int r2{getRank(sq2)};
  int f2{getFile(sq2)};

  int dr{r2 - r1};
  int df{f2 - f1};

  // Check alignment
  if (dr != 0 && df != 0 && (dr != df && dr != -df)) {
    return 0ULL;
  }

  auto sign = [](int val) { return (0 < val) - (val < 0); };
  int stepR = sign(dr);
  int stepF = sign(df);

  Bitboard bb{};

  // Walk forward from sq1 to the edge of the board
  int r{r1};
  int f{f1};
  while (r >= 0 && r < 8 && f >= 0 && f < 8) {
    bb |= squareBB(makeSquare(static_cast<File>(f), static_cast<Rank>(r)));
    r += stepR;
    f += stepF;
  }

  // Walk backward from sq1 to the other edge
  r = r1 - stepR;
  f = f1 - stepF;
  while (r >= 0 && r < 8 && f >= 0 && f < 8) {
    bb |= squareBB(makeSquare(static_cast<File>(f), static_cast<Rank>(r)));
    r -= stepR;
    f -= stepF;
  }

  return bb;
}

constexpr std::array<Bitboard, BOARD_SQUARE_COUNT> precomputeKingAttacks() {
  std::array<Bitboard, BOARD_SQUARE_COUNT> table{};
  for (size_t sq{}; sq < BOARD_SQUARE_COUNT; ++sq) {
    table[sq] = maskKingAttacks(static_cast<Square>(sq));
  }
  return table;
}

constexpr std::array<Bitboard, BOARD_SQUARE_COUNT> precomputeKnightAttacks() {
  std::array<Bitboard, BOARD_SQUARE_COUNT> table{};
  for (size_t sq{}; sq < BOARD_SQUARE_COUNT; ++sq) {
    table[sq] = maskKnightAttacks(static_cast<Square>(sq));
  }
  return table;
}

constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>, COLOR_NONE>
precomputePawnAttacks() {
  std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>, COLOR_NONE> table{};
  for (size_t sq{}; sq < BOARD_SQUARE_COUNT; ++sq) {
    table[WHITE][sq] = maskPawnAttacks(WHITE, static_cast<Square>(sq));
    table[BLACK][sq] = maskPawnAttacks(BLACK, static_cast<Square>(sq));
  }
  return table;
}

constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>,
                     BOARD_SQUARE_COUNT>
precomputeBetween() {
  std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>, BOARD_SQUARE_COUNT>
      table{};
  for (size_t s1{}; s1 < BOARD_SQUARE_COUNT; ++s1) {
    for (size_t s2{}; s2 < BOARD_SQUARE_COUNT; ++s2) {
      table[s1][s2] =
          maskBetween(static_cast<Square>(s1), static_cast<Square>(s2));
    }
  }
  return table;
}

constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>,
                     BOARD_SQUARE_COUNT>
precomputeLine() {
  std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>, BOARD_SQUARE_COUNT>
      table{};
  for (size_t s1{}; s1 < BOARD_SQUARE_COUNT; ++s1) {
    for (size_t s2{}; s2 < BOARD_SQUARE_COUNT; ++s2) {
      table[s1][s2] =
          maskLine(static_cast<Square>(s1), static_cast<Square>(s2));
    }
  }
  return table;
}

constexpr std::array<Bitboard, BOARD_SQUARE_COUNT> KING_ATTACKS{
    precomputeKingAttacks()};
constexpr std::array<Bitboard, BOARD_SQUARE_COUNT> KNIGHT_ATTACKS{
    precomputeKnightAttacks()};    
constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>, COLOR_NONE>
    PAWN_ATTACKS{precomputePawnAttacks()};
constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>,
                     BOARD_SQUARE_COUNT>
    BETWEEN_BB{precomputeBetween()};
constexpr std::array<std::array<Bitboard, BOARD_SQUARE_COUNT>,
                     BOARD_SQUARE_COUNT>
    LINE_BB{precomputeLine()};

constexpr std::array<int, 4> ROOK_DIRS = { 8, -8, 1, -1 };
constexpr std::array<int, 4> BISHOP_DIRS = { 9, 7, -7, -9 };

inline bool canStep(Square sq, int dir) {
  const File f = getFile(sq);
  const Rank r = getRank(sq);

  switch (dir) {
    case 8:  return r != RANK_8;
    case -8: return r != RANK_1;
    case 1:  return f != FILE_H;
    case -1: return f != FILE_A;
    case 9:  return r != RANK_8 && f != FILE_H;
    case 7:  return r != RANK_8 && f != FILE_A;
    case -7: return r != RANK_1 && f != FILE_H;
    case -9: return r != RANK_1 && f != FILE_A;
    default: return false;
  }
}

inline Bitboard rayAttacks(Square from, std::span<const int> dirs, Bitboard occ) {
  Bitboard attacks = 0;
  for (const int dir : dirs) {
    Square sq = from;
    while (canStep(sq, dir)) {
      sq = static_cast<Square>(sq + dir);
      attacks |= squareBB(sq);
      if (occ & squareBB(sq)) break;
    }
  }
  return attacks;
}

inline Bitboard getRookAttacks(Square s, Bitboard occ) {
  return rayAttacks(s, ROOK_DIRS, occ);
}

inline Bitboard getBishopAttacks(Square s, Bitboard occ) {
  return rayAttacks(s, BISHOP_DIRS, occ);
}

inline Bitboard getQueenAttacks(Square s, Bitboard occ) {
  return getRookAttacks(s, occ) | getBishopAttacks(s, occ);
}
