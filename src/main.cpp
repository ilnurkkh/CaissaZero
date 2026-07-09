#include <iostream>

#include "tests.hpp"

int main() {
  std::cout << "Caissa Zero - Test Suite\n";
  std::cout << "========================\n\n";

  // Phase 1 Unit Tests
  Tests::testCheckpoint1_1();
  Tests::testCheckpoint1_2();
  Tests::testCheckpoint1_3();
  Tests::testCheckpoint1_4();
  Tests::testCheckpoint1_5();

  // Phase 1 Integration Test
  Tests::runPhase1ExitGate();

  // Phase 2 Unit Tests
  Tests::testCheckpoint2_2();

  return 0;
}
