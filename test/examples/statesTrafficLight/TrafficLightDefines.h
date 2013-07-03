#ifndef TRAFFICLIGHTDEFINES_H
#define TRAFFICLIGHTDEFINES_H

#include <Qt>
#include <QColor>

namespace NTrafficLight
{
  
  typedef QColor Color ;

  const Color None = Qt::white;
  const Color RedYellow = Qt::red | Qt::yellow;
  const Color Red = Qt::red;
  const Color Yellow = Qt::yellow;
  const Color Green = Qt::green;

}

#endif

