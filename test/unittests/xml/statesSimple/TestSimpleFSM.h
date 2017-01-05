
#define TESTFSM

 
#include "ISimpleFSMActionHandler.h"
#include "SimpleFSM.h"


class QTestSimpleFSM: public QObject, public ISimpleFSMActionHandler
{
  Q_OBJECT
  public:
  QTestSimpleFSM()
  : mSimpleFSM(this)
  {
    mSimpleFSM.initFSM();
  }

  private slots:
    void initTestCase() {}
    void testInitFSM();
    void cleanupTestCase() { }
// implementation from ISimpleFSMActionHandler
public:
  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestSimpleFSM'" << std::endl;  }
  virtual void onExitRun( void )    {  std::cout << "onExitRun called on 'TestSimpleFSM'" << std::endl;  }
  //virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleFSM'" << std::endl;  }
  virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleFSM'" << std::endl; mSimpleFSM.end();  }

public:
  SimpleFSM mSimpleFSM;

private:
inline QTestSimpleFSM& self() {return *this;}


};
