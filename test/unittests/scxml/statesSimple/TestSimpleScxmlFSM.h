
#define TESTFSM

 
#include "ISimpleScxmlFSMActionHandler.h"
#include "SimpleScxmlFSM.h"


class QTestSimpleScxmlFSM: public QObject, public ISimpleScxmlFSMActionHandler
{
  Q_OBJECT
  public:
  QTestSimpleScxmlFSM()
  : mSimpleScxmlFSM(this)
  {
    mSimpleScxmlFSM.initFSM();
  }

  private slots:
    void initTestCase() {}
    void testInitFSM();
    void cleanupTestCase() { }
// implementation from ISimpleFSMActionHandler
public:
  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestSimpleScxmlFSM'" << std::endl;  }
  virtual void onExitRun( void )    {  std::cout << "onExitRun called on 'TestSimpleScxmlFSM'" << std::endl;  }
  //virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleFSM'" << std::endl;  }
  virtual void onRun( void )        {  std::cout << "onRun called on 'TestSimpleScxmlFSM'" << std::endl; mSimpleScxmlFSM.end();  }

public:
  SimpleScxmlFSM mSimpleScxmlFSM;

private:
inline QTestSimpleScxmlFSM& self() {return *this;}


};
