// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_GETSELECTEDOBJECTINSTANCES_HPP
#define NOGGIT_GETSELECTEDOBJECTINSTANCES_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>
#include <vector>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class GetSelectedObjectInstancesNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  GetSelectedObjectInstancesNode();
  void compute() override;

private:
  std::vector<std::shared_ptr<NodeData>> _objects;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_GETSELECTEDOBJECTINSTANCES_HPP
