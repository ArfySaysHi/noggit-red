// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

#include <noggit/DBC.h>

#include <QMediaPlayer>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

#include <string>

namespace Noggit {
namespace Ui {
class ZoneIntroMusicPickerWindow : public QWidget {
  Q_OBJECT
public:
  ZoneIntroMusicPickerWindow(QPushButton *button, QWidget *parent = nullptr);

private:
  QLineEdit *_tree_searchbar;
  QListWidget *_picker_listview;

  int _entry_id = 0;
  int _priority = 1;
  QLabel *_entry_id_lbl;
  QLineEdit *_name_ledit;
  QSpinBox *_min_delay_spinbox;

  QPushButton *_sound_button;

  void select_entry(int id);
  void save_entry(int entry_id);
  // void duplicate_entry();
};
} // namespace Ui
} // namespace Noggit
