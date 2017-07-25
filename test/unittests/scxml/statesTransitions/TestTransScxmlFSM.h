
#define TESTFSM

 
#include "TransScxmlFSM.h"


class QTestTransScxmlFSM: public QObject
{
  Q_OBJECT
  public:
  QTestTransScxmlFSM()
  : mTransScxmlFSM(nullptr)
  {
  }

  private slots:
    void initTestCase() {}
    void testInitFSM();
    void testSecondCondition();
    void cleanupTestCase() { }

public:
  TransScxmlFSM* mTransScxmlFSM = nullptr;

private:
inline QTestTransScxmlFSM& self() {return *this;}


};
