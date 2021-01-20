#ifndef PATHMONOTIFIER_EXECUTION_HPP
#define PATHMONOTIFIER_EXECUTION_HPP

#include "textQtso/text.hpp"
#include "pathConfig.hpp"

#include <QString>
#include <QMap>
#include <QObject>
#include <QProcess>

#include <vector>
#include <unordered_set>

class fileState_c
{
    QString filePath_pri;

    bool isDirectory_pri = false;
    bool firstCycle_pri = true;

    //current values
    //in bytes
    int_fast64_t currentFileSize_pri = 0;
    //epoch milliseconds
    int_fast64_t currentLastModificationDatetimeMs_pri = 0;
    //to check if two files are equal, only when the lastModificationDatetime has changed and the size is the same
    //probably xxhash
    uint_fast64_t currentHash_pri = 0;
    bool currentHashSet_pri = false;
    bool exists_pri = false;
    //bool sizeIncreasing_pri = false;
    //bool sizeDecreasing_pri = false;

    //old values
    int_fast64_t oldFileSize_pri = 0;
    int_fast64_t oldLastModificationDatetimeMs_pri = 0;
    uint_fast64_t oldHash_pri = 0;
    bool oldHashSet_pri = false;
    bool oldExists_pri = false;
    //bool oldSizeIncreasing_pri = false;
    //bool oldSizeDecreasing_pri = false;

    bool requiresHash_pri = false;

    //because JSON de/serialization "will" happen
    //a default ctor will be required
    bool initialized_pri = false;

    //copyCurrentToOld_f initializes it
    //changed/decreased/deleted/new functions will return false always if it hasn't been
    bool oldIinitalized_pri = false;

    bool wasANewFile_pri = false;

    int_fast64_t cyclesWithoutSizeIncreasing = 0;
    int_fast64_t cyclesWithoutSizeDecreasing = 0;

    void updateCurrentValues_f();
    void copyCurrentToOld_f();
public:
    fileState_c() = default;
    fileState_c(
            const QString& filePath_par_con
            , const bool firstCycle_par_con
            , const bool requiresHash_par_con
    );

    void updateFileValues_f();

    int_fast64_t currentFileSize_f() const;
    int_fast64_t currentLastModificationDatetimeMs_f() const;
    uint_fast64_t currentHash_f() const;
    bool exists_f() const;
    uint_fast64_t oldFileSize_f() const;
    int_fast64_t oldLastModificationDatetimeMs_f() const;
    uint_fast64_t oldHash_f() const;
    bool oldExists_f() const;
    QString filePath_f() const;

    //updateFileValues_f (which calls copyCurrentToOld_f) must be called once
    //otherwise all this fucntions bellow will return false
    bool sizeChanged_f() const;
    bool sizeIncreased_f() const;
    bool sizeDecreased_f() const;
    bool lastModificationDatetimeChanged_f() const;
    bool isNew_f() const;
    bool isDeleted_f() const;
    bool contentChanged_f() const;

    //this is done by checking how many cycles has the file been without increasing size
    bool sizeWasIncreasingAndStopped_f(const int_fast64_t cycleThreshold_par_con = 1) const;
    //this is wasANewFile_pri + sizeWasIncreasingAndStopped_f
    bool newFileFinishedLoading_f(const int_fast64_t cycleThreshold_par_con = 1) const;

    //this is done by checking how many cycles has the file been without decreasing
    //I don't think this can't have many uses, most of file operations don't cause a file size decrease
    //except a user editing a text file
    bool sizeWasDecreasingAndStopped_f(const int_fast64_t cycleThreshold_par_con = 1) const;

    //it's not possible? to monitor when a file is "moved", since OS file moves are a copy + delete at the end,
    //the source won't shrink or show any signs of being moved
    //bool possibleFileMove_f() const;
};

class directoryFilter_c;

class pathMonitifierExecution_c : public QObject
{
    Q_OBJECT

    pathConfig_c pathConfigToMonitor_pri;

    int_fast64_t lastMonitoringStart_pri = 0;
    QMap<QString, fileState_c> monitoredFilesMap_pri;
    std::vector<QString> monitoredFiles_pri;
    std::vector<QString> monitoredFilesToRemove_pri;
    directoryFilter_c* directoryFilter_pri = nullptr;
    bool firstCycle_pri = true;

    bool hashRequired_pri = false;

    bool stopMonitoring_pri = false;

    //used to shorten the path when notifying
    QString absolutePathToMonitorForNotifications_pri;
    QString pathToMonitorForNotifications_pri;

    std::atomic_int_fast64_t processCounter_pri = 0;
    std::atomic_int_fast64_t processCap_pri = 4;

    std::unordered_set<QString> finishedLoading_pri;

    std::vector<pathConfig_c::changeToMonitor_ec> anyChangeToReact_f(const fileState_c& fileStateObj_par_con);
    void monitoringScheduler_f();
    void launchMonitoringGatherFilesThread_f();
    void executeReaction_f(const QString& monitoredFile_par_con);
    void notify_f(const fileState_c& fileState_par_con, const pathConfig_c::changeToMonitor_ec change_par_con);
    void startProcess_f(QProcess* processPtr_par, const QString& argument_par_con);
    void decreaseProcessCounter_f();
public:
    explicit pathMonitifierExecution_c(QObject* parent_par, const pathConfig_c& pathConfigToMonitor_par_con);

Q_SIGNALS:
    void startMonitoring_signal();
    void monitoringFinishedGatheringFiles_signal();
    void monitoringStopped_signal();

private Q_SLOTS:
    void monitoringGatherFiles_f();
    void monitoringReact_f();
    void monitoringCycleEnd_f();
    //for the QProcess/s, mostly for debug
    void readError_f(QProcess::ProcessError error_par);
    void readStderr_f();
    void readStdout_f();
    void readProcessState_f(QProcess::ProcessState newState_par);
public Q_SLOTS:
    void executeMonitoring_f();
    void stopMonitoring_f();
};

#endif // PATHMONOTIFIER_EXECUTION_HPP
