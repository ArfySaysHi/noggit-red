#ifndef NOGGIT_LOGICBRANCH_HPP
#define NOGGIT_LOGICBRANCH_HPP

#include <external/NodeEditor/include/nodes/FlowScene>
#include <external/NodeEditor/include/nodes/Node>
#include <stack>

using QtNodes::Connection;
using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class LogicBranch {
public:
  explicit LogicBranch(Node *logic_node);
  bool executeNode(Node *node, Node *source_node);
  static bool executeNodeLeaves(Node *node, Node *source_node);
  void markNodesComputed(Node *start_node, bool state);
  void markNodeLeavesComputed(Node *start_node, Node *source_node, bool state);
  void setCurrentLoop(Node *node) { _loop_stack.push(node); };
  void unsetCurrentLoop() { _loop_stack.pop(); };
  Node *getCurrentLoop() {
    return _loop_stack.empty() ? nullptr : _loop_stack.top();
  };
  bool execute();

private:
  Node *_logic_node;
  std::stack<Node *> _loop_stack;
  bool _return = false;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_LOGICBRANCH_HPP
