#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <QApplication>

#include "capture.h"
#include "specter_widget.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  Capture::instance();

  SpecterWidget sw;
  sw.show();

  return app.exec();
}
