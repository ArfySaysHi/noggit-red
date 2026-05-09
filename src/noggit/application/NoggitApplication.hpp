// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#ifndef NOGGIT_APPLICATION_HPP
#define NOGGIT_APPLICATION_HPP

#include <ClientData.hpp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSplashScreen>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtGui/QOffscreenSurface>
#include <QtOpenGLContext>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <filesystem>
#include <memory>
#include <noggit/application/Configuration/NoggitApplicationConfiguration.hpp>
#include <noggit/application/Configuration/NoggitApplicationConfigurationReader.hpp>
#include <noggit/application/Configuration/NoggitApplicationConfigurationWriter.hpp>
#include <noggit/ui/windows/noggitWindow/NoggitWindow.hpp>
#include <noggit/ui/windows/projectSelection/NoggitProjectSelectionWindow.hpp>
#include <revision.h>
#include <string>
#include <string_view>
#include <util/exception_to_string.hpp>
#include <vector>

namespace Noggit::Ui::Windows {
class NoggitProjectSelectionWindow;
}

namespace Noggit::Application {

class NoggitApplication {
public:
  static NoggitApplication *instance() {
    static NoggitApplication inst{};
    return &inst;
  }

  BlizzardArchive::ClientData *clientData() { return _client_data.get(); }
  void setClientData(std::shared_ptr<BlizzardArchive::ClientData> data) {
    _client_data = data;
  }

  void initalize(int argc, char *argv[], std::vector<bool> Parser);
  std::shared_ptr<Noggit::Application::NoggitApplicationConfiguration>
  getConfiguration();
  static void terminationHandler();
  bool GetCommand(int index);

protected:
  std::vector<bool> Command;

private:
  NoggitApplication() = default;

  std::shared_ptr<Noggit::Application::NoggitApplicationConfiguration>
      _application_configuration;
  std::unique_ptr<Noggit::Ui::Windows::NoggitProjectSelectionWindow>
      _project_selection_page;
  std::shared_ptr<BlizzardArchive::ClientData> _client_data;
};

} // namespace Noggit::Application

#endif // NOGGIT_APPLICATION_HPP
