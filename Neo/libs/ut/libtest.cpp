// Copyright 2009 Google Inc. All Rights Reserved.

// This sample shows how to use Google Test listener API to implement
// a primitive leak checker.

#include <stdio.h>
#include <stdlib.h>

#include "gtest/gtest.h"
#include "../neomacro.h"

using namespace NEO_LIB;

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestCase;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::UnitTest;
/*
 * 1.
TEST(FooTest, HandleNoneZeroInput)
{
    EXPECT_EQ(2, Foo(4, 10));
    EXPECT_EQ(6, Foo(30, 18));
}
2.ASSERT_TRUE(condition); 	EXPECT_TRUE(condition); 	condition is true
ASSERT_FALSE(condition); 	EXPECT_FALSE(condition); 	condition is false
ASSERT_EQ(expected, actual); 	EXPECT_EQ(expected, actual); 	expected == actual
ASSERT_NE(val1, val2); 	EXPECT_NE(val1, val2); 	val1 != val2
ASSERT_LT(val1, val2); 	EXPECT_LT(val1, val2); 	val1 < val2
ASSERT_LE(val1, val2); 	EXPECT_LE(val1, val2); 	val1 <= val2
ASSERT_GT(val1, val2); 	EXPECT_GT(val1, val2); 	val1 > val2
ASSERT_GE(val1, val2); 	EXPECT_GE(val1, val2); 	val1 >= val2
SSERT_STREQ(expected_str, actual_str); 	EXPECT_STREQ(expected_str, actual_str); 	the two C strings have the same content
ASSERT_STRNE(str1, str2); 	EXPECT_STRNE(str1, str2); 	the two C strings have different content
ASSERT_STRCASEEQ(expected_str, actual_str); 	EXPECT_STRCASEEQ(expected_str, actual_str); 	the two C strings have the same content, ignoring case
ASSERT_STRCASENE(str1, str2); 	EXPECT_STRCASENE(str1, str2); 	the two C strings have different content, ignoring case
直接返回成功：SUCCEED();

返回失败：
Fatal assertion 	Nonfatal assertion
FAIL(); 	ADD_FAILURE();
3 TestSuite事件

我们需要写一个类，继承testing::Test，然后实现两个静态方法

1. SetUpTestCase() 方法在第一个TestCase之前执行

2. TearDownTestCase() 方法在最后一个TestCase之后执行

复制代码
class FooTest : public testing::Test {
 protected:
  static void SetUpTestCase() {
    shared_resource_ = new ;
  }
  static void TearDownTestCase() {
    delete shared_resource_;
    shared_resource_ = NULL;
  }
  // Some expensive resource shared by all tests.
  static T* shared_resource_;
};
复制代码

在编写测试案例时，我们需要使用TEST_F这个宏，第一个参数必须是我们上面类的名字，代表一个TestSuite。

复制代码
TEST_F(FooTest, Test1)
 {
    // you can refer to shared_resource here
}
TEST_F(FooTest, Test2)
 {
    // you can refer to shared_resource here
}

4TestCase事件

TestCase事件是挂在每个案例执行前后的，实现方式和上面的几乎一样，不过需要实现的是SetUp方法和TearDown方法：

1. SetUp()方法在每个TestCase之前执行

2. TearDown()方法在每个TestCase之后执行
复制代码
class FooCalcTest:public testing::Test
{
protected:
    virtual void SetUp()
    {
        m_foo.Init();
    }
    virtual void TearDown()
    {
        m_foo.Finalize();
    }

    FooCalc m_foo;
};

TEST_F(FooCalcTest, HandleNoneZeroInput)
{
    EXPECT_EQ(4, m_foo.Calc(12, 16));
}

TEST_F(FooCalcTest, HandleNoneZeroInput_Error)
{
    EXPECT_EQ(5, m_foo.Calc(12, 16));
}
 *
 * */

namespace {

 char *sz_buf = NULL;
 const  char *str = "Test point are null";
 char sz_buf1[0] ; //是是一个指针，可以态态展展
const char *str1 = "Test buf is 0";
 char sz_buf2[24];
const  char *str2 = "Test buf =24 eq len= 24";
 char sz_buf3[50];
 const char *str3 = "Test buf =50 gt len= 24";
 char sz_buf4[10];
const  char *str4 = "Test buf =10 lt len= 24";



  TEST(SafePrintTest, All)
  {
      EXPECT_EQ(0, SafePrintf(sz_buf, 50, "Test point are null"));
      EXPECT_STRNE(str, sz_buf);

      EXPECT_EQ(13, SafePrintf(sz_buf1, 50, "Test buf is 0"));
      EXPECT_STREQ(str1, sz_buf1);

      EXPECT_EQ(23, SafePrintf(sz_buf2, 24, "Test buf =%d eq len= %d", sizeof(sz_buf2), strlen(str2)+1));
      EXPECT_STREQ(str2, sz_buf2);

      EXPECT_EQ(23, SafePrintf(sz_buf3, 50, "Test buf =%d gt len= %d", sizeof(sz_buf3), strlen(str3)+1));
       EXPECT_STREQ(str3, sz_buf3);

       EXPECT_EQ(9, SafePrintf(sz_buf4, 10, "Test buf =%d lt len= %d", sizeof(sz_buf4), strlen(str4)+1));
       EXPECT_STRNE(str4, sz_buf4);
  }

  TEST(TimelsUpTest, All)
  {
      //EXPECT_TRUE(TimelsUp(100, 50));
  }

TEST(GetTimeStampTest, all)
{
      EXPECT_EQ(16, GetTimeStamp(sz_buf1, 17));
      printf("%s\n", sz_buf1);
      EXPECT_EQ(25, GetTimeStamp(sz_buf1, 50));
       printf("%s\n", sz_buf1);
       EXPECT_EQ(9, GetTimeStamp(sz_buf1, 10));
       printf("%s\n", sz_buf1);
}

  TEST(NEOMinSleepTest, All)
   {
       //EXPECT_TRUE();
   }

  TEST(GetRandomTest, All)
   {
        EXPECT_EQ(0, Get0());
        EXPECT_NE(GetNot0(), GetNot0());
        EXPECT_LE(0, GetRandomBetween(0, 50));
        EXPECT_GE(50, GetRandomBetween(0, 50));
        EXPECT_LE(37, GetRandomBetween(38, 37));
        EXPECT_GE(38, GetRandomBetween(38, 37));
        EXPECT_EQ(0, GetRandomBetween(0, 0));
        EXPECT_EQ(999, GetRandomBetween(999, 999));
   }

  TEST(container_ofTest, All)
  {
        typedef struct test
        {
            void *p;
            void *p1;
            int a;
            void *p2;
        } Test;
        Test t =  {NULL, NULL, 4, NULL};
        void *m = &t;
        void *n  = container_of(&t.p2, Test, p2);
        printf("%p\n", &t);
        printf("%p\n", container_of(&t.p2, Test, p2));
        printf("%p\n", container_of(&t.p1, Test, p1));
        printf("%p\n", container_of(&t.p, Test, p));
        printf("%p\n", container_of(&t.a, Test, a));
        EXPECT_EQ(m, n);
        n  = container_of(&t.p1, Test, p1);
        EXPECT_EQ(m, n);
        n  = container_of(&t.p, Test, p);
        EXPECT_EQ(m, n);
        n  = container_of(&t.a, Test, a);
        EXPECT_EQ(m, n);
   }

  TEST(FULL_NAMETest, All)
   {
      char fullname[0];
      FULL_NAME(fullname, "/home/neo", "fuck", "exe");
      EXPECT_STREQ(fullname, "/home/neo/fuck.exe");
      FULL_NAME(fullname, "/home/neo", "fuck", "");
      EXPECT_STREQ(fullname, "/home/neo/fuck");
      FULL_NAME(fullname, "", "fuck", "exe");
      EXPECT_STREQ(fullname, "fuck.exe");
      FULL_NAME(fullname, "", "fuck", "");
       EXPECT_STREQ(fullname, "fuck");
       FULL_NAME(fullname, "", "", "");
       EXPECT_STREQ(fullname, "");
       FULL_NAME(fullname, "/home/neo", "", "exe");
        EXPECT_STREQ(fullname, "/home/neo/.exe");
   }
}  // namespace

/*int main(int argc, char **argv) {
  InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}*/
