#pragma once

#include "position.hpp"
#include "types.hpp"

namespace Tests {

// Debug output utilities
void printBB(Bitboard bb);
void printPosition(const Position& pos);

// Internal state validation
void verifyPositionIntegrity(const Position& pos);

// Phase 1 Component Tests
void testCheckpoint1_1();
void testCheckpoint1_2();
void testCheckpoint1_3();
void testCheckpoint1_4();
void testCheckpoint1_5();

// Phase 1 Integration Test
void runPhase1ExitGate();

}  // namespace Tests
