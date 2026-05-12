// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISEVIEWERNODE_HPP
#define NOGGIT_NOISEVIEWERNODE_HPP

#include <QLabel>
#include <QPushButton>
#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class NoiseViewerNode : public BaseNode {
  Q_OBJECT

public:
  NoiseViewerNode();
  NodeValidationState validate() override;
  void compute() override;

private:
  QPushButton *_update_btn;
  QLabel *_image;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISEVIEWERNODE_HPP
