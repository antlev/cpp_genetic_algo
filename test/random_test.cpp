#include "gtest/gtest.h"
#include "../src/genetic.h"

TEST (getRealBetweenOneAndZero, toto) { 
	MyRandom random;
	double realBetweenOneAndZero = random.getRealBetweenOneAndZero();
	EXPECT_TRUE (0 <= realBetweenOneAndZero);
	EXPECT_TRUE (1 >= realBetweenOneAndZero);
}
TEST (getIntAsciiChar, toto) { 
	MyRandom random;
	char intAsciiChar = random.getIntAsciiChar();
	EXPECT_TRUE (MyRandom::ASCII_DOWN_LIMIT <= intAsciiChar);
	EXPECT_TRUE (MyRandom::ASCII_UP_LIMIT >= intAsciiChar);
}
TEST (getIntRange, toto) { 
	MyRandom random;
	int downlimit=0;
	int uplimit=10;
	int intRange = random.getIntRange(downlimit, uplimit);
	EXPECT_TRUE (downlimit <= intRange);
	EXPECT_TRUE (uplimit >= intRange);
}
