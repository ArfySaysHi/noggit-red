// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_REWIRINGPOINTNODE_HPP
#define NOGGIT_REWIRINGPOINTNODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class RewiringPointNode : public BaseNode {
  Q_OBJECT

public:
  RewiringPointNode();
  void compute() override;
  QJsonObject save() const override;
  NodeValidationState validate() override;
  void restore(QJsonObject const &json_obj) override;

  bool captionVisible() const override { return false; };

public Q_SLOTS:

  void inputConnectionCreated(Connection const &) override;

  void inputConnectionDeleted(Connection const &) override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_REWIRINGPOINTNODE_HPP
