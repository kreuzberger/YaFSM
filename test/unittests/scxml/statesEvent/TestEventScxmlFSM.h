#define TESTSCXMLFSM

#include "EventScxmlFSM.h"
#include <QObject>


class TestEventScxmlFSM
{
  
public:
  TestEventScxmlFSM()
  : mEventFSM()
  {
    mEventFSM.initFSM();
  }

  virtual ~TestEventScxmlFSM()
  {
  }

public:
  EventScxmlFSM mEventFSM;


private:
inline TestEventScxmlFSM& self() {return *this;}


};

class QTestEventScxmlFSM: public QObject
{
    Q_OBJECT
  public:   
    QTestEventScxmlFSM() {}
    virtual ~QTestEventScxmlFSM() {}
 private slots:
    void initTestCase()
    {
      mpApp = new TestEventScxmlFSM();
    }
    void testInitFSM();
    void cleanupTestCase()
    {
      delete mpApp;
      mpApp = 0;
    }
  private:
    TestEventScxmlFSM* mpApp;
};

