#include "IPumpFSMActionHandler.h"
#include "PumpFSM.h"
#include <QApplication>

class Pump: public IPumpFSMActionHandler
{
public:
  Pump();
  virtual ~Pump();

// implementation from IFullFSMActionHandler
public:
//  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestFullFSM'" << std::endl;  }
  virtual void cmdPumpOpen( void ) {};
  virtual void cmdPumpClose( void ){};
  virtual void infoCtrlError( void ){};
  virtual void infoPumpOpened( void ){};
  virtual void infoPumpError( void ){};
  virtual void infoPumpCtrlOpened( void ){};
  virtual void infoPumpCtrlClosed( void ){};
  virtual void infoPumpCtrlError( void ){};

public:
  PumpFSM mPumpFSM;


private:
inline Pump& self() {return *this;}


};



Pump::Pump()
: IPumpFSMActionHandler()
, mPumpFSM(self())
{
  mPumpFSM.initFSM();
};
Pump::~Pump()
{
}


int main(int argc, char** argv)
{
  QApplication oApp(argc, argv);

  Pump pump;

  return oApp.exec();
}