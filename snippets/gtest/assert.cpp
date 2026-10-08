#include <gtest/gtest.h>

TEST(Foo, Top)
{
	ASSERT_EQ(6, 6);
	ASSERT_NE(1, 2);
	ASSERT_LT(1, 6);
	ASSERT_LE(1, 6);
	ASSERT_GT(6, 1);
	ASSERT_GE(6, 1);
	ASSERT_TRUE(true);
	ASSERT_FALSE(false);
}

TEST(Foo, Bottom)
{
	EXPECT_TRUE(true);
	EXPECT_FALSE(false);
	ASSERT_EQ('a', 'a');
}

TEST(Bar, Main)
{
	ASSERT_STREQ("hello", "hello");
	ASSERT_STRNE("hello", "world");
	ASSERT_STRCASEEQ("hello", "HellO");
	ASSERT_STRCASENE("hello", "WORLD");
	ASSERT_FLOAT_EQ(1.3, 1.3);
	ASSERT_DOUBLE_EQ(1.3, 1.3);
	ASSERT_NEAR(1.3, 1.4, 0.1);
}

int main(int argc, char *argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
