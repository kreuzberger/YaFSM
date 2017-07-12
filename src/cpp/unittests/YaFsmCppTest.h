#ifndef YAFSMCPPTEST_H
#define YAFSMCPPTEST_H

#include <string>

class YaFsmCppTest
{
public:
  YaFsmCppTest();

  void runtests();

private:
  void test( const std::string& actual, const std::string expected, const std::string& testname);
  void test( size_t actual, size_t expected, const std::string& testname);
  void test( bool test, const std::string& testname);
  void testDataModel();
  void testDataModelNode();
  void testRootStates();
  void testSubStates();
  void testEvents();

};

#endif // YAFSMCPPTEST_H
