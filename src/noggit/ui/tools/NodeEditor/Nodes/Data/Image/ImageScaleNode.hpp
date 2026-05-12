// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_IMAGESCALENODE_HPP
#define NOGGIT_IMAGESCALENODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp>

#include <QComboBox>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ImageScaleNode : public LogicNodeBase {
  Q_OBJECT

public:
  ImageScaleNode();
  void compute() override;
  NodeValidationState validate() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_mode;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_IMAGESCALENODE_HPP
