#include "appConfig.hpp"
#include "execution.hpp"

#include "essentialQtso/essentialQt.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "signalProxyQtso/signalProxyQtso.hpp"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

void appConfig_c::read_f(const QJsonObject& json_par_con)
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

    if (not json_par_con["translationConfigFile"].isUndefined())
    {
        translationConfigFileSet_pri = true;
        translationConfigFile_pri = json_par_con["translationConfigFile"].toString();
    }
    if (not json_par_con["logsDirectoryPath"].isUndefined())
    {
        logsDirectoryPathSet_pri = true;
        logsDirectoryPath_pri = json_par_con["logsDirectoryPath"].toString();
    }
    if (not json_par_con["logging"].isUndefined())
    {
        logging_pri = json_par_con["logging"].toBool(logging_pri);
    }
}

void appConfig_c::writeJSONDocumented_f(QJsonObject& json_par) const
{
    QJsonArray jsonArrayFileDialongSettingsTmp;
    QJsonObject jsonObjectTmp;
    pathConfig_c empty;
    empty.writeJSONDocumented_f(jsonObjectTmp);
    jsonArrayFileDialongSettingsTmp.append(jsonObjectTmp);
    json_par["pathsDescription"] = "(this is not an actual configuration field) must be one or more objects";
    json_par["paths"] = jsonArrayFileDialongSettingsTmp;

    json_par["translationConfigFile"] = "path to translation file, the file can be empty or not exist (it will be created with placeholders)";

    json_par["logsDirectoryPath"] = "path log directory";
    json_par["logging"] = "true, log extra information in the log file, it's not recommended to enable for regular use, it will fill the log file with information of each notification, false, minimum logging, default is false";
}

logDataHub_c* appConfig_c::logDataHub_f()
{
    return std::addressof(logDataHub_pri);
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
            pathMonitifierExecution_c* objPtr = new pathMonitifierExecution_c(this, pathConfig_ite_con);
            QObject::connect(this, &appConfig_c::stopMonitoringPaths_signal, objPtr, &pathMonitifierExecution_c::stopMonitoring_f);
            QObject::connect(objPtr, &pathMonitifierExecution_c::monitoringStopped_signal, this, &appConfig_c::checkPathConfigFinishedExecutions_f);

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
    //stop (or not) logging
    logDataHub_pri.setLoggingEnabled_f(logging_pri);
}

void appConfig_c::quit_f()
{
    if (sender() == this)
    {
        qApp->quit();
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

void appConfig_c::tryLoadTranslations_f()
{
    //check json field, else use default path
    if (translationConfigFile_pri.isEmpty())
    {
        translationConfigFile_pri = fileTypePath_f(fileTypes_ec::translationConfig);
    }

    if (QFile::exists(translationConfigFile_pri))
    {
        translator_pri.readConfigJSONFile_f(translationConfigFile_pri);
        if (translator_pri.isConfigSet_f())
        {
            MACRO_ADDMESSAGE(logDataHub_pri, "Translation/s loaded successful", logItem_c::type_ec::info);
        }
        else
        {

        }
    }

#ifdef DEBUGJOUVEN
    translator_pri.setAddNotFoundKeys_f(true);
#endif
    if (translator_pri.addNotFoundKeys_f() and not translator_pri.isConfigSet_f())
    {
        MACRO_ADDMESSAGE(logDataHub_pri, R"(Translation/s not loaded, adding "empty", "hard-coded"-"english" translation)", logItem_c::type_ec::info);
        translator_pri.addEmptyLanguageLink_f("hard-coded", "english");
        translator_pri.setTranslateFromLanguage_f("hard-coded");
        translator_pri.setTranslateToLanguageChain_f({"english"});
    }
    logDataHub_pri.setTranslatorPtr_f(std::addressof(translator_pri));
}

appConfig_c::appConfig_c(QObject* parent_par)
    : QObject(parent_par), logDataHub_pri(nullptr)
{
    QObject::connect(this, &appConfig_c::quit_signal, this, &appConfig_c::quit_f, Qt::QueuedConnection);
    threadedFunction_c::setMaxConcurrentQThreads_f(8);

    QCommandLineParser parser;
    parser.setApplicationDescription("Monitor and notify file system changes of a path/s i.e. new files, file size change, modified datetime change... \nCreated by Jouven");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption configFilePathOption("configFile", "Config file path", "configFilePath");
    parser.addOption(configFilePathOption);
    QCommandLineOption generateConfigFileDocumentationOption("g", "Generate config file documentation");
    parser.addOption(generateConfigFileDocumentationOption);

    parser.process(*qApp);

    locateConfigFilePath_f(parser, false, true);

    //check for -g
    bool generateConfigFileDocumentation(parser.isSet(generateConfigFileDocumentationOption));
    while (generateConfigFileDocumentation)
    {
        QString exampleFilenameStr(fileTypeBasePath_f(fileTypes_ec::config) + "_example" + fileTypeExtension_f(fileTypes_ec::config));

        if (QFile::exists(exampleFilenameStr))
        {
            qtOutLine_f("An example file: " + exampleFilenameStr + " already exists");
            break;
        }

        QFile exampleFile(exampleFilenameStr);
        if (exampleFile.open(QIODevice::WriteOnly))
        {
            QJsonObject tmp;
            writeJSONDocumented_f(tmp);
            QJsonDocument jsonDocTmp(tmp);
            QByteArray jsonByteArray(jsonDocTmp.toJson(QJsonDocument::JsonFormat::Indented));
            if (exampleFile.write(jsonByteArray) not_eq -1)
            {
                qtOutLine_f("Example file: " + exampleFilenameStr + " generated successfully");
            }
            else
            {
                qtOutLine_f("Error writing example file: " + exampleFilenameStr);
            }
        }
        else
        {
            qtOutLine_f("Can't create/write example file: " + exampleFilenameStr);
        }

        break;
    }

    //(try) load config
    while (not generateConfigFileDocumentation and configFilePath_f().second)
    {
        QFile configFileLoad(configFilePath_f().first);
        QByteArray jsonByteArray;
        if (configFileLoad.open(QIODevice::ReadOnly))
        {
            jsonByteArray = configFileLoad.readAll();
        }
        else
        {
            qtOutLine_f("Could not open config file " + configFilePath_f().first);
            break;
        }

        QJsonDocument jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
        if (jsonDocObj.isNull())
        {
            qtOutLine_f("Config file json is null" + configFilePath_f().first);
            break;
        }

        read_f(jsonDocObj.object());
//        loadChecksum_pri = QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5);
        configLoaded_pri = true;
//#ifdef DEBUGJOUVEN
        qtOutLine_f("App config loaded successfully");
//#endif
        break;
    }
//    if (not configFilePath_f().second)
//    {
//        qtOutRef_ext() << configFilePath_f().first;
//    }

    logDataHub_pri.setLogSaveDirectoryPath_f(logsDirectoryPath_pri);
    if (logDataHub_pri.isValidLogPathBaseName_f())
    {
        logDataHub_pri.setSaveLogFiles_f(true);
    }
    else
    {
//#ifdef DEBUGJOUVEN
        qtOutLine_f("Log custom path couldn't be set to: " + logsDirectoryPath_pri);
//#endif
    }
    if (logDataHub_pri.anyError_f())
    {
        qtOutLine_f(logDataHub_pri.getErrors_f().toRawReplace_f());
    }
    else
    {
        qtOutLine_f("Using default log path: " + logDataHub_pri.logSaveDirectoryPath_f());
    }

    logDataHub_pri.loadLogFiles_f(QString(), logFilter_c(), true, true);
    //load actonQtg translations
    tryLoadTranslations_f();
}


bool appConfig_c::saveConfigFile_f()
{
    bool configSavedTmp(false);

//    QString configFileStr;
//    if (configFilePath_f().second)
//    {
//        configFileStr = configFilePath_f().first;
//    }
//    else
//    {
//        configFileStr = fileTypePath_f(fileTypes_ec::config);
//    }

//    QJsonObject jsonObjectTmp;
//    write_f(jsonObjectTmp);
//    QJsonDocument jsonDocumentTmp(jsonObjectTmp);
//    QByteArray jsonByteArray(jsonDocumentTmp.toJson(QJsonDocument::Indented));

//    if (QCryptographicHash::hash(jsonByteArray, QCryptographicHash::Md5) not_eq loadChecksum_pri)
//    {
//        QFile configFileSaveTmp(configFileStr);
//        if (configFileSaveTmp.open(QIODevice::WriteOnly))
//        {
//            configFileSaveTmp.write(jsonByteArray);
//            configSavedTmp = true;
//        }
//    }

#ifdef DEBUGJOUVEN
    translator_pri.setAddNotFoundKeys_f(false);
    translator_pri.writeConfigJSONFile_f(fileTypePath_f(fileTypes_ec::translationConfig), true);
#endif

    return configSavedTmp;
}

bool appConfig_c::configLoaded_f() const
{
    return configLoaded_pri;
}


QString appConfig_c::translate_f(const QString& key_par_con)
{
    QString resultTmp;
    bool foundTmp(false);
    if (translator_pri.isConfigSet_f())
    {
        resultTmp = translator_pri.translate_f(key_par_con, std::addressof(foundTmp));
    }
    //this is already done when addNotFoundKeys in the translator_c object is true
    //it is done like this because otherwise a lot of texts/labels will be empty
    if (not foundTmp)
    {
        resultTmp = key_par_con;
    }
    return resultTmp;
}

QString appConfig_c::translateAndReplace_f(const text_c& text_par_con)
{
    QString resultTmp;
    bool foundTmp(false);
    if (translator_pri.isConfigSet_f())
    {
        resultTmp = translator_pri.translateAndReplace_f(text_par_con, std::addressof(foundTmp));
    }
    //this is already done when addNotFoundKeys in the translator_c object is true
    //it is done like this because otherwise a lot of texts/labels will be empty
    if (not foundTmp)
    {
        resultTmp = text_par_con.rawReplace_f();
    }
    return resultTmp;
}

QString appConfig_c::translateAndReplace_f(const textCompilation_c& textCompilation_par_con)
{
    QString resultTmp;
    if (translator_pri.isConfigSet_f())
    {
        resultTmp = translator_pri.translateAndReplaceToString_f(textCompilation_par_con);
    }
    else
    {
        resultTmp = textCompilation_par_con.toRawReplace_f();
    }
    return resultTmp;
}


bool appConfig_c::addLogMessage_f(
        const text_c& message_par_con
        , const logItem_c::type_ec logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con
)
{
    return logDataHub_pri.addMessage_f(message_par_con, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
}

std::vector<std::pair<const logItem_c* const, const QDateTime* const>> appConfig_c::getLogs_f(const logFilter_c& logFilter_par_con) const
{
    return logDataHub_pri.filter_f(logFilter_par_con);
}

appConfig_c* appConfig_ptr_ext = nullptr;
