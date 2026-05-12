// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_IMAGERESIZENODE_HPP
#define NOGGIT_IMAGERESIZENODE_HPP

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
class ImageResizeNode : public LogicNodeBase {
  Q_OBJECT

public:
  ImageResizeNode();
  void compute() override;
  NodeValidationState validate() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_aspect_ratio_mode;
  QComboBox *_mode;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_IMAGERESIZENODE_HPP
