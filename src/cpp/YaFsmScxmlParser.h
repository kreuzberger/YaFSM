#ifndef YAFSMSCXMLPARSER_H
#define YAFSMSCXMLPARSER_H
#include "tinyxml/tinyxml2.h"

#include <string>
#include <map>
#include <vector>

class YaFsmScxmlParser
{
  friend class YaFsmCppTest;
public:
  YaFsmScxmlParser();

  std::string fileName() const;
  void setFileName(const std::string &fileName);

  bool genCode() const;
  void setGenCode(bool genCode);

  std::string codeOutDir() const;
  void setCodeOutDir(const std::string &codeOutDir);

  void init();
  void readFSM();
  void parseDefinitions(const tinyxml2::XMLElement* );
  void parseFSM(const tinyxml2::XMLElement* );

private:

  bool hasSubStates( const tinyxml2::XMLElement* );

  std::string mFileName;
  bool        mGenCode = true;
  std::string mCodeOutDir;
  std::map< std::string, std::string >    mDataModel;
  std::map< std::string, const tinyxml2::XMLElement* > mMembers;
  std::map< std::string, const tinyxml2::XMLElement* > mStates;
//  bool        mGenView;
//  std::string mDotType;
//  std::string mViewOutDir;

};

#endif // YAFSMSCXMLPARSER_H
