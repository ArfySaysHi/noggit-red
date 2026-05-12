// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_LOADIMAGENODE_HPP
#define NOGGIT_LOADIMAGENODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class LoadImageNode : public LogicNodeBase {
  Q_OBJECT

public:
  LoadImageNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

public Q_SLOTS:
  void inputConnectionCreated(Connection const &connection) override;
  void inputConnectionDeleted(Connection const &connection) override;

private:
  QPushButton *_load_button;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_LOADIMAGENODE_HPP
