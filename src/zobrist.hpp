#pragma once

#include <random>
#include <array>

#include "types.hpp"

inline constexpr uint64_t ZOBRIST_SEED = 0x8A25F71B3C9DE046ULL;

struct Zobrist {
    std::array<std::array<uint64_t, BOARD_SQUARE_COUNT>, PIECE_NONE> pieceKeys{};
    std::array<uint64_t, 16> castlingKeys{};
    std::array<uint64_t, 9> enPassantKeys{};
    uint64_t sideKey{};

    Zobrist() {
        std::mt19937_64 rng(ZOBRIST_SEED);

        for (auto& pieceArray : pieceKeys)
            for (uint64_t& key : pieceArray) key = rng();

        for (uint64_t& key : castlingKeys) key = rng();
        for (uint64_t& key : enPassantKeys) key = rng();
        sideKey = rng();
    }
};

inline Zobrist ZOBRIST;
