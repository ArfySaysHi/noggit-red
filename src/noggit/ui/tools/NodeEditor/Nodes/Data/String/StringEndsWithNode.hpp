// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_STRINGENDSWITHNODE_HPP
#define NOGGIT_STRINGENDSWITHNODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class StringEndsWithNode : public BaseNode {
  Q_OBJECT

public:
  StringEndsWithNode();
  void compute() override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_STRINGENDSWITHNODE_HPP
