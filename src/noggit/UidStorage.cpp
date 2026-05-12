// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#include <noggit/UidStorage.hpp>
#include <noggit/project/CurrentProject.hpp>

#include <QtCore/QSettings>

namespace {
QString uid_file_path() {
  QString str =
      QString(Noggit::Project::CurrentProject::get()->ProjectPath.c_str());
  if (!(str.endsWith('\\') || str.endsWith('/'))) {
    str += "/";
  }
  return str + "/uid.ini";
}
} // namespace

bool UidStorage::hasMaxUIDStored(uint32_t mapID) {
  QSettings uid_file(uid_file_path(), QSettings::Format::IniFormat);
  return uid_file.value(QString::number(mapID), -1).toUInt() != -1;
}

uint32_t UidStorage::getMaxUID(uint32_t mapID) {
  QSettings uid_file(uid_file_path(), QSettings::Format::IniFormat);
  return uid_file.value(QString::number(mapID), 0).toUInt();
}

void UidStorage::saveMaxUID(uint32_t mapID, uint32_t uid) {
  QSettings uid_file(uid_file_path(), QSettings::Format::IniFormat);
  uid_file.setValue(QString::number(mapID), uid);
}

void UidStorage::remove_uid_for_map(uint32_t map_id) {
  QSettings uid_file(uid_file_path(), QSettings::Format::IniFormat);
  uid_file.remove(QString::number(map_id));
}
