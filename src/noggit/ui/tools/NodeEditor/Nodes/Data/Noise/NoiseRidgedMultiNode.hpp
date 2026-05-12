// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISERIDGEDMULTINODE_HPP
#define NOGGIT_NOISERIDGEDMULTINODE_HPP

#include "NoiseGeneratorBase.hpp"
#include <QComboBox>
#include <external/libnoise/src/noise/noise.h>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class NoiseRidgedMultiNode : public NoiseGeneratorBase {
  Q_OBJECT

public:
  NoiseRidgedMultiNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_quality;
  noise::module::RidgedMulti _module;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISERIDGEDMULTINODE_HPP
