#pragma once

#include <iostream>
#include <vector>

class IScxmlFSMEvent
{
  public:
  IScxmlFSMEvent();
  virtual ~IScxmlFSMEvent();

  public:
  virtual void setEventID( int eventID) = 0;
  virtual int sendEventID( const std::string& sendId, int eventID, int delayMs ) = 0;
  virtual std::vector<int> cancelEvent( const std::string& sendID ) = 0;

};



