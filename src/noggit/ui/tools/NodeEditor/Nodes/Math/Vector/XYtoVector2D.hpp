// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_XYTOVECTOR2D_HPP
#define NOGGIT_XYTOVECTOR2D_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class XYtoVector2DNode : public BaseNode {
  Q_OBJECT

public:
  XYtoVector2DNode();
  void compute() override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_XYTOVECTOR2D_HPP
