#ifndef PATHMONOTIFIER_APPCONFIG_HPP
#define PATHMONOTIFIER_APPCONFIG_HPP

#include "pathConfig.hpp"

#include "programConfigQtso/programConfigConsole.hpp"

#include <QString>
#include <QObject>

#include <vector>

class appConfig_c : public programConfigConsole_c
{
    Q_OBJECT

    std::vector<pathConfig_c> pathConfigs_pri;

     //to control what's executing when quitting
    int_fast64_t pathConfigExecutionsFinished_pri = 0;
    int_fast64_t pathConfigExecutionsStarted_pri = 0;

    //does nothing except saving the translation placeholders in the debug builds
    void derivedReadJSON_f(const QJsonObject &json_par_con) override;
    void derivedWriteJSONDocumented_f(QJsonObject &json_par) const override;
    void derivedWriteJSON_f(QJsonObject &) const override
    {}
    bool firstPositionalArgumentCanBeConfig_f() const override
    {
        return false;
    }
    bool translationCreatePlacerholders_f() const override
    {
        return true;
    }
    bool loggingSaveLogFileRequired_f() const override
    {
        return true;
    }
    QString programName_f() const override
    {
        return "PathMonotifierQt";
    }
    text_c programDescription_f() const override
    {
        return "Monitor and notify file system changes of a path/s i.e. new files, file size change, modified datetime change... \nCreated by Jouven";
    }
    QString programVersion_f() const override
    {
        return "1.0";
    }

    bool addMessageTypeInMessage_f() const override
    {
        return false;
    }
    bool requiresJSONDocumented_f() const override
    {
        return true;
    }
    void derivedConfigureCommandLineParser_f(QCommandLineParser& parser_par) const override;
    void derivedCheckCommandLineParser_f(QCommandLineParser& parser_par) override;
    void derivedStart_f() override;
    void derivedQuit_f() override;
public:
    appConfig_c(QObject* parent_par);

Q_SIGNALS:
    void stopMonitoringPaths_signal();
private Q_SLOTS:
    void checkPathConfigFinishedExecutions_f();
public Q_SLOTS:
    void startMonitoringPaths_f();
};

//needs to be initialized
extern appConfig_c* appConfig_ptr_ext;

#endif // PATHMONOTIFIER_APPCONFIG_HPP
