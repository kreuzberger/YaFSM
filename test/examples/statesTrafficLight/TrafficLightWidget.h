#ifndef TRAFFICLIGHTWIDGET_H
#define TRAFFICLIGHTWIDGET_H


#include <QtGui>

class TrafficLightFSM;
class LightWidget : public QWidget
{
  Q_OBJECT
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
  Q_OBJECT
public:
    TrafficLightWidget(QWidget *parent = 0);


    LightWidget *redLight() const;
    LightWidget *yellowLight() const;
    LightWidget *greenLight() const;
    void setFSM(TrafficLightFSM*);

public slots:
    void onErrorToggled(bool);
private:
    LightWidget *m_red;
    LightWidget *m_yellow;
    LightWidget *m_green;
    QPushButton* m_ErrorBtn;
    TrafficLightFSM* mpoFSM;
    TrafficLightWidget& self();
};

inline TrafficLightWidget& TrafficLightWidget::self()
{
  return *this;
}

#endif // TRAFFICLIGHTWIDGET_H
