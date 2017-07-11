#ifndef YAFSM_H
#define YAFSM_H

#include <vector>
#include <string>
class YaFsm
{
public:
  YaFsm();

  static void printDbg(const std::string&);
  static void printWarn(const std::string&);
  static void printFatal(const std::string&);

  static std::vector<std::string> split(const std::string &s, char delim);

private:
  static std::vector<std::string>& split(const std::string &s, char delim,std::vector<std::string> &elems);

};

#endif // YAFSM_H
