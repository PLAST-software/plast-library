
#include <misc/api/PLogger.hpp>

#include <execinfo.h>
#include <stdio.h>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

/********************************************************************************/
namespace misc {
/********************************************************************************/
namespace PLogger {

namespace{
    log4cpp::Category& root = log4cpp::Category::getRoot();
    bool init = false;
}

void initialize(){
    log4cpp::Appender *appender = new log4cpp::FileAppender("default", "plast.log");
    log4cpp::PatternLayout *pattern = new log4cpp::PatternLayout();
    pattern->setConversionPattern("%d [%p] - %m%n %x");
    appender->setLayout(pattern);
    root.setPriority(log4cpp::Priority::DEBUG);
    root.addAppender(appender);

    init = true;
}

void debug(const char* message){
    if (!init)
        return;
    root.debug(message);
    void* callstack[2];
    int i, frames = backtrace(callstack, 2);
    char** strs = backtrace_symbols(callstack, frames);
    for (i = 0; i < frames; ++i) {
    printf("%s\n", strs[i]);
    }
    free(strs);
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
