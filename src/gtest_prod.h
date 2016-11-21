// This is just a local copy of googletest gtest_prod.h, to avoid
// the dependency on the gtest library for production only sources.

#ifndef GTEST_INCLUDE_GTEST_GTEST_PROD_H_
#define GTEST_INCLUDE_GTEST_GTEST_PROD_H_
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test
#endif  // GTEST_INCLUDE_GTEST_GTEST_PROD_H_

