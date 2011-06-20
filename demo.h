#ifndef DEMO_H
#define DEMO_H

#include <QWidget>

class Ui_Demo;

class Demo : public QWidget {
  Q_OBJECT

public:
  Demo(QWidget* parent = NULL);
  ~Demo();

private slots:
  void SetTimerSpeed(int speed);
  void TimerFired();

  void SetManualValue(int value);

  void OpenDialog();
  void DialogTimerFired();

private:
  Ui_Demo* ui_;

  QTimer* timer_;
};

#endif // DEMO_H
