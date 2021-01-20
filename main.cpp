#include "appConfig.hpp"

//#include "translatorJSONQtso/translator.hpp"

#include "signalso/signal.hpp"
#include "signalProxyQtso/signalProxyQtso.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    MACRO_signalHandler

    QCoreApplication qAppTmp(argc, argv);
    QCoreApplication::setApplicationName("Path monotifier");
    QCoreApplication::setApplicationVersion("1.0");

    signalso::signalProxy_ptr_ext = new signalso::signalProxy_c(std::addressof(qAppTmp));

    appConfig_c appConfigTmp(nullptr);
    appConfig_ptr_ext = std::addressof(appConfigTmp);

    if (appConfigTmp.configLoaded_f())
    {
        QTimer::singleShot(0, std::addressof(appConfigTmp), &appConfig_c::startMonitoringPaths_f);
        qAppTmp.exec();
    }
    else
    {
        qtOutLine_f("Couldn't load any config file, pass \"-g\" argument to generate a config file documentation file");
    }
    qtOutLine_f("\n");
}
