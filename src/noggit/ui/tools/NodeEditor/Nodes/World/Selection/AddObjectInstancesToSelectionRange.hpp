// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_ADDOBJECTINSTANCESTOSELECTIONRANGE_HPP
#define NOGGIT_ADDOBJECTINSTANCESTOSELECTIONRANGE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextLogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class AddObjectInstancesToSelectionRangeNode : public ContextLogicNodeBase {
  Q_OBJECT

public:
  AddObjectInstancesToSelectionRangeNode();
  void compute() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_ADDOBJECTINSTANCESTOSELECTIONRANGE_HPP
