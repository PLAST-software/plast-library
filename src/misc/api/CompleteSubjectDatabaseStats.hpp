/** \file CompleteSubjectDatabaseStats.hpp
 *  \brief Structure holding information for the complete subject database.
 *  \date 27/10/2014
 *  \author ipetrov
 */

#ifndef _COMPLETE_SUBJECT_DATABASE_STATS_HPP_
#define _COMPLETE_SUBJECT_DATABASE_STATS_HPP_

#include <misc/api/types.hpp>


/** \namespace misc
 * \brief Miscellanous definitions
 */

/********************************************************************************/
namespace misc {
/********************************************************************************/

/** \brief Define statistics of the complete subject database.
 *
 * This information is useful in case it has been given to more than one node
 * to process.
 */
class CompleteSubjectDatabaseStats {

public:

    CompleteSubjectDatabaseStats() : isFilled(false), size(-1), numberOfSequences(-1) {}

    bool isFilled;

    /** The size of the complete subject database */
    size_t size;

    /** The number of sequences in the complete subject database */
    int numberOfSequences;

    /*********************************************************************
    ** METHOD  : CompleteSubjectDatabaseStats::readFromFile
    ** PURPOSE : Reads data from a file and sets it appropriately.
    ** INPUT   : The filename of the file containting the initialization data.
    ** OUTPUT  : None.
    ** RETURN  : None.
    ** REMARKS : If the data is not enough for complete initialization, it discards
    **           all the data and prints an error message.
    *********************************************************************/
    void readFromFile(const char* filepath);
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _COMPLETE_SUBJECT_DATABASE_STATS_HPP_ */
