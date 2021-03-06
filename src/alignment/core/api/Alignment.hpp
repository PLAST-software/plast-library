/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

/** \file Alignment.hpp
 *  \brief Definition of an alignment.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ALIGNMENT_HPP_
#define _ALIGNMENT_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>
#include <misc/api/macros.hpp>
#include <database/api/ISequenceDatabase.hpp>

#include <iostream>
#include <sstream>

/********************************************************************************/
/** Namespace holding material related to several aspects of alignments (information, containers, filter...) */
namespace alignment {
/** Definition of alignment concept and the associated containers. */
namespace core      {
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
 *      - number of mismatches between the two databases
 *
 * Note that we have here redundant information; for instance, the offsets in the sequence may
 * be retrieved from the offsets in the database. The redundancy is here only to avoid some
 * re-computations.
 */
class Alignment
{
public:

    /** Structure that links alignment and sequence information.  */
    struct AlignSequenceInfo
    {
        AlignSequenceInfo () : _sequence(0), _nbGaps(0), _frame(0), _isTranslated(false) {}

        AlignSequenceInfo (
            database::ISequence* sequence,
            misc::Range32        range,
            u_int16_t            nbGaps = 0,
            int8_t               frame  = 0,
            bool                 isTranslated = false
        )  : _sequence(sequence), _range(range), _nbGaps(nbGaps), _frame(0), _isTranslated(isTranslated)  {}

        database::ISequence* _sequence;
        misc::Range32        _range;
        u_int16_t            _nbGaps;

        /** Frame for the alignment. +1, +2, +3 ,-1, -2 ,-3 according to the frame. */
        int8_t _frame;

        /** Tells whether the sequence has been translated from nucleotides to amino acids. */
        bool  _isTranslated;

        u_int64_t getOffsetInDb () const { return _range.begin + _sequence->offsetInDb; }

        /** */
        friend std::ostream& operator<< (std::ostream& s, const AlignSequenceInfo& o)
        {
            char c = ' ';
            return s << o._range << c << o._nbGaps;
        }

        /** */
        friend std::istream& operator>> (std::istream& s, AlignSequenceInfo& o)
        {
            char c;
            return s >> o._range >> c >> o._nbGaps;
        }
    };

    /** Structure that provides transient information (useful during containers visit).  */
    struct AlignExtraInfo
    {
        misc::ProgressInfo qryProgress;
        misc::ProgressInfo sbjProgress;
        misc::ProgressInfo alignProgress;
    };

    /** Enumeration that distinguish the two kinds of databases. */
    enum DbKind { QUERY=0, SUBJECT=1 };

    /** Default constructor. */
    Alignment ()  :  _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)  {}

    /** Constructor that just define query and subject ranges (mainly for test purpose).
     * \param[in] sbjRange : range in the subject sequence
     * \param[in] qryRange : range in the query sequence.
     */
    Alignment (const misc::Range32& sbjRange, const misc::Range32& qryRange)
        :  _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)
    {
        setRange (SUBJECT, sbjRange);
        setRange (QUERY,   qryRange);
    }

    /** Constructor that just define query and subject AlignSequenceInfo (mainly for test purpose).
     * \param[in] sbjInfo : information related to the subject database
     * \param[in] qryInfo : information related to the querydatabase
     */
    Alignment (const AlignSequenceInfo& sbjInfo, const AlignSequenceInfo& qryInfo)
        :  _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)
    {
        _info[QUERY]   = qryInfo;
        _info[SUBJECT] = sbjInfo;
    }

    /** Constructor. This is the typical constructor used when some HSP has
     *  been computed and ranges in query/subject have been computed by some
     *  dynamic programming. Here, we memorize these information and provide
     *  getter to access it.
     *  \param[in] qrySequence    : reference to the query sequence
     *  \param[in] sbjSequence    : reference to the subject sequence
     *  \param[in] qryOffset      : offset in the query sequence of the initial seed
     *  \param[in] sbjOffset      : offset in the subject sequence of the initial seed
     *  \param[in] qryLeftOffset  : length in the query sequence of the alignment extension to the left of the initial seed
     *  \param[in] sbjLeftOffset  : length in the subject sequence of the alignment extension to the left of the initial seed
     *  \param[in] qryRightOffset : length in the query sequence of the alignment extension to the right of the initial seed
     *  \param[in] sbjRightOffset : length in the subject sequence of the alignment extension to the right of the initial seed
     */
    Alignment (
        const database::ISequence* qrySequence,
        const database::ISequence* sbjSequence,
        u_int32_t qryOffset,
        u_int32_t sbjOffset,
        u_int32_t qryLeftOffset,
        u_int32_t sbjLeftOffset,
        u_int32_t qryRightOffset,
        u_int32_t sbjRightOffset
    )
        : _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)
    {
        _info[QUERY]._sequence     = (database::ISequence*)qrySequence;
        _info[QUERY]._range.begin  = qryOffset - qryLeftOffset;
        _info[QUERY]._range.end    = qryOffset + qryRightOffset;

        _info[SUBJECT]._sequence     = (database::ISequence*)sbjSequence;
        _info[SUBJECT]._range.begin  = sbjOffset - sbjLeftOffset;
        _info[SUBJECT]._range.end    = sbjOffset + sbjRightOffset;

        _length = 1 + MAX (sbjRightOffset + sbjLeftOffset,  qryRightOffset + qryLeftOffset);
    }

    /** Constructor. This is the typical constructor used when some HSP has
     * been computed and ranges in query/subject have been computed by some
     * dynamic programming. Here, we memorize these information and provide
     * getter to access it.
     * \param[in] qrySequence : reference to the query sequence
     * \param[in] sbjSequence : reference to the subject sequence
     * \param[in] qryRange    : range of [left,right] offsets of the alignment in the query sequence
     * \param[in] sbjRange    : range of [left,right] offsets of the alignment in the subject sequence
     */
    Alignment (
        const database::ISequence* qrySequence,
        const database::ISequence* sbjSequence,
        const misc::Range32& qryRange,
        const misc::Range32& sbjRange
    )
        : _length(0), _evalue(0), _bitscore(0), _score(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0), _extraInfo(0)
    {
        _info[QUERY]._sequence = (database::ISequence*)qrySequence;
        _info[QUERY]._range    = qryRange;

        _info[SUBJECT]._sequence = (database::ISequence*)sbjSequence;
        _info[SUBJECT]._range    = sbjRange;

        _length = MAX (qryRange.getLength(), sbjRange.getLength());
    }

    /** Constructor that configure an alignment from some string source (mainly for test purpose).
     * \param[in] source : line to be parsed (looks like a tabulated line output from blast)
     * \param[in] format : kind of format of the source line
     */
    Alignment (const std::string& source, const std::string& format="tabulated");

    /** Define some way to compare two alignments. It returns the overlap ratio of the two provided alignments.
     * \param[in] a1 : first alignment
     * \param[in] a2 : second alignment
     * \return overlap value; it is a ratio, so it lies in [0;1]
     **********************************************************************/
    static double overlap (const Alignment& a1, const Alignment& a2)
    {
        double o1 = 0.0;
        double o2 = 0.0;

        /** We define here a little macro for easing the overlap impl. */
        #define VAL(type,kind,bound)    (type (a1.getRange(kind).bound, a2.getRange(kind).bound))

        if (VAL(MIN,Alignment::QUERY,end) > VAL(MAX,Alignment::QUERY,begin))
        {
            o1 = (double) (VAL(MIN,Alignment::QUERY,end) - VAL(MAX,Alignment::QUERY,begin) + 1) /
                 (double) (VAL(MAX,Alignment::QUERY,end) - VAL(MIN,Alignment::QUERY,begin) + 1);
        }
        else
        {
            o1 = (double) (VAL(MIN,Alignment::QUERY,begin) - VAL(MAX,Alignment::QUERY,end) + 1) /
                 (double) (VAL(MAX,Alignment::QUERY,begin) - VAL(MIN,Alignment::QUERY,end) + 1);
        }

        if (VAL(MIN,Alignment::SUBJECT,end) > VAL(MAX,Alignment::SUBJECT,begin))
        {
            o2 = (double) (VAL(MIN,Alignment::SUBJECT,end) - VAL(MAX,Alignment::SUBJECT,begin) + 1) /
                 (double) (VAL(MAX,Alignment::SUBJECT,end) - VAL(MIN,Alignment::SUBJECT,begin) + 1);
        }
        else
        {
            o2 = (double) (VAL(MIN,Alignment::SUBJECT,begin) - VAL(MAX,Alignment::SUBJECT,end) + 1) /
                 (double) (VAL(MAX,Alignment::SUBJECT,begin) - VAL(MIN,Alignment::SUBJECT,end) + 1);
        }

        return MAX (0, MIN (o1, o2));
    }

    /**********************************************************************
     * Getters.
     **********************************************************************/

    /** Return the length of the alignment. This length may include gaps.
     * \return the length of the alignment. */
    u_int32_t getLength ()  const { return _length; }

    /** \return the evalue */
    double    getEvalue   ()  const { return _evalue;    }

    /** \return the bit score */
    double    getBitScore ()  const { return _bitscore;  }

    /** \return the raw score */
    u_int32_t getScore    ()  const { return _score;     }

    /** \return the number of identical residues between the two sequences for this alignment */
    u_int32_t getNbIdentities ()       const { return _nbIdentities; }

    /** \return the identity percentage */
    double    getPercentIdentities ()  const { return (double)getNbIdentities() / (double)getLength(); }

    /** \return the number of residues having a positive score (with BLOSUM62 for instance) */
    u_int32_t getNbPositives ()        const { return _nbPositives; }

    /** \return the positive percentage */
    double    getPercentPositives ()   const { return (double)getNbPositives() / (double)getLength(); }

    /** \return the number of mismatches between the two sequences for this alignment */
    u_int32_t getNbMisses ()           const { return _nbMisses; }

    /** \return the mismatch percentage */
    double    getPercentMisses ()      const { return (double)getNbMisses() / (double)getLength(); }

    /** \return the number of gaps in the alignment. */
    u_int32_t getNbGaps ()        const { return getNbGaps(QUERY) + getNbGaps(SUBJECT); }

    /** \return the percentage of gaps */
    double    getPercentGaps ()   const { return (double)getNbGaps() / (double)getLength(); }

    const misc::ProgressInfo& getQryProgress () const
    {
        if (_extraInfo != 0)  { return _extraInfo->qryProgress;     }
        else                  { return misc::ProgressInfo::null (); }
    }

    const misc::ProgressInfo& getSbjProgress () const
    {
        if (_extraInfo != 0)  { return _extraInfo->sbjProgress;     }
        else                  { return misc::ProgressInfo::null (); }
    }

    const misc::ProgressInfo& getAlignProgress () const
    {
        if (_extraInfo != 0)  { return _extraInfo->alignProgress;     }
        else                  { return misc::ProgressInfo::null (); }
    }

    /**********************************************************************
     * Setters.
     **********************************************************************/

    /** Set the number of gaps
     * \param[in] nb : the number of gaps */
    void setNbGaps       (u_int16_t nb)
    {
        setNbGaps (QUERY,   (nb%2==0 ? nb/2 : (nb-1)/2));
        setNbGaps (SUBJECT, (nb%2==0 ? nb/2 : (nb+1)/2));
    }

    /** Set the alignment length
     * \param[in] nb : the alignment length */
    void setLength       (u_int32_t nb)  { _length = nb; }

    /** Set the evalue
     * \param[in] value : the evalue */
    void setEvalue       (double    value)  { _evalue   = value;  }

    /** Set the bit score
     * \param[in] value : the bit score */
    void setBitScore     (double    value)  { _bitscore = value;  }

    /** Set the raw score
     * \param[in] value : the raw score */
    void setScore        (u_int32_t value)  { _score    = value;  }

    /** Set the number of identities
     * \param[in] nb : the identities number */
    void setNbIdentities (u_int32_t nb)             { _nbIdentities = nb;   }

    /** Set the number of positives
     * \param[in] nb : the positives number */
    void setNbPositives  (u_int32_t nb)             { _nbPositives  = nb;   }

    /** Set the number of mismatches
     * \param[in] nb : the mismatches number */
    void setNbMisses     (u_int32_t nb)             { _nbMisses     = nb;   }

    void setExtraInfo    (AlignExtraInfo* info)     { _extraInfo    = info; }

    /**********************************************************************
     * Getters and setters (generic).
     **********************************************************************/

    /** Get alignment information for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the alignment information */
    const AlignSequenceInfo&    getInfo       (DbKind kind) const  { return _info[kind]; }

    /** Get sequence reference for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the sequence reference */
    const database::ISequence*  getSequence   (DbKind kind) const  { return getInfo(kind)._sequence;  }

    /** Get range for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the range
     */
    const misc::Range32&        getRange      (DbKind kind) const  { return getInfo(kind)._range;     }

    /** Get number of gaps for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the number of gaps
     */
    u_int16_t                   getNbGaps     (DbKind kind) const  { return getInfo(kind)._nbGaps;    }

    /** Get frame number for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the frame number
     */
    int8_t                      getFrame      (DbKind kind) const  { return getInfo(kind)._frame; }

    /** Tells whether the sequence has been translated from nucleotides to amino acids
     * \param[in] kind : QUERY or SUBJECT
     * \return the translation state
     */
    bool                        isTranslated      (DbKind kind) const  { return getInfo(kind)._isTranslated; }

    /** Get alignment coverage for query or subject
     * \param[in] kind : QUERY or SUBJECT
     * \return the alignment coverage as a ratio */
    double getCoverage   (DbKind kind) const
    {
        u_int32_t seqLen = getSequence(kind)->getLength();

        if (isTranslated(kind) == true)  { seqLen *= 3; }

        return  (double)(getRange(kind).getLength()) / (double) seqLen;
    }

    /** Set the sequence reference for QUERY or SUBJECT
     * \param[in] kind : QUERY or SUBJECT
     * \param seq : the reference on the sequence */
    void setSequence  (DbKind kind, database::ISequence* seq)    {  _info[kind]._sequence = seq;    }

    /** Set the range for QUERY or SUBJECT
     * \param[in] kind : QUERY or SUBJECT
     * \param range : the range */
    void setRange     (DbKind kind, const misc::Range32& range)  {  _info[kind]._range    = range;  }

    /** Set the number of gaps for QUERY or SUBJECT
     * \param[in] kind : QUERY or SUBJECT
     * \param nb : the number of gaps */
    void setNbGaps    (DbKind kind, u_int16_t nb)                {  _info[kind]._nbGaps   = nb;     }

    /** Set the frame number for QUERY or SUBJECT
     * \param[in] kind : QUERY or SUBJECT
     * \param frame : the frame number */
    void setFrame     (DbKind kind, int8_t frame)                {  _info[kind]._frame    = frame;  }

    /** Set the translation status for QUERY or SUBJECT
     * \param[in] kind : QUERY or SUBJECT
     * \param isTranslated : the translation status */
    void setIsTranslated (DbKind kind, bool translated)          {  _info[kind]._isTranslated = translated;  }

    /** Return a string representing some information of the alignment (for test/debug purpose).
     * \return a string. */
    std::string toString ()  const
    {
        std::stringstream ss;
        ss << "ALIGN  "
           << "qry [" << getRange(QUERY).begin+1   << ":" << getRange(QUERY).end+1   << "  frm=" << (int)getFrame(QUERY)   << "]  "
           << "sbj [" << getRange(SUBJECT).begin+1 << ":" << getRange(SUBJECT).end+1 << "  frm=" << (int)getFrame(SUBJECT) << "]  ";
        return ss.str ();
    }

    /** Overload of output stream for Alignment class
     * \param[in] s : the output stream
     * \param[in] o : the alignment to be output
     * \return the output stream */
    friend std::ostream& operator<< (std::ostream& s, const Alignment& o)
    {
        char c = ' ';
        s << o._info[QUERY]       << c
          << o._info[SUBJECT]     << c
          << o._length            << c
          << o._evalue            << c
          << o._bitscore          << c
          << o._nbIdentities      << c
          << o._nbPositives       << c
          << o._nbMisses;
        return s;
    }

    /** Overload of input stream for Alignment class
     * \param[in] s : the input stream
     * \param[in] o : the alignment to be input
     * \return the input stream */
    friend std::istream& operator>> (std::istream& s, Alignment& o)
    {
        char c = ' ';
        s >> o._info[QUERY]       >> c
          >> o._info[SUBJECT]     >> c
          >> o._length            >> c
          >> o._evalue            >> c
          >> o._bitscore          >> c
          >> o._nbIdentities      >> c
          >> o._nbPositives       >> c
          >> o._nbMisses;
        return s;
    }

private:

    /** Factorized information holding sequence, range and number of gaps. */
    AlignSequenceInfo _info[2];

    /** Length of the alignment. */
    u_int32_t _length;

    /** Expected value. */
    double _evalue;

    /** Bit score. */
    double _bitscore;

    /** HSP score (need 32 bits for ADN sequences that can be huge). */
    u_int32_t _score;

    /** Number of identical residues in the alignment. */
    u_int32_t _nbIdentities;

    /** Number of positive residues alignments (ie substitution score > 0). */
    u_int32_t _nbPositives;

    /** Number of mismatches residues alignments. */
    u_int32_t _nbMisses;

    /** Extra information. Can be transient (ie reference may change during Alignment instance life). */
    AlignExtraInfo* _extraInfo;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_HPP_ */
