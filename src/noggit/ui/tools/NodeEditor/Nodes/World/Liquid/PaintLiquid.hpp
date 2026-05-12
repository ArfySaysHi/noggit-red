// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_PAINTLIQUID_HPP
#define NOGGIT_PAINTLIQUID_HPP

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
class PaintLiquidNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  PaintLiquidNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_liquid_type;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_PAINTLIQUID_HPP
