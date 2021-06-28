#include "appConfig.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QCoreApplication>

#ifdef DEBUGJOUVEN
#ifndef Q_OS_WIN
//this is to get pretty stacktrace when the execution crashes
//instructions:
//1 this only applies to program projects, libs don't need this (libs need debug, -gX flags when compiling)
//2 link to -ldw or the elftutils library
//3 set the DEFINES in the .pro BACKWARD_HAS_UNWIND BACKWARD_HAS_DW (check backward.hpp source for more info about the macros)
//more info https://github.com/bombela/backward-cpp
#include "backward-cpp/backward.hpp"
namespace {
backward::SignalHandling sh;
}
#endif
#endif

int main(int argc, char *argv[])
{
    MACRO_signalHandler

    QCoreApplication qAppTmp(argc, argv);

    appConfig_c appConfigTmp(nullptr);

    auto returnValue(qAppTmp.exec());

    //stdout a new line before quitting otherwise shell line will be on top of the last printed line and it might look weird-wrong
    qtStdout_f() << Qt::endl;
    return returnValue;
}

