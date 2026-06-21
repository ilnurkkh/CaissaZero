#pragma once

#include <array>
#include <cstdint>

// --- Type Aliases ---

using Bitboard = uint64_t;
// using Move = uint16_t;

// --- Constants ---

constexpr int BOARD_SQUARE_COUNT = 64;

// --- Enums ---

enum Square : uint8_t {
  A1,
  B1,
  C1,
  D1,
  E1,
  F1,
  G1,
  H1,
  A2,
  B2,
  C2,
  D2,
  E2,
  F2,
  G2,
  H2,
  A3,
  B3,
  C3,
  D3,
  E3,
  F3,
  G3,
  H3,
  A4,
  B4,
  C4,
  D4,
  E4,
  F4,
  G4,
  H4,
  A5,
  B5,
  C5,
  D5,
  E5,
  F5,
  G5,
  H5,
  A6,
  B6,
  C6,
  D6,
  E6,
  F6,
  G6,
  H6,
  A7,
  B7,
  C7,
  D7,
  E7,
  F7,
  G7,
  H7,
  A8,
  B8,
  C8,
  D8,
  E8,
  F8,
  G8,
  H8,
  SQUARE_NONE = 64
};

enum File : uint8_t {
  FILE_A,
  FILE_B,
  FILE_C,
  FILE_D,
  FILE_E,
  FILE_F,
  FILE_G,
  FILE_H,
  FILE_NONE = 8
};

enum Rank : uint8_t {
  RANK_1,
  RANK_2,
  RANK_3,
  RANK_4,
  RANK_5,
  RANK_6,
  RANK_7,
  RANK_8,
  RANK_NONE = 8
};

enum Color : uint8_t { WHITE, BLACK, COLOR_NONE = 2 };

enum PieceType : uint8_t {
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING,
  PIECE_TYPE_NONE = 6
};

enum Piece : uint8_t {
  WHITE_PAWN,
  WHITE_KNIGHT,
  WHITE_BISHOP,
  WHITE_ROOK,
  WHITE_QUEEN,
  WHITE_KING,
  BLACK_PAWN,
  BLACK_KNIGHT,
  BLACK_BISHOP,
  BLACK_ROOK,
  BLACK_QUEEN,
  BLACK_KING,
  PIECE_NONE = 12
};

enum CastlingRights : uint8_t {
  WHITE_KINGSIDE = 0b0001,
  WHITE_QUEENSIDE = 0b0010,
  BLACK_KINGSIDE = 0b0100,
  BLACK_QUEENSIDE = 0b1000
};

// --- Square Decomposition ---

constexpr File getFile(Square sq) { return static_cast<File>(sq & 7); }

constexpr Rank getRank(Square sq) { return static_cast<Rank>(sq >> 3); }

constexpr Square makeSquare(File file, Rank rank) {
  return static_cast<Square>((rank << 3) | file);
}

// --- Square Bitboard Primitive ---
constexpr Bitboard squareBB(Square sq) { return 1ULL << sq; }

// --- Precomputed Bitboard Masks ---

constexpr std::array<Bitboard, 8> RANK_BB = {
    0x00000000000000FFULL,  // Rank 1
    0x000000000000FF00ULL,  // Rank 2
    0x0000000000FF0000ULL,  // Rank 3
    0x00000000FF000000ULL,  // Rank 4
    0x000000FF00000000ULL,  // Rank 5
    0x0000FF0000000000ULL,  // Rank 6
    0x00FF000000000000ULL,  // Rank 7
    0xFF00000000000000ULL   // Rank 8
};

constexpr std::array<Bitboard, 8> FILE_BB = {
    0x0101010101010101ULL,  // File A
    0x0202020202020202ULL,  // File B
    0x0404040404040404ULL,  // File C
    0x0808080808080808ULL,  // File D
    0x1010101010101010ULL,  // File E
    0x2020202020202020ULL,  // File F
    0x4040404040404040ULL,  // File G
    0x8080808080808080ULL   // File H
};

// Board wrap prevention masks
constexpr Bitboard NOT_A_FILE = ~FILE_BB[FILE_A];
constexpr Bitboard NOT_H_FILE = ~FILE_BB[FILE_H];
constexpr Bitboard NOT_AB_FILE = ~(FILE_BB[FILE_A] | FILE_BB[FILE_B]);
constexpr Bitboard NOT_GH_FILE = ~(FILE_BB[FILE_G] | FILE_BB[FILE_H]);

// Board pattern masks
constexpr Bitboard LIGHT_SQUARES_BB = 0x55AA55AA55AA55AAULL;
constexpr Bitboard DARK_SQUARES_BB = 0xAA55AA55AA55AA55ULL;
constexpr Bitboard MAIN_DIAGONAL_BB = 0x8040201008040201ULL;  // A1 to H8
constexpr Bitboard ANTI_DIAGONAL_BB = 0x0102040810204080ULL;  // H1 to A8

// Board zone masks
constexpr Bitboard QUEENSIDE_BB =
    FILE_BB[FILE_A] | FILE_BB[FILE_B] | FILE_BB[FILE_C] | FILE_BB[FILE_D];
constexpr Bitboard KINGSIDE_BB =
    FILE_BB[FILE_E] | FILE_BB[FILE_F] | FILE_BB[FILE_G] | FILE_BB[FILE_H];
constexpr Bitboard CENTER_SQUARES_BB =
    squareBB(D4) | squareBB(E4) | squareBB(D5) | squareBB(E5);
constexpr Bitboard EXTENDED_CENTER_BB =
    squareBB(C3) | squareBB(D3) | squareBB(E3) | squareBB(F3) | squareBB(C4) |
    squareBB(D4) | squareBB(E4) | squareBB(F4) | squareBB(C5) | squareBB(D5) |
    squareBB(E5) | squareBB(F5) | squareBB(C6) | squareBB(D6) | squareBB(E6) |
    squareBB(F6);

// --- Bit Manipulation API ---

// Finds the index of the lowest set bit (the first piece on the board)
inline Square lsb(Bitboard bb) {
  return static_cast<Square>(__builtin_ctzll(bb));
}

// Finds the index of the highest set bit
inline Square msb(Bitboard bb) {
  return static_cast<Square>(63 ^ __builtin_clzll(bb));
}

// Finds the lowest set bit, completely removes it from the bitboard, and
// returns the square
inline Square popLsb(Bitboard &bb) {
  Square sq = lsb(bb);
  bb &= bb - 1;  // Clear the least significant bit
  return sq;
}

// Instantly counts set bits (how many pieces are on this board)
inline int popCount(Bitboard bb) { return __builtin_popcountll(bb); }
