// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_JSONOBJECTINFO_HPP
#define NOGGIT_JSONOBJECTINFO_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class JSONObjectInfoNode : public BaseNode {
  Q_OBJECT

public:
  JSONObjectInfoNode();
  void compute() override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_JSONOBJECTINFO_HPP
