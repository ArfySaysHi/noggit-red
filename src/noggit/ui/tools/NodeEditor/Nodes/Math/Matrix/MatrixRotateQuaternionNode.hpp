// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_MATRIXROTATEQUATERNIONNODE_HPP
#define NOGGIT_MATRIXROTATEQUATERNIONNODE_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class MatrixRotateQuaternionNode : public BaseNode {
  Q_OBJECT

public:
  MatrixRotateQuaternionNode();
  NodeValidationState validate() override;
  void compute() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_MATRIXROTATEQUATERNIONNODE_HPP
