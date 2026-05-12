// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_GETJSONVALUE_HPP
#define NOGGIT_GETJSONVALUE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class GetJSONValueNode : public BaseNode {
  Q_OBJECT

public:
  GetJSONValueNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_GETJSONVALUE_HPP
