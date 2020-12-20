#include "pathConfig.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/essentialQt.hpp"
#include "essentialQtso/macros.hpp"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QString pathConfig_c::path_f() const
{
    return path_pri;
}

int_fast64_t pathConfig_c::monitorIntervalMilliseconds_f() const
{
    return monitorIntervalMilliseconds_pri;
}

std::unordered_set<pathConfig_c::changeToMonitor_ec> pathConfig_c::changesToMonitor_f() const
{
    return changesToMonitor_pri;
}

std::unordered_set<pathConfig_c::extraNotificationType_ec> pathConfig_c::extraNotificationTypes_f() const
{
    return extraNotificationTypes_pri;
}

int_fast64_t pathConfig_c::sizeIncreaseCycleThreshold_f() const
{
    return sizeIncreaseCycleThreshold_pri;
}

int_fast64_t pathConfig_c::sizeDecreaseCycleThreshold_f() const
{
    return sizeDecreaseCycleThreshold_pri;
}

QString pathConfig_c::runProcessPath_f() const
{
    return runProcessPath_pri;
}

QString pathConfig_c::dateTimeFormat_f() const
{
    return dateTimeFormat_pri;
}

bool pathConfig_c::UTC_f() const
{
    return UTC_pri;
}

bool pathConfig_c::threadFileSystemScan_f() const
{
    return threadFileSystemScan_pri;
}

bool pathConfig_c::passNotificationTextAsArgument_f() const
{
    return passNotificationTextAsArgument_pri;
}

QString pathConfig_c::separator_f() const
{
    return separator_pri;
}

pathConfig_c::pathConfig_c(
        const QString& path_par_con
        , const int_fast64_t monitorIntervalMilliseconds_par_con
        , const std::unordered_set<changeToMonitor_ec> changesToMonitor_par_con
        , const std::unordered_set<extraNotificationType_ec> notificationTypes_par_con
        , const int_fast64_t sizeIncreaseCycleThreshold_par_con
        , const int_fast64_t sizeDecreaseCycleThreshold_par_con
        , const QString& runProcessPath_par_con
        , const bool passNotificationTextAsArgument_par_con
        , const QString& dateTimeFormat_par_con
        , const bool UTC_par_con
        , const bool threadFileSystemScan_par_con
        , const QString separator_par_con)
    : path_pri(path_par_con)
    , monitorIntervalMilliseconds_pri(monitorIntervalMilliseconds_par_con)
    , changesToMonitor_pri(changesToMonitor_par_con)
    , extraNotificationTypes_pri(notificationTypes_par_con)
    , sizeIncreaseCycleThreshold_pri(sizeIncreaseCycleThreshold_par_con)
    , sizeDecreaseCycleThreshold_pri(sizeDecreaseCycleThreshold_par_con)
    , runProcessPath_pri(runProcessPath_par_con)
    , passNotificationTextAsArgument_pri(passNotificationTextAsArgument_par_con)
    , dateTimeFormat_pri(dateTimeFormat_par_con)
    , UTC_pri(UTC_par_con)
    , threadFileSystemScan_pri(threadFileSystemScan_par_con)
    , separator_pri(separator_par_con)
{}

const QMap<QString, pathConfig_c::changeToMonitor_ec> pathConfig_c::strToChangeToMonitorMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    {	"newfile", pathConfig_c::changeToMonitor_ec::newFile}
    , {	"deletedmovedrenamedfile", pathConfig_c::changeToMonitor_ec::deletedMovedRenamedFile}
    , {	"modificationdatetimechange", pathConfig_c::changeToMonitor_ec::modificationDateTimeChange}
    , {	"filesizechange", pathConfig_c::changeToMonitor_ec::fileSizeChange}
    , {	"hashchange", pathConfig_c::changeToMonitor_ec::hashChange}
    , {	"newfilefinishedloading", pathConfig_c::changeToMonitor_ec::newFileFinishedLoading}
    , {	"filefinishedloading", pathConfig_c::changeToMonitor_ec::fileFinishedLoading}
    //, {	"fileshrinkingdeleted", pathConfig_c::changeToMonitor_ec::fileShrinkingDeleted}
});

const std::unordered_map<pathConfig_c::changeToMonitor_ec, QString> pathConfig_c::changeToMonitorToStrUMap_sta_con(
{
    {	pathConfig_c::changeToMonitor_ec::newFile, "newFile" }
    , {	pathConfig_c::changeToMonitor_ec::deletedMovedRenamedFile, "deletedMovedRenamedFile" }
    , {	pathConfig_c::changeToMonitor_ec::modificationDateTimeChange, "modificationDateTimeChange" }
    , {	pathConfig_c::changeToMonitor_ec::fileSizeChange, "fileSizeChange" }
    , {	pathConfig_c::changeToMonitor_ec::hashChange, "hashChange" }
    , {	pathConfig_c::changeToMonitor_ec::newFileFinishedLoading, "newFileFinishedLoading" }
    , {	pathConfig_c::changeToMonitor_ec::fileFinishedLoading, "fileFinishedLoading" }
    //, {	pathConfig_c::changeToMonitor_ec::fileShrinkingDeleted, "fileShrinkingDeleted" }
});

const QMap<QString, pathConfig_c::extraNotificationType_ec> pathConfig_c::strToNotificationTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    //{	"text", pathConfig_c::extraNotificationType_ec::text}
     {	"bell", pathConfig_c::extraNotificationType_ec::bell}
    , {	"runprocess", pathConfig_c::extraNotificationType_ec::runProcess}
    //, {	"texttospeech", pathConfig_c::extraNotificationType_ec::textToSpeech}
});

const std::unordered_map<pathConfig_c::extraNotificationType_ec, QString> pathConfig_c::notificationTypeToStrUMap_sta_con(
{
    //{	pathConfig_c::extraNotificationType_ec::text, "text" }
     {	pathConfig_c::extraNotificationType_ec::bell, "bell" }
    , {	pathConfig_c::extraNotificationType_ec::runProcess, "runProcess" }
    //, {	pathConfig_c::extraNotificationType_ec::textToSpeech, "textToSpeech" }
});

void pathConfig_c::read_f(const QJsonObject& json_par_con)
{
    path_pri = json_par_con["path"].toString();
    monitorIntervalMilliseconds_pri = json_par_con["monitorIntervalMilliseconds"].toString(QString::number(monitorIntervalMilliseconds_pri)).toLongLong();
    sizeIncreaseCycleThreshold_pri = json_par_con["sizeIncreaseCycleThreshold"].toString(QString::number(sizeIncreaseCycleThreshold_pri)).toLongLong();
    sizeDecreaseCycleThreshold_pri = json_par_con["sizeDecreaseCycleThreshold"].toString(QString::number(sizeDecreaseCycleThreshold_pri)).toLongLong();

    QJsonArray changesToMonitorArrayTmp(json_par_con["changesToMonitor"].toArray());
    for (const auto item_ite_con : changesToMonitorArrayTmp)
    {
        pathConfig_c::changeToMonitor_ec changeToMonitorTmp(pathConfig_c::strToChangeToMonitorMap_sta_con.value(item_ite_con.toString().toLower(), changeToMonitor_ec::empty));
        changesToMonitor_pri.emplace(changeToMonitorTmp);
    }

    QJsonArray notificationsArrayTmp(json_par_con["extraNotificationTypes"].toArray());
    for (const auto item_ite_con : notificationsArrayTmp)
    {
        pathConfig_c::extraNotificationType_ec notificationTypeTmp(pathConfig_c::strToNotificationTypeMap_sta_con.value(item_ite_con.toString().toLower(), extraNotificationType_ec::empty));
        extraNotificationTypes_pri.emplace(notificationTypeTmp);
    }

    changesToMonitor_pri.erase(changeToMonitor_ec::empty);
    extraNotificationTypes_pri.erase(extraNotificationType_ec::empty);

    runProcessPath_pri = json_par_con["runProcessPath"].toString();
    dateTimeFormat_pri = json_par_con["dateTimeFormat"].toString(dateTimeFormat_pri);
    UTC_pri = json_par_con["UTC"].toBool(UTC_pri);
    threadFileSystemScan_pri = json_par_con["threadFileSystemScan"].toBool(threadFileSystemScan_pri);
    separator_pri = json_par_con["separator"].toString(separator_pri);
}

void pathConfig_c::writeJSONDocumented_f(QJsonObject& json_par)
{
    json_par["path"] = "a directory path, absolute or relative, to monitor";
    json_par["monitorIntervalMilliseconds"] = "interval in milliseconds, to check for filesystem changes in the path, this value is a number but the field is a string because json number limitations" ", default is \"" + QString::number(monitorIntervalMilliseconds_pri) + "\"";
    json_par["sizeIncreaseCycleThreshold"] = "how many cycles of monitorIntervalMilliseconds without a file size increasing to assume a file size stopped increasing, the reverse is used to assume it's increasing" ", default is \"" + QString::number(sizeIncreaseCycleThreshold_pri) + "\"";
    json_par["sizeDecreaseCycleThreshold"] = "how many cycles of monitorIntervalMilliseconds without a file size decreasing to assume a file size stopped decreasing, the reverse is used to assume it's decreasing" ", default is \"" + QString::number(sizeDecreaseCycleThreshold_pri) + "\"";

    json_par["changesToMonitorDescription"] = "(this is not an actual configuration field) must be one or more objects";
    //changesToMonitor
    QJsonArray changesToMonitorArrayTmp;
    changesToMonitorArrayTmp.append(QJsonValue("newFile, when a file that wasn't there in the previous monitor interval shows up in the next send a notification, this can be because a new file has been created or a file was moved/copied to the path"));
    changesToMonitorArrayTmp.append(QJsonValue("deletedMovedRenamedFile, when a file that was there in the previous monitor interval doesn't show up in the next send a notification, this can because the file was deleted, renamed or it was moved from the path to another path"));
    changesToMonitorArrayTmp.append(QJsonValue("fileSizeChange, when the size of a file changes between two intervals send a notification"));
    changesToMonitorArrayTmp.append(QJsonValue("hashChange, this implies file size change first and if it's equal check if the hash has changed from a previous interval to the next one send a notification"));
    changesToMonitorArrayTmp.append(QJsonValue("newFileFinishedLoading, this implies \"newFile\" and going over sizeIncreaseCycleThreshold, going x cycles without size increasing, when that happens send a notification"));
    changesToMonitorArrayTmp.append(QJsonValue("fleFinishedLoading, same as above without requiring \"newFile\""));
    //changesToMonitorArrayTmp.append(QJsonValue("fileShrinkingDeleted, when a file starts shrinking between intervals and in the end gets deleted send a notification"));

    json_par["changesToMonitor"] = changesToMonitorArrayTmp;

    json_par["extraNotificationTypesDescription"] = "(this is not an actual configuration field) must be one or more objects";

    QJsonArray notificationTypesArrayTmp;
    notificationTypesArrayTmp.append(QJsonValue("bell, aka \\a throught stdout, test it on your terminal using echo -e '\\a'"));
    notificationTypesArrayTmp.append(QJsonValue("runProccess, run a process"));
    json_par["extraNotificationTypes"] = notificationTypesArrayTmp;

    json_par["runProcessPath"] = "processPath, no arguments (use a a proxy script for that)";
    json_par["passNotificationTextAsArgument"] = "pass the notification text as a single argument to the process" ", default is \"" + QSTRINGBOOL(passNotificationTextAsArgument_pri) + "\"";
    json_par["dateTimeFormat"] = "see QDatetime::toString documentation" ", default is \"" + dateTimeFormat_pri + "\"";
    json_par["UTC"] = "true: show time in UTC, false: show in local" ", default is \"" + QSTRINGBOOL(UTC_pri) + "\"";
    json_par["threadFileSystemScan"] = "true: make an extra thread when scanning the filesystem, one extra per each pathConfig object being monitored, false: use the main thread" ", default is \"" + QSTRINGBOOL(threadFileSystemScan_pri) + "\"";
    json_par["separator"] = QString("separator used between the \"notification\" fields" ", default is \"" + separator_pri + "\"");
}

bool pathConfig_c::isValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(true);

    textCompilation_c errorTexts;

    //check path errors
    while (true)
    {
        if (path_pri.isEmpty())
        {
            errorTexts.append_f({"Empty path"});
            break;
        }

        QFileInfo pathFileInfo(path_pri);
        if (not pathFileInfo.exists())
        {
            errorTexts.append_f({"Path \"{0}\" doesn't exist", path_pri});
            break;
        }

        if (not pathFileInfo.isDir())
        {
            errorTexts.append_f({"Path \"{0}\" isn't a directory", path_pri});
            break;
        }

        break;
    }

    //check interval and timeout errors
    while (true)
    {
        if (monitorIntervalMilliseconds_pri < 0)
        {
            errorTexts.append_f({"monitorIntervalMilliseconds {0} must be => 0", monitorIntervalMilliseconds_pri});
            break;
        }

        if (sizeIncreaseCycleThreshold_pri < 0)
        {
            errorTexts.append_f({"sizeIncreaseCycleThreshold {0} must be => 0", sizeIncreaseCycleThreshold_pri});
            break;
        }

        if (sizeDecreaseCycleThreshold_pri < 0)
        {
            errorTexts.append_f({"sizeDecreaseCycleThreshold_pri {0} must be => 0", sizeDecreaseCycleThreshold_pri});
            break;
        }
        break;
    }

    //check changes to monitor and notification type/s errors
    while (true)
    {
        if (changesToMonitor_pri.size() < 1)
        {
            errorTexts.append_f({"No changes to monitor"});
            break;
        }

        bool containsRunProcessExtraNotificationType(extraNotificationTypes_pri.count(extraNotificationType_ec::runProcess) > 0);

        if (containsRunProcessExtraNotificationType)
        {
            if (runProcessPath_pri.isEmpty())
            {
                errorTexts.append_f({"Empty run process path for path \"{0}\"", path_pri});
                break;
            }

            QFileInfo runProcessFileInfo(runProcessPath_pri);
            if (not runProcessFileInfo.exists())
            {
                errorTexts.append_f({"Run process path \"{0}\" doesn't exist", runProcessPath_pri});
                break;
            }

            if (not runProcessFileInfo.isFile())
            {
                errorTexts.append_f({"Run process path \"{0}\" is not a file", runProcessPath_pri});
                break;
            }

            if (not runProcessFileInfo.isExecutable())
            {
                errorTexts.append_f({"Run process path file \"{0}\" is not executable", runProcessPath_pri});
                break;
            }
        }
        break;
    }

    if (not errorTexts.empty_f())
    {
        resultTmp = false;
        if (errorsPtr_par not_eq nullptr)
        {
            *errorsPtr_par = errorTexts;
        }
    }

    return resultTmp;
}
