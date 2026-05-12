#include <QCheckBox>
#include <QClipboard>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QStatusBar>
#include <noggit/Action.hpp>
#include <noggit/ActionManager.hpp>
#include <noggit/DBC.h>
#include <noggit/MapView.h>
#include <noggit/ui/Help.h>
#include <noggit/ui/HelperModels.h>
#include <noggit/ui/ModelImport.h>
#include <noggit/ui/ObjectEditor.h>
#include <noggit/ui/RotationEditor.h>
#include <noggit/ui/TexturingGUI.h>
#include <noggit/ui/Toolbar.h>
#include <noggit/ui/texture_palette_small.hpp>
#include <noggit/ui/texturing_tool.hpp>
#include <noggit/ui/tools/ViewToolbar/Ui/ViewToolbar.hpp>
#include <noggit/ui/windows/noggitWindow/NoggitWindow.hpp>

void MapView::setupFileMenu() {
  auto file_menu(_main_window->_menuBar->addMenu("Editor"));
  connect(this, &QObject::destroyed, file_menu, &QObject::deleteLater);

  ADD_ACTION(file_menu, "Save current tile", "Ctrl+Shift+S", [this] {
    save(save_mode::current);
    emit saved();
  });
  ADD_ACTION(file_menu, "Save changed tiles", QKeySequence::Save, [this] {
    save(save_mode::changed);
    emit saved();
  });
  ADD_ACTION(file_menu, "Save all tiles", "Ctrl+Shift+A", [this] {
    save(save_mode::all);
    emit saved();
  });
  ADD_ACTION(file_menu, "Generate new WDL", "", [this] {
    QMessageBox prompt;
    prompt.setIcon(QMessageBox::Warning);
    prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
    prompt.setText(
        std::string("Warning!\nThis will attempt to load all tiles in the map "
                    "to generate a new WDL."
                    "\nThis is likely to crash if there is any issue with any "
                    "tile, it is recommended that you save your work first. "
                    "Only use this if you really need a fresh WDL.")
            .c_str());
    prompt.setInformativeText(std::string("Are you sure ?").c_str());
    prompt.setStandardButtons(QMessageBox::StandardButton::Yes |
                              QMessageBox::StandardButton::No);
    prompt.setDefaultButton(QMessageBox::No);
    bool answer = prompt.exec() == QMessageBox::StandardButton::Yes;
    if (answer)
      _world->horizon.save_wdl(_world.get(), true);
  });

  ADD_ACTION(file_menu, "Reload tile", "Shift+J", [this] {
    makeCurrent();
    OpenGL::context::scoped_setter const _(::gl, context());
    _world->reload_tile(_camera.position);
    _rotation_editor_need_update = true;
    emit saved();
  });

  file_menu->addSeparator();
  ADD_ACTION_NS(file_menu, "Force uid check on next opening",
                ([this] { _force_uid_check = true; }));
  file_menu->addSeparator();

  ADD_ACTION(file_menu, "Add bookmark", Qt::CTRL + Qt::Key_F5, [this] {
    auto bookmark = Noggit::Project::NoggitProjectBookmarkMap();
    bookmark.position = _camera.position;
    bookmark.camera_pitch = _camera.pitch()._;
    bookmark.camera_yaw = _camera.yaw()._;
    bookmark.map_id = _world->getMapID();
    bookmark.name = gAreaDB.getAreaName(_world->getAreaID(_camera.position));

    _project->createBookmark(bookmark);
  });

  ADD_ACTION(
      file_menu, "Write coordinates to port.txt and copy to clipboard",
      Qt::Key_G, [this] {
        std::stringstream port_command;
        port_command << ".go XYZ " << (ZEROPOINT - _camera.position.z) << " "
                     << (ZEROPOINT - _camera.position.x) << " "
                     << _camera.position.y << " " << _world->getMapID();
        std::ofstream f("ports.txt", std::ios_base::app);
        f << "Map: " << gAreaDB.getAreaName(_world->getAreaID(_camera.position))
          << " on ADT " << std::floor(_camera.position.x / TILESIZE) << " "
          << std::floor(_camera.position.z / TILESIZE) << std::endl;
        f << "Trinity/AC:" << std::endl << port_command.str() << std::endl;
        // f << "ArcEmu:" << std::endl << ".worldport " << _world->getMapID() <<
        // " " << (ZEROPOINT - _camera.position.z) << " " << (ZEROPOINT -
        // _camera.position.x) << " " << _camera.position.y << " " << std::endl
        // << std::endl;
        f.close();
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(port_command.str().c_str(), QClipboard::Clipboard);
      });
}

void MapView::setupEditMenu() {
  auto edit_menu(_main_window->_menuBar->addMenu("Edit"));
  connect(this, &QObject::destroyed, edit_menu, &QObject::deleteLater);

  edit_menu->addSeparator();
  edit_menu->addAction(createTextSeparator("Selected object"));
  edit_menu->addSeparator();
  ADD_ACTION(edit_menu, "Delete", Qt::Key_Delete, [this] {
    NOGGIT_ACTION_MGR->beginAction(this, Noggit::ActionFlags::eOBJECTS_REMOVED);
    DeleteSelectedObjects();
    NOGGIT_ACTION_MGR->endAction();
  });

  ADD_ACTION(edit_menu, "Reset rotation", "Ctrl+R", [this] {
    NOGGIT_ACTION_MGR->beginAction(this,
                                   Noggit::ActionFlags::eOBJECTS_TRANSFORMED);
    ResetSelectedObjectRotation();
    NOGGIT_ACTION_MGR->endAction();
  });
  ADD_ACTION(edit_menu, "Set to ground", Qt::Key_PageDown, [this] {
    NOGGIT_ACTION_MGR->beginAction(this,
                                   Noggit::ActionFlags::eOBJECTS_TRANSFORMED);
    snap_selected_models_to_the_ground();
    NOGGIT_ACTION_MGR->endAction();
  });

  edit_menu->addSeparator();
  edit_menu->addAction(createTextSeparator("Options"));
  edit_menu->addSeparator();
  ADD_TOGGLE_NS(edit_menu, "Locked cursor mode", _locked_cursor_mode);

  edit_menu->addSeparator();
  edit_menu->addAction(createTextSeparator("State"));
  edit_menu->addSeparator();
  ADD_ACTION(edit_menu, "Undo", "Ctrl+Z",
             [this] { NOGGIT_ACTION_MGR->undo(); });
  ADD_ACTION(edit_menu, "Redo", "Ctrl+Shift+Z",
             [this] { NOGGIT_ACTION_MGR->redo(); });
}

void MapView::setupAssistMenu() {
  auto assist_menu(_main_window->_menuBar->addMenu("Assist"));
  connect(this, &QObject::destroyed, assist_menu, &QObject::deleteLater);

  assist_menu->addSeparator();
  assist_menu->addAction(createTextSeparator("Model"));
  assist_menu->addSeparator();
  ADD_ACTION(assist_menu, "Last M2 from WMV", "Shift+V",
             [this] { objectEditor->import_last_model_from_wmv(eMODEL); });
  ADD_ACTION(assist_menu, "Last WMO from WMV", "Alt+V",
             [this] { objectEditor->import_last_model_from_wmv(eWMO); });
  ADD_ACTION_NS(assist_menu, "Helper models",
                ([this] { objectEditor->helper_models_widget->show(); }));

  assist_menu->addSeparator();
  assist_menu->addAction(createTextSeparator("Current ADT"));
  assist_menu->addSeparator();
  ADD_ACTION_NS(assist_menu, "Set Area ID", ([this] {
                  if (_selected_area_id != -1) {
                    NOGGIT_ACTION_MGR->beginAction(
                        this, Noggit::ActionFlags::eCHUNKS_AREAID);
                    _world->setAreaID(_camera.position, _selected_area_id,
                                      true);
                    NOGGIT_ACTION_MGR->endAction();
                  }
                }));

  ADD_ACTION_NS(assist_menu, "Ensure 4 texture layers", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                  _world->ensureAllTilesetsADT(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));

  auto cleanup_menu(assist_menu->addMenu("Clean up"));

  ADD_ACTION_NS(cleanup_menu, "Clear height map", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TERRAIN);
                  _world->clearHeight(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));
  ADD_ACTION_NS(cleanup_menu, "Remove texture duplicates", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                  _world->removeTexDuplicateOnADT(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));
  ADD_ACTION_NS(cleanup_menu, "Clear textures", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                  _world->clearTextures(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));
  ADD_ACTION_NS(cleanup_menu, "Clear textures + set base", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                  _world->setBaseTexture(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));
  ADD_ACTION_NS(cleanup_menu, "Clear shadows", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNK_SHADOWS);
                  _world->clear_shadows(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));
  ADD_ACTION_NS(cleanup_menu, "Clear models", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eOBJECTS_REMOVED);
                  _world->clearAllModelsOnADT(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                  _rotation_editor_need_update = true;
                }));
  ADD_ACTION_NS(cleanup_menu, "Clear duplicate models", ([=, this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      _world->delete_duplicate_model_and_wmo_instances();)
                }));

  auto cur_adt_export_menu(assist_menu->addMenu("Export"));
  ADD_ACTION_NS(cur_adt_export_menu, "Export alphamaps", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  _world->exportADTAlphamap(_camera.position);
                }));

  ADD_ACTION_NS(cur_adt_export_menu, "Export alphamaps (current texture)",
                ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());

                  if (!!Noggit::Ui::selected_texture::get()) {
                    _world->exportADTAlphamap(
                        _camera.position, Noggit::Ui::selected_texture::get()
                                              ->get()
                                              ->file_key()
                                              .filepath());
                  }
                }));

  ADD_ACTION_NS(cur_adt_export_menu, "Export vertex color map", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());

                  _world->exportADTVertexColorMap(_camera.position);
                }));

  QDialog *heightmap_export_params = new QDialog(this);
  heightmap_export_params->setWindowFlags(Qt::Popup);
  heightmap_export_params->setWindowTitle("Heightmap Exporter");
  QVBoxLayout *heightmap_export_params_layout =
      new QVBoxLayout(heightmap_export_params);

  heightmap_export_params_layout->addWidget(
      new QLabel("Min:", heightmap_export_params));
  QDoubleSpinBox *heightmap_export_min =
      new QDoubleSpinBox(heightmap_export_params);
  heightmap_export_min->setRange(-10000000, 10000000);
  heightmap_export_params_layout->addWidget(heightmap_export_min);

  heightmap_export_params_layout->addWidget(
      new QLabel("Max:", heightmap_export_params));
  QDoubleSpinBox *heightmap_export_max =
      new QDoubleSpinBox(heightmap_export_params);
  heightmap_export_max->setRange(-10000000, 10000000);
  heightmap_export_max->setValue(100.0);
  heightmap_export_params_layout->addWidget(heightmap_export_max);

  QPushButton *heightmap_export_okay =
      new QPushButton("Okay", heightmap_export_params);
  heightmap_export_params_layout->addWidget(heightmap_export_okay);

  connect(heightmap_export_min,
          qOverload<double>(&QDoubleSpinBox::valueChanged),
          [=, this](double value) {
            if (!(heightmap_export_max->value() > value))
              heightmap_export_max->setValue(value + 1.0);
          });

  connect(heightmap_export_max,
          qOverload<double>(&QDoubleSpinBox::valueChanged),
          [=, this](double value) {
            if (!(heightmap_export_min->value() < value))
              heightmap_export_min->setValue(value - 1.0);
          });

  connect(heightmap_export_okay, &QPushButton::clicked,
          [=, this]() { heightmap_export_params->accept(); });

  ADD_ACTION_NS(cur_adt_export_menu, "Export heightmap", ([=, this] {
                  QPoint new_pos = QCursor::pos();

                  heightmap_export_params->setGeometry(
                      new_pos.x(), new_pos.y(),
                      heightmap_export_params->width(),
                      heightmap_export_params->height());

                  if (heightmap_export_params->exec() == QDialog::Accepted) {
                    makeCurrent();
                    OpenGL::context::scoped_setter const _(::gl, context());

                    _world->exportADTHeightmap(_camera.position,
                                               heightmap_export_min->value(),
                                               heightmap_export_max->value());
                  }
                }));

  ADD_ACTION_NS(cur_adt_export_menu, "Export normalmap", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  _world->exportADTNormalmap(_camera.position);
                }));

  auto cur_adt_import_menu(assist_menu->addMenu("Import"));

  QDialog *adt_import_params = new QDialog(this);
  adt_import_params->setWindowFlags(Qt::Popup);
  adt_import_params->setWindowTitle("Alphamap Importer");
  QVBoxLayout *adt_import_params_layout = new QVBoxLayout(adt_import_params);

  adt_import_params_layout->addWidget(new QLabel("Layer:", adt_import_params));
  QSpinBox *adt_import_params_layer = new QSpinBox(adt_import_params);
  adt_import_params_layer->setRange(1, 3);
  adt_import_params_layout->addWidget(adt_import_params_layer);

  QPushButton *adt_import_params_okay =
      new QPushButton("Okay", adt_import_params);
  adt_import_params_layout->addWidget(adt_import_params_okay);

  connect(adt_import_params_okay, &QPushButton::clicked,
          [=, this]() { adt_import_params->accept(); });

  ADD_ACTION_NS(
      cur_adt_import_menu, "Import alphamap (file)", ([=, this] {
        QPoint new_pos = QCursor::pos();

        adt_import_params->setGeometry(new_pos.x(), new_pos.y(),
                                       heightmap_export_params->width(),
                                       heightmap_export_params->height());

        if (adt_import_params->exec() == QDialog::Accepted) {
          makeCurrent();
          OpenGL::context::scoped_setter const _(::gl, context());

          QString filepath = QFileDialog::getOpenFileName(
              this, tr("Open alphamap"), "", "PNG file (*.png);;");

          if (!QFileInfo::exists(filepath))
            return;

          QImage img;
          img.load(filepath, "PNG");

          NOGGIT_ACTION_MGR->beginAction(this,
                                         Noggit::ActionFlags::eCHUNKS_TEXTURE);
          _world->importADTAlphamap(_camera.position, img,
                                    adt_import_params_layer->value());
          NOGGIT_ACTION_MGR->endAction();
        }
      }));

  ADD_ACTION_NS(cur_adt_import_menu, "Import alphamap", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());

                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                  _world->importADTAlphamap(_camera.position);
                  NOGGIT_ACTION_MGR->endAction();
                }));

  QDialog *adt_import_height_params = new QDialog(this);
  adt_import_height_params->setWindowFlags(Qt::Popup);
  adt_import_height_params->setWindowTitle("Alphamap Importer");
  QVBoxLayout *adt_import_height_params_layout =
      new QVBoxLayout(adt_import_height_params);

  adt_import_height_params_layout->addWidget(
      new QLabel("Multiplier:", adt_import_height_params));
  QDoubleSpinBox *adt_import_height_params_multiplier =
      new QDoubleSpinBox(adt_import_height_params);
  adt_import_height_params_multiplier->setRange(0, 100000000);
  adt_import_height_params_layout->addWidget(
      adt_import_height_params_multiplier);

  adt_import_height_params_layout->addWidget(
      new QLabel("Mode:", adt_import_height_params));
  QComboBox *adt_import_height_params_mode =
      new QComboBox(adt_import_height_params);
  adt_import_height_params_layout->addWidget(adt_import_height_params_mode);
  adt_import_height_params_mode->addItems(
      {"Set", "Add", "Subtract", "Multiply"});

  QCheckBox *adt_import_height_tiled_edges =
      new QCheckBox("Tiled Edges", adt_import_height_params);
  adt_import_height_params_layout->addWidget(adt_import_height_tiled_edges);

  QPushButton *adt_import_height_params_okay =
      new QPushButton("Okay", adt_import_height_params);
  adt_import_height_params_layout->addWidget(adt_import_height_params_okay);

  connect(adt_import_height_params_okay, &QPushButton::clicked,
          [=, this]() { adt_import_height_params->accept(); });

  ADD_ACTION_NS(cur_adt_import_menu, "Import heightmap (file)", ([=, this] {
                  if (adt_import_height_params->exec() == QDialog::Accepted) {
                    makeCurrent();
                    OpenGL::context::scoped_setter const _(::gl, context());

                    QString filepath = QFileDialog::getOpenFileName(
                        this, tr("Open heightmap (257x257)"), "",
                        "PNG file (*.png);;");

                    if (!QFileInfo::exists(filepath))
                      return;

                    QImage img;
                    img.load(filepath, "PNG");

                    NOGGIT_ACTION_MGR->beginAction(
                        this, Noggit::ActionFlags::eCHUNKS_TERRAIN);
                    _world->importADTHeightmap(
                        _camera.position, img,
                        adt_import_height_params_multiplier->value(),
                        adt_import_height_params_mode->currentIndex(),
                        adt_import_height_tiled_edges->isChecked());
                    NOGGIT_ACTION_MGR->endAction();
                  }
                }));

  ADD_ACTION_NS(cur_adt_import_menu, "Import heightmap", ([=, this] {
                  if (adt_import_height_params->exec() == QDialog::Accepted) {
                    makeCurrent();
                    OpenGL::context::scoped_setter const _(::gl, context());

                    NOGGIT_ACTION_MGR->beginAction(
                        this, Noggit::ActionFlags::eCHUNKS_TERRAIN);
                    _world->importADTHeightmap(
                        _camera.position,
                        adt_import_height_params_multiplier->value(),
                        adt_import_height_params_mode->currentIndex(),
                        adt_import_height_tiled_edges->isChecked());
                    NOGGIT_ACTION_MGR->endAction();
                  }
                }));

  QDialog *adt_import_vcol_params = new QDialog(this);
  adt_import_vcol_params->setWindowFlags(Qt::Popup);
  adt_import_vcol_params->setWindowTitle("Alphamap Importer");
  QVBoxLayout *adt_import_vcol_params_layout =
      new QVBoxLayout(adt_import_vcol_params);

  adt_import_vcol_params_layout->addWidget(
      new QLabel("Mode:", adt_import_vcol_params));
  QComboBox *adt_import_vcol_params_mode =
      new QComboBox(adt_import_vcol_params);
  adt_import_vcol_params_layout->addWidget(adt_import_vcol_params_mode);
  adt_import_vcol_params_mode->addItems({"Set", "Add", "Subtract", "Multiply"});

  QCheckBox *adt_import_vcol_params_mode_tiled_edges =
      new QCheckBox("Tiled Edges", adt_import_vcol_params);
  adt_import_vcol_params_layout->addWidget(
      adt_import_vcol_params_mode_tiled_edges);

  QPushButton *adt_import_vcol_params_okay =
      new QPushButton("Okay", adt_import_vcol_params);
  adt_import_vcol_params_layout->addWidget(adt_import_vcol_params_okay);

  connect(adt_import_vcol_params_okay, &QPushButton::clicked,
          [=, this]() { adt_import_vcol_params->accept(); });

  ADD_ACTION_NS(cur_adt_import_menu, "Import vertex color map (file)",
                ([=, this] {
                  if (adt_import_vcol_params->exec() == QDialog::Accepted) {
                    makeCurrent();
                    OpenGL::context::scoped_setter const _(::gl, context());

                    QString filepath = QFileDialog::getOpenFileName(
                        this, tr("Open vertex color map (257x257)"), "",
                        "PNG file (*.png);;");

                    if (!QFileInfo::exists(filepath))
                      return;

                    QImage img;
                    img.load(filepath, "PNG");

                    NOGGIT_ACTION_MGR->beginAction(
                        this, Noggit::ActionFlags::eCHUNKS_VERTEX_COLOR);
                    _world->importADTVertexColorMap(
                        _camera.position, img,
                        adt_import_vcol_params_mode->currentIndex(),
                        adt_import_vcol_params_mode_tiled_edges->isChecked());
                    NOGGIT_ACTION_MGR->endAction();
                  }
                }));

  ADD_ACTION_NS(cur_adt_import_menu, "Import vertex color map", ([=, this] {
                  if (adt_import_vcol_params->exec() == QDialog::Accepted) {
                    makeCurrent();
                    OpenGL::context::scoped_setter const _(::gl, context());

                    NOGGIT_ACTION_MGR->beginAction(
                        this, Noggit::ActionFlags::eCHUNKS_VERTEX_COLOR);
                    _world->importADTVertexColorMap(
                        _camera.position,
                        adt_import_vcol_params_mode->currentIndex(),
                        adt_import_vcol_params_mode_tiled_edges->isChecked());
                    NOGGIT_ACTION_MGR->endAction();
                  }
                }));

  assist_menu->addSeparator();
  assist_menu->addAction(createTextSeparator("Loaded ADTs"));
  assist_menu->addSeparator();
  ADD_ACTION_NS(assist_menu, "Fix gaps", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  NOGGIT_ACTION_MGR->beginAction(
                      this, Noggit::ActionFlags::eCHUNKS_TERRAIN);
                  _world->fixAllGaps();
                  NOGGIT_ACTION_MGR->endAction();
                }));

  assist_menu->addSeparator();
  assist_menu->addAction(createTextSeparator("Global"));
  assist_menu->addSeparator();
  ADD_ACTION_NS(assist_menu, "Map to big alpha", ([=, this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      _world->convertAlphamap(true);)
                }));
  ADD_ACTION_NS(assist_menu, "Map to old alpha", ([=, this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      _world->convertAlphamap(false);)
                }));

  ADD_ACTION_NS(assist_menu, "Ensure 4 texture layers", ([=, this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      _world->ensureAllTilesetsAllADTs();)
                }));

  auto all_adts_export_menu(assist_menu->addMenu("Export"));

  ADD_ACTION_NS(all_adts_export_menu, "Export alphamaps", ([=, this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      _world->exportAllADTsAlphamap();)
                }));

  ADD_ACTION_NS(
      all_adts_export_menu, "Export alphamaps (current texture)", ([this] {
        DESTRUCTIVE_ACTION(
            makeCurrent();
            OpenGL::context::scoped_setter const _(::gl, context());

            if (!!Noggit::Ui::selected_texture::get()) {
              _world->exportAllADTsAlphamap(Noggit::Ui::selected_texture::get()
                                                ->get()
                                                ->file_key()
                                                .filepath());
            })
      }));

  ADD_ACTION_NS(all_adts_export_menu, "Export heightmap", ([this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());

                      _world->exportAllADTsHeightmap();)
                }));

  ADD_ACTION_NS(all_adts_export_menu, "Export vertex color map", ([this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());

                      _world->exportAllADTsVertexColorMap();)
                }));

  auto all_adts_import_menu(assist_menu->addMenu("Import"));

  ADD_ACTION_NS(all_adts_import_menu, "Import alphamaps", ([this] {
                  DESTRUCTIVE_ACTION(
                      makeCurrent();
                      OpenGL::context::scoped_setter const _(::gl, context());
                      NOGGIT_ACTION_MGR->beginAction(
                          this, Noggit::ActionFlags::eCHUNKS_TEXTURE);
                      _world->importAllADTsAlphamaps();
                      NOGGIT_ACTION_MGR->endAction();

                  )
                }));

  ADD_ACTION_NS(all_adts_import_menu, "Import heightmaps", ([=, this] {
                  if (adt_import_height_params->exec() == QDialog::Accepted) {
                    DESTRUCTIVE_ACTION(
                        makeCurrent();
                        OpenGL::context::scoped_setter const _(::gl, context());
                        NOGGIT_ACTION_MGR->beginAction(
                            this, Noggit::ActionFlags::eCHUNKS_TERRAIN);
                        _world->importAllADTsHeightmaps(
                            adt_import_height_params_multiplier->value(),
                            adt_import_height_params_mode->currentIndex(),
                            adt_import_height_tiled_edges->isChecked());
                        NOGGIT_ACTION_MGR->endAction();)
                  }
                }));

  ADD_ACTION_NS(
      all_adts_import_menu, "Import vertex color maps", ([=, this] {
        if (adt_import_vcol_params->exec() == QDialog::Accepted) {
          DESTRUCTIVE_ACTION(
              makeCurrent();
              OpenGL::context::scoped_setter const _(::gl, context());
              NOGGIT_ACTION_MGR->beginAction(
                  this, Noggit::ActionFlags::eCHUNKS_VERTEX_COLOR);
              _world->importAllADTVertexColorMaps(
                  adt_import_vcol_params_mode->currentIndex(),
                  adt_import_vcol_params_mode_tiled_edges->isChecked());
              NOGGIT_ACTION_MGR->endAction();)
        }
      }));

  auto debug_menu(assist_menu->addMenu("Debug"));

  ADD_ACTION_NS(debug_menu, "Load all tiles", ([=, this] {
                  makeCurrent();
                  OpenGL::context::scoped_setter const _(::gl, context());
                  _world->loadAllTiles();
                }));
}

void MapView::setupViewMenu() {
  auto view_menu(_main_window->_menuBar->addMenu("View"));
  connect(this, &QObject::destroyed, view_menu, &QObject::deleteLater);

  view_menu->addSeparator();
  view_menu->addAction(createTextSeparator("Drawing"));
  view_menu->addSeparator();
  ADD_TOGGLE(view_menu, "Doodads", Qt::Key_F1, _draw_models);
  ADD_TOGGLE(view_menu, "WMO doodads", Qt::Key_F2, _draw_wmo_doodads);
  ADD_TOGGLE(view_menu, "Terrain", Qt::Key_F3, _draw_terrain);
  ADD_TOGGLE(view_menu, "Water", Qt::Key_F4, _draw_water);
  ADD_TOGGLE(view_menu, "WMOs", Qt::Key_F6, _draw_wmo);

  ADD_TOGGLE_POST(
      view_menu, "Lines", Qt::Key_F7, _draw_lines, ([=, this] {
        _world->renderer()->getTerrainParamsUniformBlock()->draw_lines =
            _draw_lines.get();
        _world->renderer()->markTerrainParamsUniformBlockDirty();
      }));

  ADD_TOGGLE_POST(view_menu, "Contours", Qt::Key_F9, _draw_contour, ([=, this] {
                    _world->renderer()
                        ->getTerrainParamsUniformBlock()
                        ->draw_terrain_height_contour = _draw_contour.get();
                    _world->renderer()->markTerrainParamsUniformBlockDirty();
                  }));

  ADD_TOGGLE_POST(
      view_menu, "Wireframe", Qt::Key_F10, _draw_wireframe, ([=, this] {
        _world->renderer()->getTerrainParamsUniformBlock()->draw_wireframe =
            _draw_wireframe.get();
        _world->renderer()->markTerrainParamsUniformBlockDirty();
      }));

  ADD_TOGGLE(view_menu, "Toggle Animation", Qt::Key_F11,
             _draw_model_animations);
  ADD_TOGGLE(view_menu, "Draw fog", Qt::Key_F12, _draw_fog);

  ADD_TOGGLE_POST(
      view_menu, "Hole lines", Qt::SHIFT + Qt::Key_F1, _draw_hole_lines,
      ([=, this] {
        _world->renderer()->getTerrainParamsUniformBlock()->draw_hole_lines =
            _draw_hole_lines.get();
        _world->renderer()->markTerrainParamsUniformBlockDirty();
      }));

  ADD_TOGGLE_POST(view_menu, "Climb", Qt::SHIFT + Qt::Key_F2, _draw_climb,
                  ([=, this] {
                    _world->renderer()
                        ->getTerrainParamsUniformBlock()
                        ->draw_impassible_climb = _draw_climb.get();
                    _world->renderer()->markTerrainParamsUniformBlockDirty();
                  }));

  ADD_TOGGLE_POST(
      view_menu, "Vertex Color", Qt::SHIFT + Qt::Key_F3, _draw_vertex_color,
      ([=, this] {
        _world->renderer()->getTerrainParamsUniformBlock()->draw_vertex_color =
            _draw_vertex_color.get();
        _world->renderer()->markTerrainParamsUniformBlockDirty();
      }));

  ADD_TOGGLE_POST(
      view_menu, "Baked Shadows", Qt::SHIFT + Qt::Key_F4, _draw_baked_shadows,
      ([=, this] {
        _world->renderer()->getTerrainParamsUniformBlock()->draw_shadows =
            _draw_baked_shadows.get();
        _world->renderer()->markTerrainParamsUniformBlockDirty();
      }));

  ADD_TOGGLE_NS(view_menu, "Flight Bounds", _draw_mfbo);

  ADD_TOGGLE_NS(view_menu, "Models with box", _draw_models_with_box);
  //! \todo space+h in object mode
  ADD_TOGGLE_NS(view_menu, "Hidden models", _draw_hidden_models);

  ADD_TOGGLE_NS(view_menu, "Game Mode", _game_mode_camera);

  auto debug_menu(view_menu->addMenu("Debug"));
  ADD_TOGGLE_NS(debug_menu, "Occlusion boxes", _draw_occlusion_boxes);

  view_menu->addSeparator();
  view_menu->addAction(createTextSeparator("Tools"));
  view_menu->addSeparator();

  ADD_TOGGLE(view_menu, "Show Node Editor", "Shift+N", _show_node_editor);

  view_menu->addSeparator();
  view_menu->addAction(createTextSeparator("Minimap"));
  view_menu->addSeparator();

  ADD_TOGGLE(view_menu, "Show", Qt::Key_M, _show_minimap_window);

  ADD_TOGGLE_NS(view_menu, "Show ADT borders", _show_minimap_borders);

  ADD_TOGGLE_NS(view_menu, "Show light zones", _show_minimap_skies);

  view_menu->addSeparator();
  view_menu->addAction(createTextSeparator("Windows"));
  view_menu->addSeparator();

  auto hide_widgets = [=, this] {
    QWidget *widget_list[] = {_texture_browser_dock,
                              _texture_picker_dock,
                              _detail_infos_dock,
                              _keybindings,
                              _minimap_dock,
                              objectEditor->modelImport,
                              objectEditor->rotationEditor,
                              objectEditor->helper_models_widget,
                              _texture_palette_small,
                              _object_palette_dock,
                              _asset_browser_dock,
                              _overlay_widget,
                              _tool_panel_dock

    };

    if (_main_window->displayed_widgets.empty()) {
      for (auto widget : widget_list)
        if (widget->isVisible()) {
          _main_window->displayed_widgets.emplace(widget);
          widget->hide();
        }

    } else {
      for (auto widget : _main_window->displayed_widgets)
        widget->show();

      _main_window->displayed_widgets.clear();
    }

    _main_window->statusBar()->setVisible(ui_hidden);
    _toolbar->setVisible(ui_hidden);
    _view_toolbar->setVisible(ui_hidden);

    ui_hidden = !ui_hidden;

    setToolPropertyWidgetVisibility(terrainMode);
  };

  ADD_ACTION(view_menu, "Toggle UI", Qt::Key_Tab, hide_widgets);

  ADD_TOGGLE(view_menu, "Detail infos", Qt::Key_F8, _show_detail_info_window);

  ADD_TOGGLE(view_menu, "Texture Browser", Qt::Key_X,
             _show_texture_palette_window);

  ADD_TOGGLE_NS(view_menu, "Texture palette",
                _show_texture_palette_small_window);

  addHotkey(
      Qt::Key_H, MOD_none,
      [this] { _show_texture_palette_small_window.toggle(); },
      [this] { return terrainMode == editing_mode::paint; });

  ADD_ACTION(view_menu, "Increase time speed", Qt::Key_N,
             [this] { mTimespeed += 90.0f; });
  ADD_ACTION(view_menu, "Decrease time speed", Qt::Key_B,
             [this] { mTimespeed = std::max(0.0f, mTimespeed - 90.0f); });
  ADD_ACTION(view_menu, "Pause time", Qt::Key_J, [this] { mTimespeed = 0.0f; });
  ADD_ACTION(view_menu, "Invert mouse", "I", [this] { mousedir *= -1.f; });
  ADD_ACTION(view_menu, "Decrease camera speed", Qt::Key_O,
             [this] { _camera.move_speed *= 0.5f; });
  ADD_ACTION(view_menu, "Increase camera speed", Qt::Key_P,
             [this] { _camera.move_speed *= 2.0f; });
  ADD_ACTION(view_menu, "Turn camera around 180°", "Shift+R", [this] {
    _camera.add_to_yaw(math::degrees(180.f));
    _camera_moved_since_last_draw = true;
  });

  ADD_ACTION(view_menu, "Toggle tile mode", Qt::Key_U, [this] {
    if (NOGGIT_CUR_ACTION)
      return;

    if (_display_mode == display_mode::in_2D) {
      _display_mode = display_mode::in_3D;
      set_editing_mode(saveterrainMode);
    } else {
      _display_mode = display_mode::in_2D;
      saveterrainMode = terrainMode;
      set_editing_mode(editing_mode::paint);
    }
  });
}

void MapView::setupHelpMenu() {
  auto help_menu(_main_window->_menuBar->addMenu("Help"));
  connect(this, &QObject::destroyed, help_menu, &QObject::deleteLater);

  ADD_TOGGLE(help_menu, "Key Bindings", "Ctrl+F1", _show_keybindings_window);

#if defined(_WIN32) || defined(WIN32)
  ADD_ACTION_NS(help_menu, "WoW Modding Discord", [] {
    ShellExecute(nullptr, "open", "https://discord.gg/Dnrztg7dCZ", nullptr,
                 nullptr, SW_SHOWNORMAL);
  });
  ADD_ACTION_NS(help_menu, "Noggit Red Repository", [] {
    ShellExecute(nullptr, "open",
                 "https://gitlab.com/prophecy-rp/noggit-red/-/tree/"
                 "noggit-shadowlands?ref_type=heads",
                 nullptr, nullptr, SW_SHOWNORMAL);
  });

  ADD_ACTION_NS(help_menu, "Noggit Red Discord", [] {
    ShellExecute(nullptr, "open", "https://discord.gg/Tk2TpN8CaF", nullptr,
                 nullptr, SW_SHOWNORMAL);
  });
#endif
}
