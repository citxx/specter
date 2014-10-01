#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>

#include "capture.h"
#include "specter_widget.h"
#include "ui_specter_widget.h"

SpecterWidget::SpecterWidget(QWidget *parent):
    QWidget(parent),
    _ui(new Ui::SpecterWidget) {
  _ui->setupUi(this);
  _timer = new QTimer(this);
  _scene = new QGraphicsScene;
  _ui->spectrumView->setScene(_scene);
  
  connect(_timer, SIGNAL(timeout()), this, SLOT(update()));
  _timer->start(100);
}

SpecterWidget::~SpecterWidget() {
  delete _ui;
  delete _timer;
}

void SpecterWidget::update() {
  _buffer = Capture::instance()->buffer();
  draw();
}

void SpecterWidget::draw() {
  int width = _ui->spectrumView->width();
  int height = _ui->spectrumView->height();
  int middle = height / 2;
  QImage canvas(width, height, QImage::Format_RGB32);
  canvas.fill(qRgb(0, 0, 0));
  QRgb color = qRgb(30, 220, 30);

  for (int i = 0; i < width; ++i) {
    float mx = 0;
    int w_start = _buffer.size() * i / width;
    int w_end = _buffer.size() * (i + 1) / width;
    for (int j = w_start; j <= w_end; ++j) {
      mx = std::max(mx, _buffer[j]);
    }
    mx = std::min(mx, 1.0f);
    int delta = mx * height / 2;
    for (int j = middle - delta; j <= middle + delta; ++j) {
      canvas.setPixel(i, j, color);
    }
  }

  _scene->clear();
  QGraphicsItem *item = _scene->addPixmap(QPixmap::fromImage(canvas));
  item->setPos(0, 0);
  _ui->spectrumView->setSceneRect(0, 0, width, height);
}
