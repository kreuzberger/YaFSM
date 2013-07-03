#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "ITrafficLightFSMActionHandler.h"
#include "TrafficLightFSM.h"
#include "TrafficLightDefines.h"
#include "TrafficLightWidget.h"

#define TESTFSM

class TrafficLight: public ITrafficLightFSMActionHandler
{
public:
  TrafficLight();
  virtual ~TrafficLight();

// implementation from IFullFSMActionHandler
public:
//  virtual void onEnterRun( void )   {  std::cout << "onEnterRun called on 'TestFullFSM'" << std::endl;  }
  virtual void onRun( void );
  virtual void onError( void );
  virtual void showColor( const NTrafficLight::Color& );

private:
  TrafficLightFSM mTrafficLightFSM;
  TrafficLightWidget mTrafficLightWidget;



private:
inline TrafficLight& self() {return *this;}


};

#endif

