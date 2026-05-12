// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NOISECONSTVALUENODE_HPP
#define NOGGIT_NOISECONSTVALUENODE_HPP

#include "NoiseGeneratorBase.hpp"
#include <external/libnoise/src/noise/noise.h>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class NoiseConstValueNode : public NoiseGeneratorBase {
  Q_OBJECT

public:
  NoiseConstValueNode();
  void compute() override;

private:
  noise::module::Const _module;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_NOISECONSTVALUENODE_HPP
