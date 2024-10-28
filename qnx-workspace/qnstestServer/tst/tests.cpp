/*
 * tests.cpp
 *
 *  Created on: 28.10.2024
 *      Author: Marc
 */

#include "gtest/gtest.h"
#include "../src/qnstest.cpp"
#include <errno.h>

TEST(ClientTest, CheckSuccessfullExit){
	printf("Running Server Test ... \n");
	EXPECT_EQ(server(), EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



