// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_EXTENDEDSLIDER_HPP
#define NOGGIT_EXTENDEDSLIDER_HPP

#include "ui_ExtendedSliderUi.h"
#include <QWidget>
#include <noggit/TabletManager.hpp>

namespace Noggit {
namespace Ui::Tools::UiCommon {
class ExtendedSlider : public QWidget {
  Q_OBJECT
public:
  ExtendedSlider(QWidget *parent = nullptr);

  void setPrefix(const QString &prefix);
  void setMinimum(double min);
  void setMaximum(double max);
  void setRange(double min, double max);
  void setDecimals(int decimals);
  void setSingleStep(double val);
  void setSliderRange(int min, int max);
  void setValue(double value);
  double value();
  double rawValue();

  void setTabletSupportEnabled(bool state);

signals:
  void valueChanged(double value);

private:
  ::Ui::ExtendedSliderUi _ui;
  bool _is_tablet_supported = true;
  bool _is_tablet_affecting = false;
  QWidget *_tablet_popup;
  unsigned _tablet_sens_factor = 300;
  TabletManager *_tablet_manager;
};
} // namespace Ui::Tools::UiCommon
} // namespace Noggit

#endif // NOGGIT_EXTENDEDSLIDER_HPP
