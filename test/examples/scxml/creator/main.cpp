#include "ICreatorStateFSMActionHandler.h"
#include "CreatorStateFSM.h"
#include <QApplication>

class Simple: public ICreatorStateFSMActionHandler
{
public:
  Simple();
  virtual ~Simple();

// implementation from ICreatorStateFSMActionHandler
public:

public:
  CreatorStateFSM mCreatorStateFSM;


private:
inline Simple& self() {return *this;}


};



Simple::Simple()
: ICreatorStateFSMActionHandler()
, mCreatorStateFSM(this)
{
  mCreatorStateFSM.initFSM();
}

Simple::~Simple()
{
}


int main(int argc, char** argv)
{
  QApplication oApp(argc, argv);

  Simple Simple;

  return oApp.exec();
}
