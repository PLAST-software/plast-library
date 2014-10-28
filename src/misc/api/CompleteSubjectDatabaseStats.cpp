#include <designpattern/api/IProperty.hpp>
#include <designpattern/impl/ConfigFileProperties.hpp>

#include <misc/api/CompleteSubjectDatabaseStats.hpp>
#include <misc/api/PlastStrings.hpp>

#include <set>
#include <string>

/********************************************************************************/
namespace misc {
/********************************************************************************/

void CompleteSubjectDatabaseStats::readFromFile(const char* filepath)
{
    std::set<std::string> allowedKeys;
    allowedKeys.insert(STR_OPTION_COMPLETE_SUBJECT_DB_SIZE);
    allowedKeys.insert(STR_OPTION_COMPLETE_SUBJECT_DB_SEQUENCE_NUMBER);

    dp::impl::ConfigFileProperties completeSubjectDatabaseStats(allowedKeys, filepath);

    isFilled = true;

    dp::IProperty* prop = completeSubjectDatabaseStats.getProperty(STR_OPTION_COMPLETE_SUBJECT_DB_SIZE);
    if (prop == NULL)
    {
        isFilled = false;

        std::cerr << "The value for size was not found in " << filepath
            << ". Proceeding with default values for subject database statistics.";

        return;
    }
    size = misc::atoi(prop->value.c_str());

    prop = completeSubjectDatabaseStats.getProperty(STR_OPTION_COMPLETE_SUBJECT_DB_SEQUENCE_NUMBER);
    if (prop == NULL)
    {
        isFilled = false;

        std::cerr << "The value for number of sequences was not found in " << filepath
            << ". Proceeding with default values for subject database statistics.";

        return;
    }
    numberOfSequences = misc::atoi(prop->value.c_str());
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
