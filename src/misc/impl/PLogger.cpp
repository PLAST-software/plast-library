
#include <misc/api/PLogger.hpp>

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>

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

log4cpp::Category& getLoggerByName(const char *loggerName){
    if (strcmp(loggerName, ROOT_LOGGER_NAME) == 0){
        return log4cpp::Category::getRoot();
    }
    else{
        return log4cpp::Category::getInstance(loggerName);
    }
}

/********************************************************************************/
}}  /* end of namespaces. */
/********************************************************************************/
