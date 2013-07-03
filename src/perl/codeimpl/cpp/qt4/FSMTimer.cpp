#include "FSMTimer.h"
#include <QtCore/QTimer>
#include <QtCore/QTimerEvent>
#include <QtCore/QObject>


FSMTimer::FSMTimer( IFSMTimerCB& handler)
: QObject()
, IFSMTimer()
, mCbHandler( handler )
, mTimerMap()
, mActiveTimerMap()
{

}

FSMTimer::~FSMTimer()
{


}

void FSMTimer::setTimerID( int iTimerID, int iTimeOutMs, int iRepeat )
{
  if(!mTimerMap.contains(iTimerID))
  {
    FSMTimerInfo timerInfo(iTimeOutMs,iRepeat);
    mTimerMap.insert(iTimerID, timerInfo);
  }
  
}


void FSMTimer::startTimerID( int iTimerID )
{
  if(mTimerMap.contains(iTimerID))
  {
    int iActiveTimerID = startTimer(mTimerMap[iTimerID].miTimeOutMs);
    mActiveTimerMap.insert(iTimerID,iActiveTimerID);
    mTimerMap[iTimerID].miRepeatCnt = 0;
  }
}

void FSMTimer::stopTimerID( int iTimerID )
{
  if(mActiveTimerMap.contains(iTimerID))
  {
    killTimer(mActiveTimerMap[iTimerID]);
    mActiveTimerMap.remove(iTimerID);

  }
} 

void FSMTimer::timerEvent(QTimerEvent* pEvent)
{
  if( 0 != pEvent)
  {
    int iTimerID = mActiveTimerMap.key(pEvent->timerId());
    if (0 != iTimerID )
    {
      if( mTimerMap[iTimerID].miRepeatCnt == ( mTimerMap[iTimerID].miRepeat - 1 ) )
      {
        stopTimerID(iTimerID);
      }
      else
      {
        mTimerMap[iTimerID].miRepeatCnt++;
      }
  
      mCbHandler.processTimerEventID(iTimerID);
      
    }
  
  }
}

