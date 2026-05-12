// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_DELETEVARIABLENODE_HPP
#define NOGGIT_DELETEVARIABLENODE_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/LogicNodeBase.hpp"
#include <external/tsl/robin_map.h>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
using VariableMap =
    tsl::robin_map<std::string,
                   std::pair<std::string, std::shared_ptr<NodeData>>>;

class DeleteVariableNodeBase : public LogicNodeBase {
  Q_OBJECT

public:
  DeleteVariableNodeBase();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

protected:
  virtual VariableMap *getVariableMap() = 0;
};

// Scene scope

class DeleteVariableNode : public DeleteVariableNodeBase {
  Q_OBJECT

public:
  DeleteVariableNode();

protected:
  VariableMap *getVariableMap() override;
};

// Context scope

class DeleteContextVariableNode : public DeleteVariableNodeBase {
  Q_OBJECT

public:
  DeleteContextVariableNode();

protected:
  VariableMap *getVariableMap() override;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_DELETEVARIABLENODE_HPP
