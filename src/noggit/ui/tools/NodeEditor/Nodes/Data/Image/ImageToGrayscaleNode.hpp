// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_IMAGETOGRAYSCALENODE_HPP
#define NOGGIT_IMAGETOGRAYSCALENODE_HPP

#include <noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class ImageToGrayscaleNode : public LogicNodeBase {
  Q_OBJECT

public:
  ImageToGrayscaleNode();
  void compute() override;
  NodeValidationState validate() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_IMAGETOGRAYSCALENODE_HPP
