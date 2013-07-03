
#include "FSMEvent.h"
#include "QtCore/QCoreApplication"

FSMEvent::FSMEvent(IFSMEventCB& oCBHandler)
: QObject()
, IFSMEvent()
, mCBHandler(oCBHandler)
, mEventMap()
{

}

FSMEvent::~FSMEvent()
{

}

void FSMEvent::registerEventID( int iEventID )
{
  if(!mEventMap.contains(iEventID))
  {
    QEvent::Type event = static_cast<QEvent::Type>(QEvent::registerEventType());
    mEventMap.insert(iEventID, event);
  }
}


void FSMEvent::sendEventID( int iEventID )
{
  if(mEventMap.contains(iEventID))
  {
    QEvent* pEvent = new QEvent(mEventMap[iEventID]);
    if( 0 != pEvent )
    {
      QCoreApplication::postEvent(this, pEvent);
    }
  }
}


bool FSMEvent::event( QEvent* e )
{
  bool bRet = false;
  if( 0 != e)
  {
    int iEventID = mEventMap.key(e->type());
    mCBHandler.processEventID( iEventID );
    bRet = true;
  }

  return bRet;
}
