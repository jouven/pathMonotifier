#ifndef PATHMONOTIFIER_PATHCONFIG_HPP
#define PATHMONOTIFIER_PATHCONFIG_HPP

#include <QString>
#include <QMap>

#include <unordered_set>
#include <unordered_map>

class QJsonObject;
class textCompilation_c;

class pathConfig_c
{
    QString path_pri;
    int_fast64_t monitorIntervalMilliseconds_pri = 1000;

public:
    enum class changeToMonitor_ec
    {
        empty = 0
        , newFile = 1
        //EXPLAIN OR CHANGE: if a file is renamed or moved away it counts as "deleted",
        //renamed files could be monitored if hash is forced it could be detected when a new file with the same hash but a differente filename as an old file appears,
        //moved files... "can't be done" (maybe if it was full, all paths, monitoring... but that's not the idea of this program)
        , deletedMovedRenamedFile = 2
        , fileSizeChange = 3
        , modificationDateTimeChange = 4
        , hashChange = 5
        //to detect end of transfer situations like copy, move, detecting when a video recording ends
        , newFileFinishedLoading = 6
        //same as above but without requiring the file to be new
        , fileFinishedLoading = 7
        //as I mentioned in the fileState_c class file shrinking is not a thing, (AFAIK actonQtg is the only program with that kind of move mechanics)
        //the other side of the newFileStoppedGrowing, i.e., a file being transfered away
        //, fileShrinkingDeleted = 7
    };
    enum class extraNotificationType_ec
    {
        empty = 0
        , bell = 1
        //it's better to allow users run a process than try to configure qt to do certain things, like play a sound or textToSpeech,
        //when it's easier to call specialized programs like mpv or eSpeak
        , runProcess = 2
    };
private:
    std::unordered_set<changeToMonitor_ec> changesToMonitor_pri;
    std::unordered_set<extraNotificationType_ec> extraNotificationTypes_pri;

    //how many cycles of "monitorIntervalMilliseconds_pri" without a file increasing/decreasing before deciding it's not increasing,
    //the reverse is used to know-decide if a file increasing or shrinking
    //regular size change check doesn't use this (there is no file size is "continuously" changing feature yet)
    int_fast64_t sizeIncreaseCycleThreshold_pri = 1;
    int_fast64_t sizeDecreaseCycleThreshold_pri = 1;
    QString runProcessPath_pri;
    bool passNotificationTextAsArgument_pri = false;
    QString dateTimeFormat_pri = "yyyy-MM-dd hh:mm:ss.zzz";
    //UTC or local
    bool UTC_pri = false;
    //use only if path configs happen on different storage devices and multiple filesystem scan can happen at the same time, also mind the overall storage bus
    //bandwith because >1 SSD disks (5xxMB/s) doing transfers at the same time can easily cap the SATA3.x (6GBit/s) bus.
    //using it on >1 path configs happening on the same device will destroy that device performance while the scans happen and these scans will be slower
    //also factor in if hash change is required, because it forces full file readings to generate hash/s
    bool threadFileSystemScan_pri = false;
    QString separator_pri = " | ";
public:
    static const QMap<QString, changeToMonitor_ec> strToChangeToMonitorMap_sta_con;
    static const std::unordered_map<changeToMonitor_ec, QString> changeToMonitorToStrUMap_sta_con;

    static const QMap<QString, extraNotificationType_ec> strToNotificationTypeMap_sta_con;
    static const std::unordered_map<extraNotificationType_ec, QString> notificationTypeToStrUMap_sta_con;

    pathConfig_c() = default;
    pathConfig_c(
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
            , const QString separator_par_con);

    void read_f(const QJsonObject &json_par_con);
    //void write_f(QJsonObject &json_par) const;
    void writeJSONDocumented_f(QJsonObject& json_par);
    //check if this pathConfig_c object is usable to start monitoring, e.g., path exists, there is at least one change to monitor, there is at least one notification type and so on
    //pass errorsPtr_par argument to get the error texts
    bool isValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    QString path_f() const;
    int_fast64_t monitorIntervalMilliseconds_f() const;
    std::unordered_set<changeToMonitor_ec> changesToMonitor_f() const;
    std::unordered_set<extraNotificationType_ec> extraNotificationTypes_f() const;
    int_fast64_t sizeIncreaseCycleThreshold_f() const;
    int_fast64_t sizeDecreaseCycleThreshold_f() const;
    QString runProcessPath_f() const;
    QString dateTimeFormat_f() const;

    bool UTC_f() const;
    bool threadFileSystemScan_f() const;
    bool passNotificationTextAsArgument_f() const;
    QString separator_f() const;
};

#endif // PATHMONOTIFIER_PATHCONFIG_HPP
