#include "appConfig.hpp"
#include "execution.hpp"

#include "essentialQtso/essentialQt.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "signalProxyQtso/signalProxyQtso.hpp"

#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>

void appConfig_c::derivedReadJSON_f(const QJsonObject& json_par_con)
{
    if (not json_par_con["paths"].isUndefined() and json_par_con["paths"].isArray())
    {
        QJsonArray pathArrayTmp(json_par_con["paths"].toArray());
        pathConfigs_pri.reserve(pathArrayTmp.size());
        for (const auto item_ite_con : pathArrayTmp)
        {
            pathConfig_c pathConfigTmp;
            pathConfigTmp.read_f(item_ite_con.toObject());
            pathConfigs_pri.emplace_back(pathConfigTmp);
        }
    }
}

void appConfig_c::derivedWriteJSONDocumented_f(QJsonObject& json_par) const
{
    QJsonArray jsonArrayFileDialongSettingsTmp;
    QJsonObject jsonObjectTmp;
    pathConfig_c empty;
    empty.writeJSONDocumented_f(jsonObjectTmp);
    jsonArrayFileDialongSettingsTmp.append(jsonObjectTmp);
    json_par["pathsDescription"] = "(this is not an actual configuration field) must be one or more objects";
    json_par["paths"] = jsonArrayFileDialongSettingsTmp;
}

void appConfig_c::derivedConfigureCommandLineParser_f(QCommandLineParser& ) const
{
}

void appConfig_c::derivedCheckCommandLineParser_f(QCommandLineParser& )
{
}

void appConfig_c::startMonitoringPaths_f()
{
    bool anythingValid(false);
    for (const pathConfig_c& pathConfig_ite_con : pathConfigs_pri)
    {
        textCompilation_c errorsTmp;
        if (pathConfig_ite_con.isValid_f(std::addressof(errorsTmp)))
        {
            anythingValid = true;
            qtOutLine_f("Monitoring path " + pathConfig_ite_con.path_f());
            folderChangeReactionActionExecution_c* objPtr = new folderChangeReactionActionExecution_c(this, pathConfig_ite_con);
            QObject::connect(this, &appConfig_c::stopMonitoringPaths_signal, objPtr, &folderChangeReactionActionExecution_c::stopMonitoring_f);
            QObject::connect(objPtr, &folderChangeReactionActionExecution_c::monitoringStopped_signal, this, &appConfig_c::checkPathConfigFinishedExecutions_f);

            pathConfigExecutionsStarted_pri = pathConfigExecutionsStarted_pri + 1;
            objPtr->executeMonitoring_f();
        }
        if (not errorsTmp.empty_f())
        {
            qtOutLine_f("Path config \"" + pathConfig_ite_con.path_f() + "\" errors:\n" + errorsTmp.toRawReplace_f());
        }
    }
    if (pathConfigs_pri.empty() or not anythingValid)
    {
        qtOutLine_f("No valid path config/s loaded");
        Q_EMIT quit_signal();
    }
    else
    {
        QObject::connect(signalso::signalProxy_ptr_ext, &signalso::signalProxy_c::signalTriggered_signal, this, &appConfig_c::stopMonitoringPaths_signal);
        qtOutLine_f("SIGTINT/SIGTERM/SIGHUP/SIGBREAK to stop");
    }
}

void appConfig_c::checkPathConfigFinishedExecutions_f()
{
    pathConfigExecutionsFinished_pri = pathConfigExecutionsFinished_pri + 1;
    if (pathConfigExecutionsStarted_pri >= pathConfigExecutionsFinished_pri)
    {
        Q_EMIT quit_signal();
    }
}

appConfig_c::appConfig_c(QObject* parent_par)
    : programConfigConsole_c(parent_par)
{
    threadedFunction_c::setMaxUsableThreads_f(8);
}

void appConfig_c::derivedStart_f()
{
    signalso::signalProxy_ptr_ext = new signalso::signalProxy_c(qApp);
    appConfig_ptr_ext = this;

    loadConfigFile_f();
    tryLoadTranslations_f();
    loadLogging_f();

    if (configLoaded_f())
    {
        startMonitoringPaths_f();
    }
    else
    {
        messageUser_f({"Couldn't load {0} config file, pass \"-g\" argument to generate a config file documentation file", configFilePath_f()}, messageType_ec::information);
        Q_EMIT quit_signal();
    }
}

void appConfig_c::derivedQuit_f()
{
    saveTranslationFile_f();
}


appConfig_c* appConfig_ptr_ext = nullptr;
