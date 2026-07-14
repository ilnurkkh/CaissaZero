#include <iostream>

#include "tests.hpp"

int main() {
  std::cout << "Caissa Zero - Test Suite\n";
  std::cout << "========================\n\n";

  Tests::testCheckpoint1_1();
  Tests::testCheckpoint1_2();
  Tests::testCheckpoint1_3();
  Tests::testCheckpoint1_4();
  Tests::testCheckpoint1_5();
  Tests::runPhase1ExitGate();

  Tests::testCheckpoint2_2();
  Tests::testCheckpoint2_3();
  Tests::testCheckpoint2_4();

  return 0;
}
