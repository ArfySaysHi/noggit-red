// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_IMAGECREATENODE_HPP
#define NOGGIT_IMAGECREATENODE_HPP

#include <QComboBox>
#include <noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ImageCreateNode : public LogicNodeBase {
  Q_OBJECT

public:
  ImageCreateNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_image_format;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_IMAGECREATENODE_HPP
