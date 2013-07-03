#include "TrafficLightWidget.h"

bool LightWidget::isOn() const
{
  return m_on;
}

void LightWidget::setOn(bool on)
{
    if (on == m_on)
        return;
    m_on = on;
    update();
}

void LightWidget::turnOff()
{
  setOn(false);
}

void LightWidget::turnOn()
{
  setOn(true);
}

void LightWidget::paintEvent(QPaintEvent *)
{
    if (!m_on)
        return;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(m_color);
    painter.drawEllipse(0, 0, width(), height());
}


LightWidget * TrafficLightWidget::redLight() const
{
  return m_red;
}

LightWidget * TrafficLightWidget::yellowLight() const
{
  return m_yellow;
}

LightWidget * TrafficLightWidget::greenLight() const
{
  return m_green;
}

TrafficLightWidget::TrafficLightWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    m_red = new LightWidget(Qt::red);
    vbox->addWidget(m_red);
    m_yellow = new LightWidget(Qt::yellow);
    vbox->addWidget(m_yellow);
    m_green = new LightWidget(Qt::green);
    vbox->addWidget(m_green);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);
}
