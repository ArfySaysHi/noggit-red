// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_DATALISTNODE_HPP
#define NOGGIT_DATALISTNODE_HPP

#include "noggit/ui/tools/NodeEditor/Nodes/Containers/List/ListNodeBase.hpp"

#include <QComboBox>
#include <external/tsl/robin_map.h>
#include <vector>

using QtNodes::NodeData;
using QtNodes::NodeDataModel;
using QtNodes::NodeDataType;
using QtNodes::NodeValidationState;
using QtNodes::PortIndex;
using QtNodes::PortType;

namespace Noggit {
namespace Ui::Tools::NodeEditor::Nodes {
class DataListNode : public ListNodeBase {
  Q_OBJECT

public:
  DataListNode();
  void compute() override;
  QJsonObject save() const override;
  void restore(QJsonObject const &json_obj) override;

private:
  QComboBox *_type;

  std::vector<std::shared_ptr<NodeData>> _data;
};

} // namespace Ui::Tools::NodeEditor::Nodes

} // namespace Noggit

#endif // NOGGIT_DATALISTNODE_HPP
