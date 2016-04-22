
#include <misc/api/PLogger.hpp>

#include <execinfo.h>
#include <stdio.h>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/NDC.hh>

/********************************************************************************/
namespace misc {
/********************************************************************************/
namespace PLogger {

namespace{
    bool init = false;
}

void initialize(log4cpp::Priority::Value priority){
    log4cpp::Category&     root = log4cpp::Category::getRoot();
    log4cpp::Appender      *appender = new log4cpp::FileAppender("default", "plast.log");
    log4cpp::PatternLayout *pattern = new log4cpp::PatternLayout();

    pattern->setConversionPattern("%d [%p]-%c- %m%n");
    appender->setLayout(pattern);

    root.setPriority(priority);
    root.addAppender(appender);

    init = true;
}

bool isInitialized(){
    return init;
}

/*void debug(log4cpp::Category& logger, const char* message){
    if (!init)
        return;
    logger.debug(message);
    void* callstack[2];
    int i, frames = backtrace(callstack, 2);
    char** strs = backtrace_symbols(callstack, frames);
    for (i = 0; i < frames; ++i) {
    printf("%s\n", strs[i]);
    }
    free(strs);
}*/

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
