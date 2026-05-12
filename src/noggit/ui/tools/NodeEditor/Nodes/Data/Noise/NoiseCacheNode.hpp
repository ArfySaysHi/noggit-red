// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISECACHENODE_HPP
#define NOGGIT_NOISECACHENODE_HPP

#include <external/libnoise/src/noise/noise.h>
#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class NoiseCacheNode : public BaseNode {
  Q_OBJECT

public:
  NoiseCacheNode();
  NodeValidationState validate() override;
  void compute() override;

private:
  noise::module::Cache _module;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISECACHENODE_HPP
