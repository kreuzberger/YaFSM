#define TESTFSM

#include "TimerScxmlFSM.h"
#include <QObject>


class TestTimerScxmlFSM
{
  
public:
  TestTimerScxmlFSM()
  : mTimerFSM()
  {
    mTimerFSM.initFSM();
  }

  virtual ~TestTimerScxmlFSM()
  {
  }

public:
  TimerScxmlFSM mTimerFSM;


private:
inline TestTimerScxmlFSM& self() {return *this;}


};

class QTestTimerScxmlFSM: public QObject
{
    Q_OBJECT
  public:   
    QTestTimerScxmlFSM() {}
    virtual ~QTestTimerScxmlFSM() {}
 private slots:
    void initTestCase()
    {
      mpApp = new TestTimerScxmlFSM();
    }
    void testInitFSM();
    void cleanupTestCase()
    {
      delete mpApp;
      mpApp = 0;
    }
  private:
    TestTimerScxmlFSM* mpApp;
};

