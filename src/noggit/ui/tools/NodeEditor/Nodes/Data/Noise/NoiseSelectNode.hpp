// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISESELECTNODE_HPP
#define NOGGIT_NOISESELECTNODE_HPP

#include <external/libnoise/src/noise/noise.h>
#include <noggit/ui/tools/NodeEditor/Nodes/BaseNode.hpp>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class NoiseSelectNode : public BaseNode {
  Q_OBJECT

public:
  NoiseSelectNode();
  NodeValidationState validate() override;
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  noise::module::Select _module;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISESELECTNODE_HPP
