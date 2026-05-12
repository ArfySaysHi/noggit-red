// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_SELECTIONINFO_HPP
#define NOGGIT_SELECTIONINFO_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/ContextNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class SelectionInfoNode : public ContextNodeBase {
  Q_OBJECT

public:
  SelectionInfoNode();
  void compute() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_SELECTIONINFO_HPP
