#include <gtest/gtest.h>

// Basic test to ensure the framework is set up correctly
TEST(SetupTest, HelloWorld) {
    ASSERT_EQ(7, 7);
}

// You can start adding tests for your System namespace here.
// For example:
// #include "System/IWindow.h"

// TEST(SystemTest, MyFirstTest) {
//   // ... your test logic
// }

// The main function that runs all the tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}