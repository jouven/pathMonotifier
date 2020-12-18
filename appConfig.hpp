#ifndef PATHMONOTIFIER_APPCONFIG_HPP
#define PATHMONOTIFIER_APPCONFIG_HPP

#include "pathConfig.hpp"
#include "translatorJSONQtso/translator.hpp"
#include "logsinJSONQtso/logDataHub.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QString>
#include <QObject>

#include <vector>

class QJsonObject;

//don't call this macro inside the appConfig_c class (like calling from the ctor and hanging the process... without errors...)
//inside use MACRO_ADDMESSAGE
#define MACRO_FILEMETA MACRO_FILENAME, __func__, __LINE__
#define MACRO_ADDPATHMONOTIFIERLOG(...) appConfig_ptr_ext->addLogMessage_f(__VA_ARGS__, MACRO_FILEMETA)
class appConfig_c : public QObject
{
    Q_OBJECT

    std::vector<pathConfig_c> pathConfigs_pri;

    bool configLoaded_pri = false;

    QString translationConfigFile_pri;
    bool translationConfigFileSet_pri = false;

    translator_c translator_pri;

    QString logsDirectoryPath_pri;
    bool logsDirectoryPathSet_pri = false;

    logDataHub_c logDataHub_pri;

    bool logging_pri = false;

    //to control what's executing when quitting
    int_fast64_t pathConfigExecutionsFinished_pri = 0;
    int_fast64_t pathConfigExecutionsStarted_pri = 0;

    void tryLoadTranslations_f();

    //does nothing except saving the translation placeholders in the debug builds
    void read_f(const QJsonObject &json_par_con);
    //void write_f(QJsonObject &json_par) const;

    void writeJSONDocumented_f(QJsonObject &json_par) const;
public:
    appConfig_c(QObject* parent_par);

    bool saveConfigFile_f();

    bool configLoaded_f() const;

    QString translate_f(const QString& key_par_con);
    QString translateAndReplace_f(const text_c& text_par_con);
    QString translateAndReplace_f(const textCompilation_c& textCompilation_par_con);
    //FUTURE allow to change language using translationFromToPairs_f (the "to" part)
    //Making a program like "translationParserAvidcalm" (which is another program made by me, that parses the source js/cpp files for translations)
    //but to generate skeleton json language files
    //(from language will always be hardcoded because the initial source-language is always harcoded)

    bool addLogMessage_f(
            const text_c& message_par_con
            , const logItem_c::type_ec logType_par_con
            , const QString& sourceFile_par_con
            , const QString& sourceFunction_par_con
            , const int_fast32_t line_par_con
    );

    std::vector<std::pair<const logItem_c* const, const QDateTime* const>> getLogs_f(const logFilter_c& logFilter_par_con = logFilter_c()) const;

    logDataHub_c* logDataHub_f();
Q_SIGNALS:
    void stopMonitoringPaths_signal();
    void quit_signal();
private Q_SLOTS:
    //first ctor line QObject::connect(this, &appConfig_c::quit_signal, this, &appConfig_c::quit_f, Qt::QueuedConnection);
    void quit_f();
    void checkPathConfigFinishedExecutions_f();
public Q_SLOTS:
    void startMonitoringPaths_f();
};

//needs to be initialized
extern appConfig_c* appConfig_ptr_ext;

#endif // PATHMONOTIFIER_APPCONFIG_HPP
