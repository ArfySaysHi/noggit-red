#ifndef NOGGIT_CONDITIONNODE_HPP
#define NOGGIT_CONDITIONNODE_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp"

#include <QComboBox>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ConditionNode : public BaseNode {
  Q_OBJECT

public:
  ConditionNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QDoubleSpinBox *_first;
  QDoubleSpinBox *_second;
  QComboBox *_operation;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_CONDITIONNODE_HPP
