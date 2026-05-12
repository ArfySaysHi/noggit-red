// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISEMATHNODE_HPP
#define NOGGIT_NOISEMATHNODE_HPP

#include <QComboBox>
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
class NoiseMathNode : public BaseNode {
  Q_OBJECT

public:
  NoiseMathNode();
  NodeValidationState validate() override;
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  std::unique_ptr<noise::module::Module> _module;
  int _last_module = -1;
  QComboBox *_operation;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISEMATHNODE_HPP
