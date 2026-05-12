// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_ISOBJECTINSTANCESELECTEDUID_HPP
#define NOGGIT_ISOBJECTINSTANCESELECTEDUID_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class IsObjectInstanceSelectedUIDNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  IsObjectInstanceSelectedUIDNode();
  void compute() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_ISOBJECTINSTANCESELECTEDUID_HPP
