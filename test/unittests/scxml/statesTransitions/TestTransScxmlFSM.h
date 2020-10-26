
#define TESTFSM

#include "TransScxmlFSM.h"

class QTestTransScxmlFSM : public QObject
{
  Q_OBJECT
public:
  QTestTransScxmlFSM()
    : mTransScxmlFSM()
  {
    mTransScxmlFSM.initFSM();
  }

private Q_SLOTS:
  void initTestCase() {}
  void testInitFSM();
  void cleanupTestCase() {}

public:
  TransScxmlFSM mTransScxmlFSM;

private:
  inline QTestTransScxmlFSM& self() { return *this; }
};
