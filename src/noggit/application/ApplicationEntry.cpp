// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).
#include <QSplashScreen>
#include <QStyleFactory>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <codecvt>
#include <external/framelesshelper/framelesswindowsmanager.h>
#include <noggit/Log.h>
#include <noggit/application/NoggitApplication.hpp>
#include <noggit/errorHandling.h>
#include <opengl/context.hpp>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>
#include <string>
#include <string_view>
#include <util/exception_to_string.hpp>

QCommandLineParser *ProcessCommandLine() {
  QCommandLineParser *parser = new QCommandLineParser();
  parser->setApplicationDescription("Help");
  parser->addHelpOption();
  parser->addVersionOption();
  parser->addOptions(
      {{"disable-update",
        QApplication::translate("main", "Disable the check for update.")},
       {"force-changelog",
        QApplication::translate("main",
                                "Force displaying the changelog popup.")}});

  return parser;
}

int main(int argc, char *argv[]) {
  Noggit::RegisterErrorHandlers();
  std::set_terminate(
      Noggit::Application::NoggitApplication::terminationHandler);

  QApplication::setStyle(QStyleFactory::create("Fusion"));
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication q_application(argc, argv);
  q_application.setApplicationName("Noggit");
  q_application.setOrganizationName("Noggit");

  auto parser = ProcessCommandLine();
  parser->process(q_application);

  std::vector<bool> Command;
  Command.push_back(parser->isSet("disable-update"));
  Command.push_back(parser->isSet("force-changelog"));

  auto noggit = Noggit::Application::NoggitApplication::instance();
  noggit->initalize(argc, argv, Command);

  auto project_selection =
      new Noggit::Ui::Windows::NoggitProjectSelectionWindow(noggit);
  project_selection->show();

  return q_application.exec();
}
