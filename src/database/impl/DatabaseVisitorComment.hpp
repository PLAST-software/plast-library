/*   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released July  2011                                        *
 *   Copyright (c) 2011                                                      *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the CECILL version 2 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   CECILL version 2 License for more details.                              *
 *****************************************************************************/

/** \file DatabaseVisitorComment.hpp
 *  \brief Definition of a database sequence visitor
 *  \date 03/06/2014
 *  \author sbrillet

 */

#ifndef _DATABASE_VISITOR_HPP_
#define _DATABASE_VISITOR_HPP_

#include <database/impl/BufferedSequenceDatabase.hpp>
#include <database/impl/CachedSubDatabase.hpp>

/********************************************************************************/
/** \brief Definition of concepts related to genomic databases. */
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
class DatabaseVisitorComment : public DatabaseVisitor
{
public:
    DatabaseVisitorComment(const ISequence& seq, std::string &str) :_result(str),_sequence(seq)
    {
    }

    void visitBufferedSequenceDatabase  (BufferedSequenceDatabase& db) {
        _result = db._refIterator->transformComment(_sequence.comment);
    }
    void visitCompositeSequenceDatabase (CompositeSequenceDatabase& db) {
        _result="CompositeSequenceDatabase";}

    void visitCachedSubDatabase (CachedSubDatabase& db) {
        _result = db.transformComment(_sequence);
    }

protected:
    std::string &_result;
    const ISequence& _sequence;

};
}
}
#endif /*  _DATABASE_VISITOR_HPP_ */
