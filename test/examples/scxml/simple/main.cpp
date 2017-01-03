#include "ISimpleFSMActionHandler.h"
#include "SimpleFSM.h"
#include <QApplication>

class Simple: public ISimpleFSMActionHandler
{
public:
  Simple();
  virtual ~Simple();

// implementation from ISimpleFSMActionHandler
public:

public:
  SimpleFSM mSimpleFSM;


private:
inline Simple& self() {return *this;}


};



Simple::Simple()
: ISimpleFSMActionHandler()
, mSimpleFSM(this)
{
  mSimpleFSM.initFSM();
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
