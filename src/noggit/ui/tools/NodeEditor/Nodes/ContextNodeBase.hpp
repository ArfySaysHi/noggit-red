// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_CONTEXTNODEBASE_HPP
#define NOGGIT_CONTEXTNODEBASE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>
#include <noggit/ui/tools/NodeEditor/Nodes/Scene/Context.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ContextNodeBase : public BaseNode {
  Q_OBJECT

public:
  ContextNodeBase() {};

  NodeValidationState validate() override {
    if (!gCurrentContext->getWorld() || !gCurrentContext->getViewport()) {
      setValidationState(NodeValidationState::Error);
      setValidationMessage("Error: this node requires a world context to run.");
    }

    return _validation_state;
  };
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_CONTEXTNODEBASE_HPP
