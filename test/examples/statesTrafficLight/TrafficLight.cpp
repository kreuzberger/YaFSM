
#include "TrafficLight.h"

TrafficLight::TrafficLight()
: ITrafficLightFSMActionHandler()
, mTrafficLightFSM(self())
{

  mTrafficLightWidget.resize(110, 300);
  mTrafficLightWidget.show();
  mTrafficLightFSM.initFSM();
  mTrafficLightFSM.run();
  mTrafficLightWidget.setFSM(&mTrafficLightFSM);

};
TrafficLight::~TrafficLight()
{
}

void
TrafficLight::onRun( void )
{
  std::cout << "onRun called" << std::endl;
}

void
TrafficLight::onError( void )
{
  std::cout << "onError called" << std::endl;
}

void
TrafficLight::showColor( const NTrafficLight::Color& color )
{
  if( NTrafficLight::Red == color )
  {
    std::cout << "TrafficLight switch to RED" << std::endl;
    mTrafficLightWidget.redLight()->turnOn();
    mTrafficLightWidget.yellowLight()->turnOff();
    mTrafficLightWidget.greenLight()->turnOff();
  }
  else if( NTrafficLight::RedYellow == color )
  {
    std::cout << "TrafficLight switch to REDYELLOW" << std::endl;
    mTrafficLightWidget.redLight()->turnOn();
    mTrafficLightWidget.yellowLight()->turnOn();
    mTrafficLightWidget.greenLight()->turnOff();
  }
  else if( NTrafficLight::Yellow == color )
  {
    std::cout << "TrafficLight switch to YELLOW" << std::endl;
    mTrafficLightWidget.redLight()->turnOff();
    mTrafficLightWidget.yellowLight()->turnOn();
    mTrafficLightWidget.greenLight()->turnOff();
  }
  else if( NTrafficLight::Green == color )
  {
    std::cout << "TrafficLight switch to GREEN" << std::endl;
    mTrafficLightWidget.redLight()->turnOff();
    mTrafficLightWidget.yellowLight()->turnOff();
    mTrafficLightWidget.greenLight()->turnOn();
  }
  else if( NTrafficLight::None == color )
  {
    std::cout << "TrafficLight switch off" << std::endl;
    mTrafficLightWidget.redLight()->turnOff();
    mTrafficLightWidget.yellowLight()->turnOff();
    mTrafficLightWidget.greenLight()->turnOff();
  }

}
