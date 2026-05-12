// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_NODEEDITOR_HPP
#define NOGGIT_NODEEDITOR_HPP

#include "../NodeRegistry.hpp"
#include <ui_NodeEditor.h>

#include <QFileSystemModel>
#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QWidget>

#include <noggit/ui/tools/PreviewRenderer/PreviewRenderer.hpp>

namespace Noggit {
namespace Ui::Tools::NodeEditor::Ui {
class NodeEditorWidget : public QMainWindow {
public:
  explicit NodeEditorWidget(QWidget *parent = nullptr);
  ~NodeEditorWidget() override;

  void loadScene(QString const &filepath);

private:
  ::Ui::NodeEditor *ui;

  QFileSystemModel *_model;
  QSortFilterProxyModel *_sort_model;
  PreviewRenderer *_preview_renderer;
};
} // namespace Ui::Tools::NodeEditor::Ui
} // namespace Noggit

#endif // NOGGIT_NODEEDITOR_HPP
