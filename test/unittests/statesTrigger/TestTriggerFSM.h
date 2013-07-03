
#define TESTFSM

 
#include "ITriggerFSMActionHandler.h"
#include "TriggerFSM.h"


class TestTriggerFSM: public QObject, public ITriggerFSMActionHandler
{
  Q_OBJECT
public:
  TestTriggerFSM()
  : mTriggerFSM(self())
  {
    mTriggerFSM.initFSM();
  };
private slots:
  void initTestCase() {}
  void testInitFSM();
  void cleanupTestCase() { }

// implementation from ITriggerFSMActionHandler
public:
  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestTriggerFSM'" << std::endl;  }
  virtual void onExitRun( void )    {  std::cout << "onExitRun called on 'TestTriggerFSM'" << std::endl;  }

public:
  TriggerFSM mTriggerFSM;

private:
inline TestTriggerFSM& self() {return *this;}


};