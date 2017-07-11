#ifndef YAFSMCPPTEST_H
#define YAFSMCPPTEST_H

#include <string>

class YaFsmCppTest
{
public:
  YaFsmCppTest();

  void runtests();

private:
  void test( const std::string& actual, const std::string expected);
  void testDataModel();

};

#endif // YAFSMCPPTEST_H
