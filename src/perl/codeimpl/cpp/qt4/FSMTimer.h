#ifndef FSMTIMER_H
#define FSMTIMER_H

#include "IFSMTimer.h"
#include "IFSMTimerCB.h"
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>

class QTimerEvent;

class FSMTimerInfo
{
  public:
    FSMTimerInfo()
    : miTimeOutMs(-1)
    , miRepeat( 0 )
    , miRepeatCnt( 0 )
    {
    }

    FSMTimerInfo( int iTimeOutMs, int iRepeat)
    : miTimeOutMs(iTimeOutMs)
    , miRepeat(iRepeat)
    , miRepeatCnt( 0 )
    {
    }

    virtual ~FSMTimerInfo() {}

  public:
    int miTimeOutMs;
    int miRepeat;
    int miRepeatCnt;
};


class FSMTimer: public QObject, public IFSMTimer
{
  Q_OBJECT
public:
  FSMTimer( IFSMTimerCB& );
  virtual ~FSMTimer();
  virtual void setTimerID( int iTimerId, int iTimeOutMs, int iRepeat );
  virtual void startTimerID( int iTimerId );
  virtual void stopTimerID( int iTimerId );
  virtual void timerEvent( QTimerEvent* );

private:
  FSMTimer();
  FSMTimer& operator=(const FSMTimer&);
  FSMTimer(const FSMTimer&);

  IFSMTimerCB& mCbHandler;
  QMap<int,FSMTimerInfo> mTimerMap;
  QMap<int,int> mActiveTimerMap;


};


#endif
 
