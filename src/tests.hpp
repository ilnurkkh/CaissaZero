#pragma once

#include "position.hpp"
#include "types.hpp"

namespace Tests {

void printBB(Bitboard bb);
void printPosition(const Position& pos);

void verifyPositionIntegrity(const Position& pos);

void testCheckpoint1_1();
void testCheckpoint1_2();
void testCheckpoint1_3();
void testCheckpoint1_4();
void testCheckpoint1_5();
void runPhase1ExitGate();

void testCheckpoint2_2();
void testCheckpoint2_3();

}
