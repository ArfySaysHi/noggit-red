// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_CHUNKGETVERTEXCOLORS_HPP
#define NOGGIT_CHUNKGETVERTEXCOLORS_HPP

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
class ChunkGetVertexColorsNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  ChunkGetVertexColorsNode();
  void compute() override;
  NodeValidationState validate() override;

private:
  std::vector<std::shared_ptr<NodeData>> _colors;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_CHUNKGETVERTEXCOLORS_HPP
