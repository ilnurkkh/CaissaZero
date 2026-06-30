#pragma once

#include <array>
#include <cctype>
#include <sstream>
#include <string>

#include "types.hpp"
#include "zobrist.hpp"

class Position {
 private:
  std::array<Bitboard, PIECE_NONE> pieces{};
  std::array<Bitboard, COLOR_NONE> byColor{};
  std::array<Bitboard, PIECE_TYPE_NONE> byType{};

  Color sideToMove{};
  int halfMoveClock{0};
  int fullMoveNumber{1};
  Square enPassantSquare{SQUARE_NONE};
  CastlingRights castlingRights{static_cast<CastlingRights>(
      WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE)};
  
  // The 64-bit Zobrist hash key representing the current board state
  uint64_t hashKey{};

 public:
  Position() = default;

  Bitboard getPieces(Piece piece) const { return pieces[piece]; }
  Bitboard getPiecesByColor(Color color) const { return byColor[color]; }
  Bitboard getPiecesByType(PieceType type) const { return byType[type]; }
  Color getSideToMove() const { return sideToMove; }
  Square getEnPassantSquare() const { return enPassantSquare; }
  CastlingRights getCastlingRights() const { return castlingRights; }
  int getHalfMoveClock() const { return halfMoveClock; }
  int getFullMoveNumber() const { return fullMoveNumber; }
  uint64_t getHashKey() const { return hashKey; }

  void setPiece(Piece piece, Square sq) {
    Bitboard bb = squareBB(sq);
    pieces[piece] |= bb;
    byColor[static_cast<Color>(piece / 6)] |= bb;
    byType[static_cast<PieceType>(piece % 6)] |= bb;

    // Incremental update: XOR the piece in
    hashKey ^= ZOBRIST.pieceKeys[piece][sq];
  }

  void removePiece(Piece piece, Square sq) {
    Bitboard bb = squareBB(sq);
    pieces[piece] &= ~bb;
    byColor[static_cast<Color>(piece / 6)] &= ~bb;
    byType[static_cast<PieceType>(piece % 6)] &= ~bb;

    // Incremental update: XOR the piece out
    hashKey ^= ZOBRIST.pieceKeys[piece][sq];
  }

  
  void toggleSideToMove() {
    sideToMove = ~sideToMove;
    hashKey ^= ZOBRIST.sideKey;
  }
  
  void setEnPassantSquare(Square sq) {
    // XOR out the old en-passant state
    if (enPassantSquare != SQUARE_NONE) {
      hashKey ^= ZOBRIST.enPassantKeys[getFile(enPassantSquare)];
    } else {
      hashKey ^= ZOBRIST.enPassantKeys[FILE_NONE];
    }
    
    enPassantSquare = sq;
    
    // XOR in the new en-passant state
    if (enPassantSquare != SQUARE_NONE) {
      hashKey ^= ZOBRIST.enPassantKeys[getFile(enPassantSquare)];
    } else {
      hashKey ^= ZOBRIST.enPassantKeys[FILE_NONE];
    }
  }
  
  void updateCastlingRights(CastlingRights cr) {
    // XOR out the old, XOR in the new
    hashKey ^= ZOBRIST.castlingKeys[castlingRights];
    castlingRights = cr;
    hashKey ^= ZOBRIST.castlingKeys[castlingRights];
  }

  // --- Utilities ---
  
  Piece getPieceAt(Square sq) const {
    Bitboard bb = squareBB(sq);
    for (size_t p = WHITE_PAWN; p <= BLACK_KING; ++p) {
      if (pieces[p] & bb) {
        return static_cast<Piece>(p);
      }
    }
    return PIECE_NONE;
  }

  // Computes the hash entirely from scratch (Used for Verification/Debugging)
  uint64_t computeHashFromScratch() const {
    uint64_t hash{};

    for (size_t sq = 0; sq < BOARD_SQUARE_COUNT; ++sq) {
      Piece p = getPieceAt(static_cast<Square>(sq));
      if (p != PIECE_NONE) {
        hash ^= ZOBRIST.pieceKeys[p][sq];
      }
    }

    if (sideToMove == BLACK) hash ^= ZOBRIST.sideKey;
    
    hash ^= ZOBRIST.castlingKeys[castlingRights];
    
    if (enPassantSquare != SQUARE_NONE) {
      hash ^= ZOBRIST.enPassantKeys[getFile(enPassantSquare)];
    } else {
      hash ^= ZOBRIST.enPassantKeys[FILE_NONE];
    }

    return hash;
  }

  // Set the position from a FEN string
  void setFEN(const std::string &fen) {
    // Clear the current position
    pieces.fill(0);
    byColor.fill(0);
    byType.fill(0);
    castlingRights = NO_CASTLING;
    enPassantSquare = SQUARE_NONE;
    hashKey = 0ULL;
    halfMoveClock = 0;
    fullMoveNumber = 1;

    // Use a stringstream to parse the FEN string
    std::istringstream fenStream(fen);
    std::string board, active, castling, enPassant, half, full;
    fenStream >> board >> active >> castling >> enPassant >> half >> full;

    // Parse the board
    int rank = RANK_8;
    int file = FILE_A;
    for (char c : board) {
      if (c == '/') {
        rank--;
        file = FILE_A;
      } else if (isdigit(c)) {
        file += c - '0';
      } else {
        Piece piece = charToPiece(c);
        if (piece != PIECE_NONE) {
          Square sq = makeSquare(static_cast<File>(file), static_cast<Rank>(rank));
          
          // Place piece physically without triggering incremental hash updates
          Bitboard bb = squareBB(sq);
          pieces[piece] |= bb;
          byColor[static_cast<Color>(piece / 6)] |= bb;
          byType[static_cast<PieceType>(piece % 6)] |= bb;
          
          file++;
        }
      }
    }

    // Parse the active color
    sideToMove = (active == "w") ? WHITE : BLACK;

    // 5. Parse Castling Rights
    if (castling != "-") {
      int rights = 0;
      if (castling.find('K') != std::string::npos) rights |= WHITE_KINGSIDE;
      if (castling.find('Q') != std::string::npos) rights |= WHITE_QUEENSIDE;
      if (castling.find('k') != std::string::npos) rights |= BLACK_KINGSIDE;
      if (castling.find('q') != std::string::npos) rights |= BLACK_QUEENSIDE;
      castlingRights = static_cast<CastlingRights>(rights);
    }

    // 6. Parse En Passant
    if (enPassant != "-") {
      File epFile = static_cast<File>(enPassant[0] - 'a');
      Rank epRank = static_cast<Rank>(enPassant[1] - '1');
      enPassantSquare = makeSquare(epFile, epRank);
    }

    // 7. Parse Clocks
    if (!half.empty()) halfMoveClock = std::stoi(half);
    if (!full.empty()) fullMoveNumber = std::stoi(full);

    // 7. Calculate the Zobrist hash ONCE at the very end
    hashKey = computeHashFromScratch();
  }
};
