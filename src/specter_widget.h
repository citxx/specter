#pragma once

#include <vector>

#include <QGraphicsScene>
#include <QTimer>
#include <QWidget>

namespace Ui {
  class SpecterWidget;
}

class SpecterWidget : public QWidget {
  Q_OBJECT

 public:
  explicit SpecterWidget(QWidget *parent = nullptr);
  ~SpecterWidget();

 private slots:
  void update();
  void draw();

 private:
  Ui::SpecterWidget *_ui;
  QTimer *_timer;
  QGraphicsScene *_scene;

  std::vector<float> _buffer;
};
