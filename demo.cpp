#include "demo.h"
#include "ui_demo.h"

#include <QProgressDialog>
#include <QPushButton>
#include <QTimer>

Demo::Demo(QWidget* parent)
  : QWidget(parent),
    ui_(new Ui_Demo),
    timer_(new QTimer(this)) {
  ui_->setupUi(this);

  connect(timer_, SIGNAL(timeout()), SLOT(TimerFired()));
  connect(ui_->timer_speed, SIGNAL(valueChanged(int)), SLOT(SetTimerSpeed(int)));

  connect(ui_->manual_dial, SIGNAL(valueChanged(int)), SLOT(SetManualValue(int)));
  connect(ui_->manual_scroll, SIGNAL(valueChanged(int)), SLOT(SetManualValue(int)));
  connect(ui_->manual_slider, SIGNAL(valueChanged(int)), SLOT(SetManualValue(int)));

  connect(ui_->dialog_open, SIGNAL(clicked()), SLOT(OpenDialog()));

  SetTimerSpeed(ui_->timer_speed->value());
  timer_->start();
}

Demo::~Demo() {
  delete ui_;
}

void Demo::SetTimerSpeed(int speed) {
  timer_->setInterval(1000 / speed);
}

void Demo::TimerFired() {
  ui_->timer_bar->setValue((ui_->timer_bar->value() + 2) % ui_->timer_bar->maximum());
}

void Demo::SetManualValue(int value) {
  ui_->manual_bar->setValue(value);
  ui_->manual_dial->setValue(value);
  ui_->manual_scroll->setValue(value);
  ui_->manual_slider->setValue(value);
}

void Demo::OpenDialog() {
  QProgressDialog* dialog = new QProgressDialog(this);
  dialog->setLabelText("Nyan nyan nyan nyan");
  dialog->setWindowTitle("Nyan nyan nyan nyan");
  dialog->setCancelButtonText("Nooooooooo!");
  dialog->setMinimumWidth(350);

  QTimer* timer = new QTimer(dialog);
  timer->setInterval(100);
  connect(timer, SIGNAL(timeout()), SLOT(DialogTimerFired()));
  connect(dialog, SIGNAL(canceled()), dialog, SLOT(deleteLater()));

  dialog->show();
  timer->start();
}

void Demo::DialogTimerFired() {
  QProgressDialog* dialog = qobject_cast<QProgressDialog*>(sender()->parent());
  dialog->setValue((dialog->value() + 4) % dialog->maximum());
}


