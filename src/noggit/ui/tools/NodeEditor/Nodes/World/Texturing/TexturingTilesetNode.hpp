// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_TEXTURINGTILESETNODE_HPP
#define NOGGIT_TEXTURINGTILESETNODE_HPP

#include <QLabel>
#include <QPushButton>
#include <noggit/ui/tools/NodeEditor/Nodes/ContextNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class TexturingTilesetNode : public ContextNodeBase {
  Q_OBJECT

public:
  TexturingTilesetNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QLabel *_image;
  QPushButton *_update_btn;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_TEXTURINGTILESETNODE_HPP
