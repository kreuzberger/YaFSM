#include "YaFsmScxmlParser.h"
#include <string>

void printHelp()
{
  fprintf(stderr, "yafsmgen <options>\n");
  fprintf(stderr, "\nOptions:\n");
  fprintf(stderr, "  --fsm <scxml>    scml file name\n");
  fprintf(stderr, "  --outcode <dir>  generated code directory\n");
  fprintf(stderr, "  --verbose        output parser info\n");
}

bool parseArgs( YaFsmScxmlParser& parser, int& argc, char** argv)
{
  bool ret = true;

  for( int idx = 1; idx < argc; idx++)
  {
    if(std::string(argv[idx]) == std::string("--fsm"))
    {
      std::string fsm = argv[idx+1];
      parser.setFileName(fsm);
      idx++;
    }
    else if(std::string(argv[idx]) == std::string("--outcode"))
    {
      std::string codePath = argv[idx+1];
      parser.setCodeOutDir(codePath);
    }
    else if(std::string(argv[idx]) == std::string("--verbose"))
    {
      parser.setVerbose(true);
    }
    else if(std::string(argv[idx]) == std::string("--help"))
    {
      printHelp();
      exit(0);
    }
  }

  return ret;


}
//       'fsm=s' => \$gFSMFileName,
//       'genview' => \$gFSMGenView,
//       'dottype=s' => \$gFSMGenDotType,
//       'outview=s' => \$gFSMViewOutPath,
//       'gencode' => \$gFSMGenCode,
//       'outcode=s' => \$gFSMCodeOutPath,
//       'verbose' => \$YaFsm::gVerbose,
//       'help' => sub{pod2usage(-verbose => 0);CORE::exit;},
//       'man' => sub{pod2usage(-verbose => 1);CORE::exit;}

int main(int argc, char** argv)
{  
  bool bOk = false;
  YaFsmScxmlParser parser;

  bOk = parseArgs(parser,argc, argv);
  parser.readFSM();

  return bOk;
}


