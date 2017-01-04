#define TESTFSM
 
#include "IFullFSMActionHandler.h"
#include "FullFSM.h"
#include "FSMTimer.h"

class QTestFullFSM: public QObject, public IFullFSMActionHandler
{
  Q_OBJECT
  public:
  QTestFullFSM()
  : IFullFSMActionHandler()
  , mFullFSM(this)
  {
    mFullFSM.initFSM();
  }
  
  virtual ~QTestFullFSM()
  {
  }
private slots:
    void initTestCase() {}
    void testInitFSM();
    void cleanupTestCase() { }
// implementation from IFullFSMActionHandler
public:
  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestFullFSM'" << std::endl;  }
  virtual void onExitRun( void )    {  std::cout << "onExitRun called on 'TestFullFSM'" << std::endl;  }
  virtual void onRun( void )        {  std::cout << "onRun called on 'TestFullFSM'" << std::endl;  }

public:
  FullFSM mFullFSM;


private:
inline QTestFullFSM& self() {return *this;}


};
