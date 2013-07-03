
#define TESTFSM

 
#include "IActionFSMActionHandler.h"
#include "ActionFSM.h"

// class QTestActionFSM: public QObject
// {
//      Q_OBJECT
//   public:   
//     QTestActionFSM() {}
//     virtual ~QTestActionFSM() {}
//  private slots:
//     void initTestCase() {}
//     void testTriggerFSM();
//     void cleanupTestCase() { }
// };

//class TestActionFSM: public IActionFSMActionHandler
//{
class QTestActionFSM: public QObject,public IActionFSMActionHandler
{
     Q_OBJECT  
public:
  QTestActionFSM()
  : mActionFSM(self())
  {
    mActionFSM.initFSM();
  };
 private slots:
    void initTestCase() {}
    void testTriggerFSM();
    void cleanupTestCase() { }
// implementation from IActionFSMActionHandler
public:
  virtual void onEnterRun( void )     {  std::cout << "onEnterRun called on 'TestActionFSM'" << std::endl;  }
  virtual void onExitRun( void )      {  std::cout << "onExitRun called on 'TestActionFSM'" << std::endl;  }
  virtual void onNotRun( bool bFlag ) {  
                                         std::cout << "onNotRun called on 'TestActionFSM' with flag " << bFlag << std::endl;  }
  virtual void onRun( void )          {  std::cout << "onRun called on 'TestActionFSM'" << std::endl;
                                         mActionFSM.setStartCounter(1);
                                      }

public:
  ActionFSM mActionFSM;

private:
inline QTestActionFSM& self() {return *this;}


};