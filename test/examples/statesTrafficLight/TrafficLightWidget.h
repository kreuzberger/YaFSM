#ifndef TRAFFICLIGHTWIDGET_H
#define TRAFFICLIGHTWIDGET_H


#include <QtGui>

class LightWidget : public QWidget
{
public:
    LightWidget(const QColor &color, QWidget *parent = 0)
        : QWidget(parent), m_color(color), m_on(false) {}

    bool isOn() const;
    void setOn(bool on);

public slots:
    void turnOff();
    void turnOn();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QColor m_color;
    bool m_on;
};

class TrafficLightWidget : public QWidget
{
public:
    TrafficLightWidget(QWidget *parent = 0);


    LightWidget *redLight() const;
    LightWidget *yellowLight() const;
    LightWidget *greenLight() const;

private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
};


#endif // TRAFFICLIGHTWIDGET_H
