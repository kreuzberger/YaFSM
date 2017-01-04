#define TESTFSM
 
#include "ITimerFSMActionHandler.h"
#include "TimerFSM.h"
#include "FSMTimer.h"
#include <QApplication>
class TestTimerFSM: public QObject, public ITimerFSMActionHandler
{
  Q_OBJECT
public:
  TestTimerFSM()
  : ITimerFSMActionHandler()
  , mTimerFSM(this)
  {
    mTimerFSM.initFSM();
  }
  
  virtual ~TestTimerFSM()
  {
  }
  private slots:
    void initTestCase() {}
    void testInitFSM();
    void cleanupTestCase() { }
    
// implementation from ITimerFSMActionHandler
public:
  virtual void onTimerAutoEnd( void )   {  std::cout << "onTimerAutoEnd called on 'TestTimerFSM'" << std::endl; qApp->exit();  }
  virtual void onTimerAutoStart( void )    {  std::cout << "onTimerAutoStart called on 'TestTimerFSM'" << std::endl;  }
  virtual void onTimerTwice( void )    {  std::cout << "onTimerTwice called on 'TestTimerFSM'" << std::endl;  }

public:
  TimerFSM mTimerFSM;


private:
inline TestTimerFSM& self() {return *this;}


};

