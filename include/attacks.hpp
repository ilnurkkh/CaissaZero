#pragma once

#include <array>

#include "types.hpp"

// --- ATTACK TABLES ---

// Generates a bitboard of all squares attacked by a king on the given square
constexpr Bitboard maskKingAttacks(Square sq) {
  Bitboard king{squareBB(sq)};

  // Generate all possible king moves by shifting the bitboard in all 8
  // directions
  Bitboard west = (king >> 1) & NOT_H_FILE;
  Bitboard east = (king << 1) & NOT_A_FILE;
  Bitboard north = king << 8;
  Bitboard south = king >> 8;
  Bitboard northWest = (king << 7) & NOT_H_FILE;
  Bitboard northEast = (king << 9) & NOT_A_FILE;
  Bitboard southWest = (king >> 9) & NOT_H_FILE;
  Bitboard southEast = (king >> 7) & NOT_A_FILE;

  // Return the combined bitboard of all attacked squares
  return west | east | north | south | northWest | northEast | southWest |
         southEast;
}

// Generates a bitboard of all squares attacked by a knight on the given square
constexpr Bitboard maskKnightAttacks(Square sq) {
  Bitboard knight{squareBB(sq)};

  // Generate all possible knight moves by shifting the bitboard in all 8
  // L-shaped directions
  Bitboard northNorthWest = (knight << 15) & NOT_H_FILE;
  Bitboard northNorthEast = (knight << 17) & NOT_A_FILE;
  Bitboard southSouthWest = (knight >> 17) & NOT_H_FILE;
  Bitboard southSouthEast = (knight >> 15) & NOT_A_FILE;
  Bitboard westWestNorth = (knight << 6) & NOT_GH_FILE;
  Bitboard westWestSouth = (knight >> 10) & NOT_GH_FILE;
  Bitboard eastEastNorth = (knight << 10) & NOT_AB_FILE;
  Bitboard eastEastSouth = (knight >> 6) & NOT_AB_FILE;

  // Return the combined bitboard of all attacked squares
  return northNorthWest | northNorthEast | southSouthWest | southSouthEast |
         westWestNorth | westWestSouth | eastEastNorth | eastEastSouth;
}

// Generates a bitboard of all squares attacked by a pawn of the given color on
// the given square
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

  // Check if they are aligned on the same Rank, File, or Diagonal
  if (dr != 0 && df != 0 && (dr != df && dr != -df)) {
    return 0ULL;  // Not aligned, return an empty bitboard
  }

  int stepR{(dr == 0) ? 0 : (dr > 0 ? 1 : -1)};
  int stepF{(df == 0) ? 0 : (df > 0 ? 1 : -1)};

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
    return 0ULL;  // Not aligned, return an empty bitboard
  }

  int stepR{(dr == 0) ? 0 : (dr > 0 ? 1 : -1)};
  int stepF{(df == 0) ? 0 : (df > 0 ? 1 : -1)};

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

// --- COMPILE-TIME TABLE INITIALIZATION ---

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

// --- GLOBAL PRECOMPUTED TABLES ---

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
