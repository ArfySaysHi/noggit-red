// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_TILEGETOBJECTSUIDS_HPP
#define NOGGIT_TILEGETOBJECTSUIDS_HPP

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
class TileGetObjectUIDsNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  TileGetObjectUIDsNode();
  void compute() override;
  NodeValidationState validate() override;

private:
  std::vector<std::shared_ptr<NodeData>> _uids;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_TILEGETOBJECTSUIDS_HPP
