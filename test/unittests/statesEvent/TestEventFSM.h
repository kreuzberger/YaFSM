#define TESTFSM

#include "EventFSM.h"
#include <QObject>


class TestEventFSM: public IEventFSMActionHandler
{
  
public:
  TestEventFSM()
  : mEventFSM(self())
  {
    mEventFSM.initFSM();
  }

  virtual ~TestEventFSM()
  {
  }

// implementation from ITimerFSMActionHandler
public:
  virtual void onEventAutoEnd( void )   {  std::cout << "onEventAutoEnd called on 'TestEventFSM'" << std::endl; }
  virtual void onEventAutoStart( void ) {
                                           std::cout << "onEventAutoStart called on 'TestEventFSM'" << std::endl;
                                           mEventFSM.sendEventID( EventFSM::EVENT_AUTOEND );
                                        }
  virtual void onSendAutoStart( void )  {
                                           std::cout << "onSendAutoStart called on 'TestEventFSM'" << std::endl;
                                           mEventFSM.sendEventID( EventFSM::EVENT_AUTOSTART );
                                        }
  virtual void onEnterRunning( void )   {  std::cout << "onEnterRunning called on 'TestEventFSM'" << std::endl; }

public:
  EventFSM mEventFSM;


private:
inline TestEventFSM& self() {return *this;}


};

class QTestEventFSM: public QObject
{
    Q_OBJECT
  public:   
    QTestEventFSM() {}
    virtual ~QTestEventFSM() {}
 private slots:
    void initTestCase()
    {
      mpApp = new TestEventFSM();
    }
    void testInitFSM();
    void cleanupTestCase()
    {
      delete mpApp;
      mpApp = 0;
    }
  private:
    TestEventFSM* mpApp;
};

