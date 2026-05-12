// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <QMouseEvent>
#include <QtWidgets/QLabel>

namespace Noggit {
namespace Ui {
class ClickableLabel : public QLabel {
  Q_OBJECT

public:
  ClickableLabel(QWidget *parent = nullptr);

signals:
  void clicked();
  void leftClicked();
  void rightClicked();
  void middleClicked();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
};
} // namespace Ui
} // namespace Noggit
