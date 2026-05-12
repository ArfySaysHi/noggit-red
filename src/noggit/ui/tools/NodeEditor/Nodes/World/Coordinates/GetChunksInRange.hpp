// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_GETCHUNKSINRANGE_HPP
#define NOGGIT_GETCHUNKSINRANGE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class GetChunksInRangeNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  GetChunksInRangeNode();
  void compute() override;

private:
  std::vector<std::shared_ptr<NodeData>> _chunks;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_GETCHUNKSINRANGE_HPP
