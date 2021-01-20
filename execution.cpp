#include "execution.hpp"
#include "appConfig.hpp"

#include "cryptoQtso/hashQt.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "filterDirectoryQtso/filterDirectory.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/essentialQt.hpp"
#include "comuso/practicalTemplates.hpp"
#include "sizeConversionso/byte.hpp"
#include "essentialQtso/macros.hpp"

#include <QFileInfo>
//#include <QCollator>
#include <QDateTime>
#include <QTimer>
//#include <QVector>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

int_fast64_t fileState_c::currentFileSize_f() const
{
    return currentFileSize_pri;
}

int_fast64_t fileState_c::currentLastModificationDatetimeMs_f() const
{
    return currentLastModificationDatetimeMs_pri;
}

uint_fast64_t fileState_c::currentHash_f() const
{
    return currentHash_pri;
}

bool fileState_c::exists_f() const
{
    return exists_pri;
}

uint_fast64_t fileState_c::oldFileSize_f() const
{
    return oldFileSize_pri;
}

int_fast64_t fileState_c::oldLastModificationDatetimeMs_f() const
{
    return oldLastModificationDatetimeMs_pri;
}

uint_fast64_t fileState_c::oldHash_f() const
{
    return oldHash_pri;
}

bool fileState_c::oldExists_f() const
{
    return oldExists_pri;
}

QString fileState_c::filePath_f() const
{
    return filePath_pri;
}

bool fileState_c::sizeChanged_f() const
{
    return oldIinitalized_pri and (currentFileSize_pri not_eq oldFileSize_pri);
}

bool fileState_c::sizeIncreased_f() const
{
    return oldIinitalized_pri and (oldFileSize_pri < currentFileSize_pri);
}

bool fileState_c::sizeDecreased_f() const
{
    return oldIinitalized_pri and (oldFileSize_pri > currentFileSize_pri);
}

bool fileState_c::lastModificationDatetimeChanged_f() const
{
    return oldIinitalized_pri and (oldLastModificationDatetimeMs_pri not_eq currentLastModificationDatetimeMs_pri);
}

bool fileState_c::isNew_f() const
{
    return not oldIinitalized_pri and exists_pri;
}

bool fileState_c::isDeleted_f() const
{
    return oldExists_pri and not exists_pri;
}

bool fileState_c::contentChanged_f() const
{
    bool contentChangedResultTmp(false);
    while (oldIinitalized_pri)
    {
        if (sizeChanged_f())
        {
            contentChangedResultTmp = true;
            break;
        }

        if (currentHashSet_pri)
        {
            contentChangedResultTmp = currentHash_pri not_eq oldHash_pri;
            break;
        }

        break;
    }
    return contentChangedResultTmp;
}

bool fileState_c::sizeWasIncreasingAndStopped_f(const int_fast64_t cycleThreshold_par_con) const
{
    return cyclesWithoutSizeIncreasing >= cycleThreshold_par_con and currentFileSize_pri > 0;
}

bool fileState_c::newFileFinishedLoading_f(const int_fast64_t cycleThreshold_par_con) const
{
    return wasANewFile_pri and sizeWasIncreasingAndStopped_f(cycleThreshold_par_con);
}

bool fileState_c::sizeWasDecreasingAndStopped_f(const int_fast64_t cycleThreshold_par_con) const
{
    return cyclesWithoutSizeDecreasing >= cycleThreshold_par_con;
}

void fileState_c::updateCurrentValues_f()
{
    QFileInfo fileTmp(filePath_pri);
    if (fileTmp.exists())
    {
        exists_pri = true;
        if (fileTmp.isFile())
        {
            currentFileSize_pri = fileTmp.size();
            currentLastModificationDatetimeMs_pri = fileTmp.lastModified().toMSecsSinceEpoch();
            if (requiresHash_pri)
            {
                hasher_c hasherTmp(
                            hasher_c::inputType_ec::file
                            , filePath_pri
                            , hasher_c::outputType_ec::unsignedXbitInteger
                            , hasher_c::hashType_ec::XXHASH64
                );
                hasherTmp.generateHash_f();
                currentHash_pri = hasherTmp.hash64BitNumberResult_f();
                currentHashSet_pri = true;
            }
        }
        else
        {
            //ignore folders
        }

        if (oldExists_f())
        {

        }
    }
    else
    {
        exists_pri = false;
    }
}

void fileState_c::copyCurrentToOld_f()
{
    if (exists_f())
    {
        if (not oldIinitalized_pri and not firstCycle_pri)
        {
            wasANewFile_pri = true;
        }

        //save some previous cycle "observed changes"
        if (oldIinitalized_pri)
        {
            if (sizeIncreased_f())
            {
                cyclesWithoutSizeIncreasing = 0;
            }
            else
            {
                cyclesWithoutSizeIncreasing = cyclesWithoutSizeIncreasing + 1;
            }

            if (sizeDecreased_f())
            {
                cyclesWithoutSizeDecreasing = 0;
            }
            else
            {
                cyclesWithoutSizeDecreasing = cyclesWithoutSizeDecreasing + 1;
            }
        }
    }

    oldHash_pri = currentHash_pri;
    oldHashSet_pri = currentHashSet_pri;
    oldExists_pri = exists_pri;
    oldFileSize_pri = currentFileSize_pri;
    oldLastModificationDatetimeMs_pri = currentLastModificationDatetimeMs_pri;
    oldIinitalized_pri = true;
}

fileState_c::fileState_c(
        const QString& filePath_par_con
        , const bool firstCycle_par_con
        , const bool requiresHash_par_con
        )
    : filePath_pri(filePath_par_con)
    , firstCycle_pri(firstCycle_par_con)
    , requiresHash_pri(requiresHash_par_con)
    , initialized_pri(true)

{
    updateCurrentValues_f();
}

void fileState_c::updateFileValues_f()
{
    copyCurrentToOld_f();
    updateCurrentValues_f();
#ifdef DEBUGJOUVEN
//    qDebug() << "filePath_pri " << filePath_pri << endl
//             << "currentFileSize_pri " << currentFileSize_pri << endl
//             << "exists_pri " << exists_pri << endl
//             << "oldExists_pri " << oldExists_pri << endl
//             << "oldIinitalized_pri " << oldIinitalized_pri << endl;
#endif
}

pathMonitifierExecution_c::pathMonitifierExecution_c(
        QObject* parent_par
        , const pathConfig_c& pathConfigToMonitor_par_con)
    : QObject(parent_par)
    , pathConfigToMonitor_pri(pathConfigToMonitor_par_con)
{}

std::vector<pathConfig_c::changeToMonitor_ec> pathMonitifierExecution_c::anyChangeToReact_f(const fileState_c& fileStateObj_par_con)
{
    std::vector<pathConfig_c::changeToMonitor_ec> resultChangesTmp;
    for (const pathConfig_c::changeToMonitor_ec changeToMonitor_ite_con : pathConfigToMonitor_pri.changesToMonitor_f())
    {
        if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::modificationDateTimeChange
            and fileStateObj_par_con.lastModificationDatetimeChanged_f())
        {
            resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::modificationDateTimeChange);
            continue;
        }

        //without previous state saving, first cycle can't be used to determine if a file was created/removed
        //the other ifs are fine because they check if there is a previous state
        if (not firstCycle_pri)
        {
            if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::newFile
                and fileStateObj_par_con.isNew_f())
            {
                resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::newFile);
#ifdef DEBUGJOUVEN
                //qDebug() << "fileStateObj_par_con.isNew_f() " << resultTmp << endl;
#endif
                continue;
            }

            if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::deletedMovedRenamedFile
                and fileStateObj_par_con.isDeleted_f())
            {
                resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::deletedMovedRenamedFile);
                continue;
            }

            if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::newFileFinishedLoading
                and fileStateObj_par_con.newFileFinishedLoading_f(pathConfigToMonitor_pri.sizeIncreaseCycleThreshold_f()))
            {
                if (finishedLoading_pri.count(fileStateObj_par_con.filePath_f()))
                {
                     //if the file is "loading" again allow to be notified again
                    if (fileStateObj_par_con.sizeChanged_f())
                    {
                        //remove it
                        finishedLoading_pri.erase(fileStateObj_par_con.filePath_f());
                    }
                }
                else
                {
                    resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::newFileFinishedLoading);
                    finishedLoading_pri.insert(fileStateObj_par_con.filePath_f());
                    continue;
                }
            }

            if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::fileFinishedLoading
                and fileStateObj_par_con.sizeWasIncreasingAndStopped_f(pathConfigToMonitor_pri.sizeIncreaseCycleThreshold_f()))
            {
                if (finishedLoading_pri.count(fileStateObj_par_con.filePath_f()))
                {
                    //if the file is "loading" again allow to be notified again
                    if (fileStateObj_par_con.sizeChanged_f())
                    {
                        //remove it
                        finishedLoading_pri.erase(fileStateObj_par_con.filePath_f());
                    }
                }
                else
                {
                    resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::fileFinishedLoading);
                    finishedLoading_pri.insert(fileStateObj_par_con.filePath_f());
                    continue;
                }
            }
        }
        else
        {
            //if it's the first cycle don't count existing files as finished loading
            if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::fileFinishedLoading)
            {
                finishedLoading_pri.insert(fileStateObj_par_con.filePath_f());
            }
        }

        if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::fileSizeChange
            and fileStateObj_par_con.sizeChanged_f())
        {
            resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::fileSizeChange);
            continue;
            //check types of size changes

//            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::any)
//            {
//                resultTmp = fileStateObj_par_con.sizeChanged_f();
//                continue;
//            }

//            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::anyDecrease)
//            {
//                resultTmp = fileStateObj_par_con.sizeDecreased_f();
//                continue;
//            }

//            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::anyIncrease)
//            {
//                resultTmp = fileStateObj_par_con.sizeIncreased_f();
//                continue;
//            }

//            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::fixedValue)
//            {
//                resultTmp = fileStateObj_par_con.currentFileSize_f() == folderChangeReactionActionPtr_pri->fileSizeChangeFixedValue_f();
//                continue;
//            }

//            if (folderChangeReactionActionPtr_pri->fileSizeChangeTypeToMonitor_f() == folderChangeReactionData_c::fileSizeChangeType_ec::threshold)
//            {
//                if (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() >= 0
//                    and fileStateObj_par_con.currentFileSize_f() > folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f())
//                {
//                    resultTmp = true;
//                    continue;
//                }

//                if (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() < 0
//                    and fileStateObj_par_con.currentFileSize_f() < (folderChangeReactionActionPtr_pri->fileSizeChangeThreshold_f() * -1))
//                {
//                    resultTmp = true;
//                    continue;
//                }
//            }
        }

        if (changeToMonitor_ite_con == pathConfig_c::changeToMonitor_ec::hashChange and fileStateObj_par_con.contentChanged_f())
        {
            resultChangesTmp.emplace_back(pathConfig_c::changeToMonitor_ec::hashChange);
            continue;
        }
    }
#ifdef DEBUGJOUVEN
    //qDebug() << "any change to react resultTmp " << resultTmp << endl;
#endif
    return resultChangesTmp;
}

void pathMonitifierExecution_c::executeReaction_f(
        const QString& monitoredFile_par_con)
{
    fileState_c fileStateTmp(monitoredFilesMap_pri.value(monitoredFile_par_con));
    std::vector<pathConfig_c::changeToMonitor_ec> changesTmp(anyChangeToReact_f(fileStateTmp));
    for (const auto change_ite_con : changesTmp)
    {
        notify_f(fileStateTmp, change_ite_con);
    }
}

void pathMonitifierExecution_c::notify_f(
        const fileState_c& fileState_par_con
        , const pathConfig_c::changeToMonitor_ec change_par_con)
{
    QString currentTimeStrTmp;
    if (pathConfigToMonitor_pri.UTC_f())
    {
        currentTimeStrTmp = QDateTime::currentDateTimeUtc().toString(pathConfigToMonitor_pri.dateTimeFormat_f());
    }
    else
    {
        currentTimeStrTmp = QDateTime::currentDateTime().toString(pathConfigToMonitor_pri.dateTimeFormat_f());
    }
    QString filenameTmp(fileState_par_con.filePath_f());
    filenameTmp.remove(0, absolutePathToMonitorForNotifications_pri.size());
    if (pathConfigToMonitor_pri.useAbsolutePathsInNotifications_f())
    {
        filenameTmp = pathToMonitorForNotifications_pri + filenameTmp;
    }

    QString notificationLineTmp(currentTimeStrTmp + pathConfigToMonitor_pri.notififactionFieldSeparator_f() + filenameTmp + pathConfigToMonitor_pri.notififactionFieldSeparator_f() + pathConfig_c::changeToMonitorToStrUMap_sta_con.at(change_par_con));
    if (change_par_con == pathConfig_c::changeToMonitor_ec::modificationDateTimeChange)
    {
        notificationLineTmp.append(pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "New: " + QDateTime::fromMSecsSinceEpoch(fileState_par_con.currentLastModificationDatetimeMs_f()).toString(pathConfigToMonitor_pri.dateTimeFormat_f())
                                   + pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "Old: " + QDateTime::fromMSecsSinceEpoch(fileState_par_con.oldLastModificationDatetimeMs_f()).toString(pathConfigToMonitor_pri.dateTimeFormat_f()));
    }
    if (change_par_con == pathConfig_c::changeToMonitor_ec::hashChange)
    {
        notificationLineTmp.append(pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "New: " + QString::number(fileState_par_con.currentHash_f())
                                   + pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "Old: " + QString::number(fileState_par_con.oldHash_f()));
    }
    if (change_par_con == pathConfig_c::changeToMonitor_ec::fileSizeChange)
    {
        notificationLineTmp.append(pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "New: " + QString::fromStdString(formatByteSizeValue_f(fileState_par_con.currentFileSize_f())).remove(' ')
                                   + pathConfigToMonitor_pri.notififactionFieldSeparator_f() + "Old: " + QString::fromStdString(formatByteSizeValue_f(fileState_par_con.oldFileSize_f())).remove(' '));
    }
    qtOutLine_f(notificationLineTmp);
    if (pathConfigToMonitor_pri.extraNotificationTypes_f().count(pathConfig_c::extraNotificationType_ec::bell) > 0)
    {
        qtStdout_f() << '\a';
    }
    if (pathConfigToMonitor_pri.extraNotificationTypes_f().count(pathConfig_c::extraNotificationType_ec::runProcess) > 0 and processCounter_pri <= processCap_pri)
    {
        QProcess* runProcessTmp(new QProcess(this));
        //process writing the stderr doesn't trigger this
        QObject::connect(runProcessTmp, &QProcess::errorOccurred, this, &pathMonitifierExecution_c::readError_f);
        //QObject::connect(&runProcess_pri, &QProcess::started, this, &runProcessActionExecution_c::setStarted_f);
        //QObject::connect(runProcess_pri, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &pathMonitifierExecution_c::setFinished_f);
        QObject::connect(runProcessTmp, &QProcess::readyReadStandardError, this, &pathMonitifierExecution_c::readStderr_f);
        QObject::connect(runProcessTmp, &QProcess::readyReadStandardOutput, this, &pathMonitifierExecution_c::readStdout_f);
        QObject::connect(runProcessTmp, &QProcess::stateChanged, this, &pathMonitifierExecution_c::readProcessState_f);
        QObject::connect(runProcessTmp, &QProcess::finished, this, &pathMonitifierExecution_c::decreaseProcessCounter_f );
        QObject::connect(runProcessTmp, &QProcess::finished, runProcessTmp, &QProcess::deleteLater);

        processCounter_pri = processCounter_pri + 1;
        startProcess_f(runProcessTmp, pathConfigToMonitor_pri.passNotificationTextAsArgument_f() ? notificationLineTmp : QString());
    }
}

void pathMonitifierExecution_c::decreaseProcessCounter_f()
{
    processCounter_pri = processCounter_pri - 1;
}

void pathMonitifierExecution_c::readError_f(QProcess::ProcessError error_par)
{
    QProcess* processPtrTmp(static_cast<QProcess*>(this->sender()));
    //requires test, maybe the switch case isn't needed
    //if nothing comes out of errorString, use the documentation descriptions
    //some of the errors aren't critical like timeout or the read/write ones

    //if the error is not given by the process being run
    //i.e. process file no existing
    bool calleeErrorTmp(false);
    QString errorStrTmp;
    switch (error_par)
    {
        case QProcess::FailedToStart:
        {
            errorStrTmp.append(processPtrTmp->errorString());
            calleeErrorTmp = true;
        }
        break;
        case QProcess::Crashed:
        {
            errorStrTmp.append(processPtrTmp->errorString());
        }
        break;
        case QProcess::UnknownError:
        {
            MACRO_ADDPATHMONOTIFIERLOG("QProcess::UnknownError", logItem_c::type_ec::warning);
            errorStrTmp.append(processPtrTmp->errorString());
        }
        break;
        default:
        {
            errorStrTmp.append("runProcess QProcess error switch case default");
            //theoretically it shouldn't enter here ever, default is QProcess::UnknownError
        }
    }

    MACRO_ADDPATHMONOTIFIERLOG("runProcess error: " + errorStrTmp, logItem_c::type_ec::warning);
    text_c errorTmp("runProcess callee error {0}", QSTRINGBOOL(calleeErrorTmp));
    MACRO_ADDPATHMONOTIFIERLOG(errorTmp, logItem_c::type_ec::warning);
}

void pathMonitifierExecution_c::readStderr_f()
{
    QProcess* processPtrTmp(static_cast<QProcess*>(this->sender()));
    QString stderrTmp("runProcess stderr: " + processPtrTmp->readAllStandardError());
    MACRO_ADDPATHMONOTIFIERLOG(stderrTmp, logItem_c::type_ec::warning);
}

void pathMonitifierExecution_c::readStdout_f()
{
    QProcess* processPtrTmp(static_cast<QProcess*>(this->sender()));
    QString stdoutTmp("runProcess stdout: " + processPtrTmp->readAllStandardOutput());
    MACRO_ADDPATHMONOTIFIERLOG(stdoutTmp, logItem_c::type_ec::info);
}

void pathMonitifierExecution_c::readProcessState_f(QProcess::ProcessState newState_par)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "readProcessState_f newState_par" << newState_par;
#endif
    QString stdoutTmp("readProcessState_f newState_par: " + QVariant::fromValue(newState_par).toString());
    MACRO_ADDPATHMONOTIFIERLOG(stdoutTmp, logItem_c::type_ec::info);
}

void pathMonitifierExecution_c::startProcess_f(QProcess* processPtr_par, const QString& argument_par_con)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "runProcessActionExecution_c::execute_f()";
#endif

#ifdef Q_OS_WIN
    //(WINDOWS) use a console to run a process, otherwise it's impossible to stop them nicely
    //and the only option is "hard" kill
    runProcess_pri.setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
    {
        args->flags |= CREATE_NEW_CONSOLE;
        args->inheritHandles = true;
        //args->startupInfo->hStdError = GetStdHandle(STD_ERROR_HANDLE);
        //args->startupInfo->hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        //args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
    });
#endif
    //TODO? issue an error on the "qt bug site" that even with a clear environment, no PATH set, it manages to run stuff in /usr/bin
    //IMPORTANT ignore above, the issue is that to find the process location path what is used is the pathmonotifier environment (not runProcess_pri),
    //qprocess::setProcessEnvironment set variables of the QProcess which won't help,
    //so pathmonotifier environment must be modified, use relative paths or use absolute path
    processPtr_par->start(pathConfigToMonitor_pri.runProcessPath_f(), {argument_par_con});
    {
        text_c logMessageTmp("Process {0} started", pathConfigToMonitor_pri.runProcessPath_f());
        MACRO_ADDPATHMONOTIFIERLOG(logMessageTmp, logItem_c::type_ec::info);
    }
}

void pathMonitifierExecution_c::monitoringReact_f()
{
    if (not stopMonitoring_pri)
    {
        //loop the files
        for (const QString& monitoredFileStr_ite_con : monitoredFiles_pri)
        {
            executeReaction_f(monitoredFileStr_ite_con);
        }
    }
    monitoringCycleEnd_f();
}

void pathMonitifierExecution_c::monitoringCycleEnd_f()
{
    //remove deleted files from monitoring
    for (const QString& itemToRemove_ite_con : monitoredFilesToRemove_pri)
    {
        monitoredFilesMap_pri.remove(itemToRemove_ite_con);
    }
    monitoredFilesToRemove_pri.clear();
    monitoringScheduler_f();
}

void pathMonitifierExecution_c::stopMonitoring_f()
{
    stopMonitoring_pri = true;
    if (directoryFilter_pri not_eq nullptr and directoryFilter_pri->currentState_f() == directoryFilter_c::state_ec::running)
    {
        directoryFilter_pri->stopFiltering_f();
    }
}

void pathMonitifierExecution_c::monitoringGatherFiles_f()
{
    if (directoryFilter_pri == nullptr)
    {
        filterOptions_s filterOptionsTmp;
        filterOptionsTmp.navigateSubdirectories_pub = true;
        filterOptionsTmp.useAbsolutePaths_pub = true;
        filterOptionsTmp.listFiles_pub = true;
        filterOptionsTmp.listHidden_pub = true;
        filterOptionsTmp.navigateHiddenDirectories_pub = true;
        filterOptionsTmp.listDirectories_pub = false;
        filterOptionsTmp.listEmptyDirectories_pub = false;

        directoryFilter_pri = new directoryFilter_c(this, pathConfigToMonitor_pri.path_f(), filterOptionsTmp);
    }
    std::vector<QString> fileListResultTmp(directoryFilter_pri->filter_f());
#ifdef DEBUGJOUVEN
    //qDebug() << "fileListResultTmp.size() " << fileListResultTmp.size() << endl;
#endif

    //update existing files details
    QMap<QString, fileState_c>::iterator iteTmp(monitoredFilesMap_pri.begin());
    while (iteTmp not_eq monitoredFilesMap_pri.end())
    {
        iteTmp.value().updateFileValues_f();
        if (not iteTmp.value().exists_f())
        {
            monitoredFilesToRemove_pri.emplace_back(iteTmp.key());
        }
        ++iteTmp;
    }

    //insert new files
    for (const QString& filePath_ite_con : fileListResultTmp)
    {
        if (monitoredFilesMap_pri.contains(filePath_ite_con))
        {
            //nothing
        }
        else
        {
            monitoredFilesMap_pri.insert(filePath_ite_con, fileState_c(filePath_ite_con, firstCycle_pri, hashRequired_pri));
        }
    }

    std::vector<QString> monitoredFilesTmp;
    monitoredFilesTmp.reserve(monitoredFilesMap_pri.size());
    for (auto ite = monitoredFilesMap_pri.keyBegin(); ite not_eq monitoredFilesMap_pri.keyEnd(); ++ite)
    {
        monitoredFilesTmp.emplace_back(*ite);
    }
//    if (not monitoredFiles_pri.empty())
//    {
//        //sort files by reactionOrder
//        QCollator collatorTmp;
//        collatorTmp.setNumericMode(true);

//        std::sort(monitoredFilesSortedTmp.begin(), monitoredFilesSortedTmp.end(),
//                  [this, &collatorTmp](const fileState_c& a_par_con, const fileState_c& b_par_con) -> bool
//        {
//            bool resultTmp(false);
//            while (true)
//            {
//                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::newestFirst)
//                {
//                    resultTmp = a_par_con.currentLastModificationDatetimeMs_f() > b_par_con.currentLastModificationDatetimeMs_f();
//                    break;
//                }

//                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::alphabetical)
//                {
//                    resultTmp = collatorTmp.compare(a_par_con.filePath_f(), b_par_con.filePath_f()) < 0;
//                    break;
//                }

//                if (folderChangeReactionActionPtr_pri->reactionOrder_f() == folderChangeReactionData_c::reactionOrder_ec::smallerFirst)
//                {
//                    resultTmp = a_par_con.currentFileSize_f() < b_par_con.currentFileSize_f();
//                    break;
//                }

//                break;
//            }
//            return resultTmp;
//        });
//    }

//    if (folderChangeReactionActionPtr_pri->reverseReactionOrder_f())
//    {
//        std::reverse(monitoredFilesSortedTmp.begin(), monitoredFilesSortedTmp.end());
//    }

    monitoredFiles_pri = monitoredFilesTmp;

    Q_EMIT monitoringFinishedGatheringFiles_signal();
}

void pathMonitifierExecution_c::launchMonitoringGatherFilesThread_f()
{
    if (pathConfigToMonitor_pri.threadFileSystemScan_f())
    {
        threadedFunction_c* threadedFunction_ptr(new threadedFunction_c(std::bind(&pathMonitifierExecution_c::monitoringGatherFiles_f, this), true));
        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &threadedFunction_c::deleteLater);
        QObject::connect(this, &pathMonitifierExecution_c::monitoringFinishedGatheringFiles_signal, threadedFunction_ptr, &threadedFunction_c::quit);
        threadedFunction_ptr->start();
    }
    else
    {
        monitoringGatherFiles_f();
    }
}

void pathMonitifierExecution_c::monitoringScheduler_f()
{
    if (stopMonitoring_pri)
    {
        stopMonitoring_pri = false;
        //to stop the monitoring cycle, directoryFilter_pri is asked to stop, then when it tries to react after
        //but the react function is skipped and since this is the end-start of the cycle it can end here
        Q_EMIT monitoringStopped_signal();
        return;
    }

    uint_fast64_t currentDateTimeTmp(QDateTime::currentMSecsSinceEpoch());
    if (lastMonitoringStart_pri == 0)
    {
        lastMonitoringStart_pri = currentDateTimeTmp;
        launchMonitoringGatherFilesThread_f();
    }
    else
    {
        firstCycle_pri = false;
        int_fast64_t timeDifferenceMsTmp(currentDateTimeTmp - lastMonitoringStart_pri);
        //if the monitor interval time is bigger than the time passed since the previous monitoring cycle start
        //wait and then come back to the monitoring scheduler
        if (pathConfigToMonitor_pri.monitorIntervalMilliseconds_f() > timeDifferenceMsTmp)
        {
            QTimer::singleShot(timeDifferenceMsTmp, this, &pathMonitifierExecution_c::monitoringScheduler_f);
        }
        else
        {
            lastMonitoringStart_pri = currentDateTimeTmp;
            launchMonitoringGatherFilesThread_f();
        }
    }
}

void pathMonitifierExecution_c::executeMonitoring_f()
{
    hashRequired_pri = pathConfigToMonitor_pri.changesToMonitor_f().count(pathConfig_c::changeToMonitor_ec::hashChange) == 1;

    QFileInfo pathFileInfoTmp(pathConfigToMonitor_pri.path_f());
    absolutePathToMonitorForNotifications_pri = pathFileInfoTmp.absoluteFilePath();
    if (pathConfigToMonitor_pri.useAbsolutePathsInNotifications_f())
    {
        pathToMonitorForNotifications_pri = pathConfigToMonitor_pri.path_f();
        if (pathToMonitorForNotifications_pri.endsWith('/'))
        {
            //nothing to do
        }
        else
        {
            pathToMonitorForNotifications_pri.append('/');
        }
    }

    QObject::connect(
                this
                , &pathMonitifierExecution_c::monitoringFinishedGatheringFiles_signal
                , this
                , &pathMonitifierExecution_c::monitoringReact_f);
    QTimer::singleShot(0, this, &pathMonitifierExecution_c::monitoringScheduler_f);
}

