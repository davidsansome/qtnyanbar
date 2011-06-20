#include <QApplication>

#include "demo.h"
#include "nyanproxystyle.h"

int main(int argc, char** argv) {
  QApplication a(argc, argv);

  a.setStyle(new NyanProxyStyle);

  Demo demo;
  demo.show();

  return a.exec();
}
