#ifndef NOGGIT_WIGDET_MAP_BOOKMARK_LIST_ITEM_HPP
#define NOGGIT_WIGDET_MAP_BOOKMARK_LIST_ITEM_HPP

#include <QAction>
#include <QGridLayout>
#include <QMenuBar>
#include <QString>
#include <QWidget>
#include <noggit/project/ApplicationProject.h>
#include <qgraphicseffect.h>

namespace Noggit::Ui::Widget {
struct MapListBookmarkData {
  QString MapName;
  glm::vec3 Position;
};

class MapListBookmarkItem : public QWidget {
  Q_OBJECT
private:
  QLabel *map_icon;
  QLabel *map_name;
  QLabel *map_position;
  int _maxWidth;

public:
  MapListBookmarkItem(const MapListBookmarkData &data, QWidget *parent);
  QSize minimumSizeHint() const override;

private:
  QString toCamelCase(const QString &s);
};
} // namespace Noggit::Ui::Widget

#endif // NOGGIT_WIGDET_MAP_BOOKMARK_LIST_ITEM_HPP