#include "pch.h"
#include "validator.h"


TEST(ValidPassword) {
	EXPECT_TRUE(isValidPassword("23asdfgh"));
}

TEST(ValidLogin) {
	EXPECT_TRUE(isValidLogin("aa"));
}