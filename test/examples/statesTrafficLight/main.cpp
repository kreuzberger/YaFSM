#include "TrafficLight.h"
#include <QApplication>

int main(int argc, char** argv)
{
  QApplication oApp(argc, argv);

  TrafficLight trafficLight;

  return oApp.exec();
}