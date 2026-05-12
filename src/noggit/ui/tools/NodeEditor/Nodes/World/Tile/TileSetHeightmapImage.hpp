// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_TILESETHEIGHTMAPIMAGE_HPP
#define NOGGIT_TILESETHEIGHTMAPIMAGE_HPP

#include <QComboBox>
#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class TileSetHeightmapImageNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  TileSetHeightmapImageNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;
  NodeValidationState validate() override;

private:
  QComboBox *_operation;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_TILESETHEIGHTMAPIMAGE_HPP
