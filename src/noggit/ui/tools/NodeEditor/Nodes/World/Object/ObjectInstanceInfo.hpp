// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_OBJECTINSTANCEINFO_HPP
#define NOGGIT_OBJECTINSTANCEINFO_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ObjectInstanceInfoNode : public ContextNodeBase {
  Q_OBJECT

public:
  ObjectInstanceInfoNode();
  void compute() override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_OBJECTINSTANCEINFO_HPP
