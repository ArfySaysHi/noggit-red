// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISEABSNODE_HPP
#define NOGGIT_NOISEABSNODE_HPP

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
class NoiseAbsNode : public BaseNode {
  Q_OBJECT

public:
  NoiseAbsNode();
  void compute() override;
  NodeValidationState validate() override;

private:
  noise::module::Abs _module;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISEABSNODE_HPP
