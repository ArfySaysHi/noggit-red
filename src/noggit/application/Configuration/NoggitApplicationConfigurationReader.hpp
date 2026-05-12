#ifndef NOGGIT_APPLICATION_CONFIGURATION_READER_HPP
#define NOGGIT_APPLICATION_CONFIGURATION_READER_HPP

#include <QFile>
#include <noggit/application/Configuration/NoggitApplicationConfiguration.hpp>

namespace Noggit::Application {

class NoggitApplicationConfigurationReader {
public:
  NoggitApplicationConfiguration ReadConfigurationState(QFile &inputFile);
};
} // namespace Noggit::Application
#endif // NOGGIT_APPLICATION_CONFIGURATION_READER_HPP