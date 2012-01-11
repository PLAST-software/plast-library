/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
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

/** \file IAlignmentResult.hpp
 *  \brief Concepts about alignments with gaps.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _IALIGNMENT_RESULT_HPP_
#define _IALIGNMENT_RESULT_HPP_

/********************************************************************************/


#include <designpattern/api/SmartPointer.hpp>
#include <designpattern/api/Iterator.hpp>

#include <misc/api/macros.hpp>

#include <index/api/IOccurrenceIterator.hpp>

#include <list>
#include <map>
#include <iostream>
#include <sstream>
#include <set>
#include <string>
#include <stdio.h>


/********************************************************************************/
namespace algo   {
/** \brief Concepts about alignments with gaps. */
namespace align {
/********************************************************************************/

/** \brief Definition of an alignment between two databases
 *
 * The Alignment class represents what the end user wants, ie. knowing where
 * similarities have been found in two genomic databases. So, the whole PLAST
 * algorithm purpose is to generate instances of Alignment.
 *
 * An alignment is defined by several attributes:
 *      - index of the matching sequence in the subject database
 *      - index of the matching sequence in the query database
 *      - [begin,end] offsets of the alignment in the subject sequence
 *      - [begin,end] offsets of the alignment in the query sequence
 *      - [begin,end] offsets of the alignment in the subject database
 *      - [begin,end] offsets of the alignment in the query database
 *      - length of the alignment
 *      - expected value of the alignment
 *      - bit score of the alignment
 *      - number of gaps in the alignment
 *      - number of identical residues between the two databases
 *      - number of misses between the two databases
 *
 * Note that we have here redundant information; for instance, the offsets in the sequence may
 * be retrieved from the offsets in the database. The redundancy is here only to avoid some
 * re-computations.
 *
 * Alignment instances should be retrieved through the IAlignmentResult interface.
 *
 * \see IAlignmentResult
 */
class Alignment
{
public:

    /** Default constructor.
     */
    Alignment ()  :  _evalue(0), _bitscore(0), _nbGap(0), _identity(0), _nbMis(0) {}

    /** Constructor.
     */
    Alignment (
        const indexation::ISeedOccurrence* occur1,
        const indexation::ISeedOccurrence* occur2,
        int leftOffsetInQuery,
        int leftOffsetInSubject,
        int rightOffsetInQuery,
        int rightOffsetInSubject,
        int score
    )  :   _occur1(occur1), _occur2(occur2),  _evalue(0), _bitscore(0), _nbGap(0), _identity(0), _nbMis(0)
    {
        configure (occur1, occur2, leftOffsetInQuery, leftOffsetInSubject, rightOffsetInQuery, rightOffsetInSubject);
    }

    /** Define some way to compare two alignments. It returns the overlap ratio of the two provided alignments.
     * \param[in] a1 : first alignment
     * \param[in] a2 : second alignment
     * \return overlap value; it is a ratio, so it lies in [0;1]
     */
    static double overlap (const Alignment& a1, const Alignment& a2)
    {
        double o1 = 0.0;
        double o2 = 0.0;

        /** We define here a little macro for easing the overlap impl. */
        #define VAL(type,subqry,bound)   type(a1._##subqry##bound##InSeq, a2._##subqry##bound##InSeq)

        if (VAL(MIN,query,End) > VAL(MAX,query,Start))
        {
            o1 = (double) (VAL(MIN,query,End) - VAL(MAX,query,Start) + 1) /
                 (double) (VAL(MAX,query,End) - VAL(MIN,query,Start) + 1);
        }
        else
        {
            o1 = (double) (VAL(MIN,query,Start) - VAL(MAX,query,End) + 1) /
                 (double) (VAL(MAX,query,Start) - VAL(MIN,query,End) + 1);
        }

        if (VAL(MIN,subject,End) > VAL(MAX,subject,Start))
        {
            o2 = (double) (VAL(MIN,subject,End) - VAL(MAX,subject,Start) + 1) /
                 (double) (VAL(MAX,subject,End) - VAL(MIN,subject,Start) + 1);
        }
        else
        {
            o2 = (double) (VAL(MIN,subject,Start) - VAL(MAX,subject,End) + 1) /
                 (double) (VAL(MAX,subject,Start) - VAL(MIN,subject,End) + 1);
        }

        return MAX (0, MIN (o1, o2));
    }

    /** Reference on the seed occurrence in the subject sequence (used for accessing the sequence data). */
    const indexation::ISeedOccurrence* _occur1;

    /** Reference on the seed occurrence in the query sequence (used for accessing the sequence data). */
    const indexation::ISeedOccurrence* _occur2;

    /** Subject database. */
    database::ISequenceDatabase* _subjectDb;

    /** Query database. */
    database::ISequenceDatabase* _queryDb;

    /** Index of the sequence in the subject database. */
    u_int32_t _subjectIdx;

    /** Index of the sequence in the query database. */
    u_int32_t _queryIdx;

    /** Start offset in the subject sequence. */
    int32_t _subjectStartInSeq;
    /** Start offset in the query sequence. */
    int32_t _queryStartInSeq;
    /** End offset in the subject sequence. */
    int32_t _subjectEndInSeq;
    /** End offset in the query sequence. */
    int32_t _queryEndInSeq;

    /** Start offset in the subject database. */
    int64_t _subjectStartInDb;
    /** Start offset in the query database. */
    int64_t _queryStartInDb;
    /** End offset in the subject database. */
    int64_t _subjectEndInDb;
    /** End offset in the query database. */
    int64_t _queryEndInDb;

    /** Length of the alignment. */
    u_int32_t _length;

    /** Expected value. */
    double _evalue;

    /** Bit score. */
    double _bitscore;

    /** Number of gaps in the alignment. */
    size_t _nbGap;

    /** Number of identical residues in the alignment. */
    size_t _identity;

    /** Number of misses. */
    size_t _nbMis;

    /** Debug string.
     * \return a string.*/
    std::string toString ()  const
    {
        std::stringstream ss;

        ss << "[ALIGNMENT " <<
                "Q(" << _queryIdx   << ") " << _queryStartInSeq+1   << " " <<  _queryEndInSeq+1  <<
                "   " <<
                "S(" << _subjectIdx << ") " << _subjectStartInSeq+1 << " " <<  _subjectEndInSeq+1 <<
                "  len=" << _length << " nbgap=" << _nbGap << " identity=" << _identity << " nbMiss=" << _nbMis <<
                "]";

        return ss.str ();
    }

private:

    /** Configuration of the attribute.
     */
    void configure (
        const indexation::ISeedOccurrence* occur1,
        const indexation::ISeedOccurrence* occur2,
        u_int32_t leftOffsetInQuery,
        u_int32_t leftOffsetInSubject,
        u_int32_t rightOffsetInQuery,
        u_int32_t rightOffsetInSubject
    )
    {
        /** Shortcuts. */
        int32_t offInSeq1 = occur1->offsetInSequence;
        int32_t offInSeq2 = occur2->offsetInSequence;

        _subjectDb = occur1->sequence.database;
        _queryDb   = occur2->sequence.database;

        _subjectIdx = occur1->sequence.index;
        _queryIdx   = occur2->sequence.index;

        _subjectStartInSeq = offInSeq1 - leftOffsetInSubject;
        _queryStartInSeq   = offInSeq2 - leftOffsetInQuery;

        _subjectEndInSeq   = offInSeq1 + rightOffsetInSubject;
        _queryEndInSeq     = offInSeq2 + rightOffsetInQuery;

        /** Shortcuts. */
        _subjectStartInDb = occur1->offsetInDatabase - leftOffsetInSubject;
        _queryStartInDb   = occur2->offsetInDatabase - leftOffsetInQuery;

        _subjectEndInDb   = occur1->offsetInDatabase + rightOffsetInSubject;
        _queryEndInDb     = occur2->offsetInDatabase + rightOffsetInQuery;

        _length = MAX (_subjectEndInSeq - _subjectStartInSeq + 1, _queryEndInSeq - _queryStartInSeq + 1);
    }
};

/********************************************************************************/

/** \brief Interface for iterating Alignment instances
 *
 * Such iterators should be generated by the classes that hold the Alignment instances,
 * ie. IAlignmentResult.
 *
 * \see IAlignmentResult
 */
class IAlignmentIterator : public dp::Iterator<const Alignment*>
{
};

/********************************************************************************/

/* Forward declaration. */
class IAlignmentResultVisitor;

/** \brief Interface for managing Alignment instances
 *
 * This interface has two main goals:
 *      - providing means for the algorithm for checking existence of an alignment and inserting new ones
 *      - providing means for iterating alignments through the IAlignmentResultVisitor interface
 *
 * Some other auxiliaries methods are available, like shrinking the alignments list (in case of redundant
 * alignments) or reading some alignments from a file.
 *
 * The PLAST algorithm will initially creates IAlignmentResult instances that are filled by the algorithm
 * during its execution when some significant similarities are found between the subject and query
 * databases. For doing so, the PLAST algorithm will mainly use doesExist() and insert() methods.
 * Once the algorithm is finished, it is possible for the end user to iterate the found alignments
 * by giving a IAlignmentResultVisitor instance to the accept() method.
 *
 * Note that the interface makes no difference between ungap and gap alignments. Actually, it is used
 * for both cases (see for instance FullGapHitIterator class).
 */
class IAlignmentResult : public dp::SmartPointer
{
public:

    /** Tells whether or not an alignment is already known.
     * \param[in] subjectOccur : occurrence in the subject database
     * \param[in] queryOccur   : occurrence in the query database
     * \param[in] bandSize     : if not 0, size to be considered to the right and to the left
     * \return true if already existing, false otherwise
     */
    virtual bool doesExist (
        const indexation::ISeedOccurrence* subjectOccur,
        const indexation::ISeedOccurrence* queryOccur,
        size_t bandSize
    ) = 0;

    /** Tells whether or not an alignment is already known.
     * \param[in] align : the alignment to be checked
     * \return true if already existing, false otherwise
     */
    virtual bool doesExist (const Alignment& align) = 0;

    /** Insert an alignment.
     * \param[in] align   : alignment to be inserted
     * \param[in] context : some context for customizing the insertion
     * \return true if insertion ok, false otherwise. */
    virtual bool insert (Alignment& align, void* context) = 0;

    /** Remove duplicates alignments if any. This can occur when several threads try
     * to fill the same IAlignmentResult instance. This step is then required at the
     * end of the algorithm.
     */
    virtual void shrink (void) = 0;

    /** Give the number of known alignments.
     * \return the number of alignments
     */
    virtual u_int32_t getSize () = 0;

    /** Accept method of the Visitor Design Pattern.
     * \param[in] visitor : the visitor to be accepted
     */
    virtual void accept (IAlignmentResultVisitor* visitor) {}

    /** Get alignments for given query and subject sequences.
     * \param[in] querySeq : the query sequence
     * \param[in] subjectSeq : the subject sequence
     * \return the list of alignments for the given query/subject sequences
     */
    virtual std::list<Alignment> getAlignments (const database::ISequence* querySeq, const database::ISequence* subjectSeq) = 0;

    /** Return properties about the instance.
     * \param[in] root : root string for the properties
     * \return a new IProperties instance
     */
    virtual dp::IProperties* getProperties (const std::string& root) = 0;

    /** Read alignments from a file. Different implementations could read from different file format.
     * \param[in] fileuri : path of the file to be read
     * \param[out] subjectComments : container for the subject comments (in FASTA file for instance)
     * \param[out] queryComments   : container for the query comments (in FASTA file for instance)
     */
    virtual void readFromFile (
        const char* fileuri,
        std::map<std::string,int>& subjectComments,
        std::map<std::string,int>& queryComments
    ) = 0;
};

/********************************************************************************/

/** \brief Visitor Design Pattern that can visit IAlignmentResult instances.
 *
 * IAlignmentResult instances manage some set of Alignment instances.
 *
 * A mean for iterating these alignments would have to add a factory method in the
 * IAlignmentResult interface creating iterators on the contained Alignment instances.
 *
 * By doing so, we would have lost some hierarchic information. For instance, it is
 * interesting to sort the alignments per query, and then to sort them per subject.
 *
 * Using a visitor approach allows to keep this structural information. In our visitor
 * definition here, we have three methods:
 *      - 1. visitQuerySequence()   : called each time a new query sequence is found in the whole alignments set
 *      - 2. visitSubjectSequence() : called each time a new subject sequence is found in the whole alignments set
 *      - 3. visitAlignment()       : called each time a new alignment is found for the currently visited query/subject sequences.
 *
 * For instance, we could have a series of calls like this: 1,2,3,3,2,3,3,3,1,2,3,3,3,3 which means that we found alignements
 * in two different query sequences, with two subject sequences matches for the first one, and only one for the other.
 *
 * Implementors of this interface therefore can use this hierarchy for structuring themself what they want; this is useful
 * for instance for dumping the alignment in a XML file.
 */
class IAlignmentResultVisitor : public dp::SmartPointer
{
public:

    /** Called when a new query sequence is visited.
     * \param[in] seq : the visited query sequence.
     */
    virtual void visitQuerySequence   (const database::ISequence* seq)    = 0;

    /** Called when a new subject sequence is visited for the currently visited query sequence
     * \param[in] seq : the visited subject sequence.
     */
    virtual void visitSubjectSequence (const database::ISequence* seq)    = 0;

    /** Called when a new alignment is visited for the currenlyt visited query and subject sequences.
     * \param[in] align : the visited alignment.
     */
    virtual void visitAlignment       (const Alignment* align) = 0;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _IALIGNMENT_RESULT_HPP_ */
