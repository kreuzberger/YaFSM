#include "YaFsm.h"

#include <sstream>
#include <iostream>

#ifdef __unix__
std::string YaFsm::sep = "/";
#else
std::string YaFsm::sep = "\\";
#endif

YaFsm::YaFsm()
{
}

std::vector<std::string>& YaFsm::split( const std::string& s, char delim, std::vector<std::string>& elems )
{
  std::stringstream ss( s );
  std::string       item;
  while ( std::getline( ss, item, delim ) )
  {
    if ( item.length() > 0 )
    {
      elems.push_back( item );
    }
  }
  return elems;
}

std::vector<std::string> YaFsm::split( const std::string& s, char delim )
{
  std::vector<std::string> elems;
  split( s, delim, elems );
  return elems;
}

void YaFsm::printDbg( const std::string& str )
{
  std::cout << "info: " << str << std::endl;
}

void YaFsm::printWarn( const std::string& str )
{
  std::cerr << "warning: " << str << std::endl;
}

void YaFsm::printFatal( const std::string& str )
{
  std::cerr << "error: " << str << std::endl;
  exit( 1 );
}
