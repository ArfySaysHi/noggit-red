#ifndef NOGGIT_SRC_NOGGIT_RED_NODEEDITOR_NODES_WORLD_CHUNK_CHUNKADDDETAILDOODADS_HPP
#define NOGGIT_SRC_NOGGIT_RED_NODEEDITOR_NODES_WORLD_CHUNK_CHUNKADDDETAILDOODADS_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit::Ui::Tools::NodeEditor::Nodes {
class ChunkAddDetailDoodads : public ContextLogicNodeBase {
  Q_OBJECT
public:
  ChunkAddDetailDoodads();
  void compute() override;
  NodeValidationState validate() override;
};
} // namespace Noggit::Ui::Tools::NodeEditor::Nodes

#endif // NOGGIT_SRC_NOGGIT_RED_NODEEDITOR_NODES_WORLD_CHUNK_CHUNKADDDETAILDOODADS_HPP
