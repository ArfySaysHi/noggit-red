// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_LOGICFORLOOPNODE_HPP
#define NOGGIT_LOGICFORLOOPNODE_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class LogicForLoopNode : public LogicNodeBase {
  Q_OBJECT

public:
  LogicForLoopNode();
  void compute() override;
  NodeValidationState validate() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_LOGICFORLOOPNODE_HPP
