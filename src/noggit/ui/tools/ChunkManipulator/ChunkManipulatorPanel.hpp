// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_CHUNKMANIPULATORPANEL_HPP
#define NOGGIT_CHUNKMANIPULATORPANEL_HPP

#include <QWidget>

class MapView;

namespace Noggit::Ui::Tools::ChunkManipulator {
class ChunkManipulatorPanel : public QWidget {
public:
  ChunkManipulatorPanel(MapView *map_view, QWidget *parent = nullptr);

private:
  MapView *_map_view;
};

} // namespace Noggit::Ui::Tools::ChunkManipulator

#endif // NOGGIT_CHUNKMANIPULATORPANEL_HPP
