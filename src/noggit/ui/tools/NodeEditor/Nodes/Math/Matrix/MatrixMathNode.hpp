// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_MATRIXMATHNODE_HPP
#define NOGGIT_MATRIXMATHNODE_HPP

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
class MatrixMathNode : public BaseNode {
  Q_OBJECT

public:
  MatrixMathNode();
  void compute() override;
  NodeValidationState validate() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_operation;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_MATRIXMATHNODE_HPP
