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
namespace alignment {
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
 *      - number of misses between the two databases
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
        AlignSequenceInfo () : _sequence(0), _nbGaps(0) {}

        AlignSequenceInfo (
            database::ISequence* sequence,
            misc::Range32        range,
            u_int16_t            nbGaps=0)
            : _sequence(sequence), _range(range), _nbGaps(nbGaps) {}

        database::ISequence* _sequence;
        misc::Range32        _range;
        u_int16_t            _nbGaps;
        u_int64_t getOffsetInDb () const { return _range.begin + _sequence->offsetInDb; }
    };

    /**********************************************************************/
    enum DbKind { QUERY, SUBJECT };

    /**********************************************************************
     * Default constructor.
     **********************************************************************/
    Alignment ()  :  _length(0), _evalue(0), _bitscore(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0)  {}

    /**********************************************************************
     * Constructor that just define query and subject ranges (mainly for test purpose).
     **********************************************************************/
    Alignment (const misc::Range32& sbjRange, const misc::Range32& qryRange)
        :  _length(0), _evalue(0), _bitscore(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0)
    {
        setSbjRange (sbjRange);
        setQryRange (qryRange);
    }

    /**********************************************************************
     * Constructor that just define query and subject ranges (mainly for test purpose).
     **********************************************************************/
    Alignment (const AlignSequenceInfo& sbjInfo, const AlignSequenceInfo& qryInfo)
        :  _length(0), _evalue(0), _bitscore(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0)
    {
        _sbjInfo = sbjInfo;
        _qryInfo = qryInfo;
    }

    /**********************************************************************
     * Constructor. This is the typical constructor used when some HSP has
     * been computed and ranges in query/subject have been computed by some
     * dynamic programmation. Here, we memorize these information and provide
     * getter to access it.
     **********************************************************************/
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
        : _length(0), _evalue(0), _bitscore(0), _nbIdentities(0), _nbPositives(0), _nbMisses(0)
    {
        _qryInfo._sequence     = (database::ISequence*)qrySequence;
        _qryInfo._range.begin  = qryOffset - qryLeftOffset;
        _qryInfo._range.end    = qryOffset + qryRightOffset;

        _sbjInfo._sequence     = (database::ISequence*)sbjSequence;
        _sbjInfo._range.begin  = sbjOffset - sbjLeftOffset;
        _sbjInfo._range.end    = sbjOffset + sbjRightOffset;

        _length = 1 + MAX (sbjRightOffset + sbjLeftOffset,  qryRightOffset + qryLeftOffset);
    }

    /**********************************************************************
     * Define some way to compare two alignments. It returns the overlap ratio
     *  of the two provided alignments.
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
    const AlignSequenceInfo& getSbjInfo () const { return _sbjInfo; }
    const AlignSequenceInfo& getQryInfo () const { return _qryInfo; }

    const database::ISequence* getSbjSequence () const { return _sbjInfo._sequence; }
    const database::ISequence* getQrySequence () const { return _qryInfo._sequence; }

    const misc::Range32& getSbjRange ()  const { return _sbjInfo._range; }
    const misc::Range32& getQryRange ()  const { return _qryInfo._range; }

    u_int16_t getSbjNbGaps () const { return _sbjInfo._nbGaps; }
    u_int16_t getQryNbGaps () const { return _qryInfo._nbGaps; }

    double getSbjCoverage () const  {  return (double)(getSbjRange().getLength()) / (double) (getSbjSequence()->getLength());  }
    double getQryCoverage () const  { return (double)(getQryRange().getLength()) / (double) (getQrySequence()->getLength());  }

    u_int32_t getLength ()  const { return _length; }

    double    getEvalue   ()  const { return _evalue;    }
    double    getBitScore ()  const { return _bitscore;  }
    u_int16_t getScore    ()  const { return _score;     }

    u_int32_t getNbIdentities ()       const { return _nbIdentities; }
    double    getPercentIdentities ()  const { return (double)getNbIdentities() / (double)getLength(); }

    u_int32_t getNbPositives ()        const { return _nbPositives; }
    double    getPercentPositives ()   const { return (double)getNbPositives() / (double)getLength(); }

    u_int32_t getNbMisses ()           const { return _nbMisses; }
    double    getPercentMisses ()      const { return (double)getNbMisses() / (double)getLength(); }

    u_int32_t getNbGaps ()        const { return getSbjNbGaps() + getQryNbGaps(); }
    double    getPercentGaps ()   const { return (double)getNbGaps() / (double)getLength(); }

    /**********************************************************************
     * Setters.
     **********************************************************************/
    void setSbjSequence  (database::ISequence* seq) { _sbjInfo._sequence = seq; }
    void setQrySequence  (database::ISequence* seq) { _qryInfo._sequence = seq; }

    void setSbjRange     (const misc::Range32& range) { _sbjInfo._range = range; }
    void setQryRange     (const misc::Range32& range) { _qryInfo._range = range; }

    void setSbjNbGaps    (u_int16_t nb)  { _sbjInfo._nbGaps = nb; }
    void setQryNbGaps    (u_int16_t nb)  { _qryInfo._nbGaps = nb; }

    void setNbGaps       (u_int16_t nb)
    {
        _qryInfo._nbGaps = (nb%2==0 ? nb/2 : (nb-1)/2);
        _sbjInfo._nbGaps = (nb%2==0 ? nb/2 : (nb+1)/2);
    }

    void setLength       (u_int32_t nb)  { _length = nb; }

    void setEvalue       (double    value)  { _evalue   = value;  }
    void setBitScore     (double    value)  { _bitscore = value;  }
    void setScore        (u_int16_t value)  { _score    = value;  }

    void setNbIdentities (u_int32_t nb)  { _nbIdentities = nb; }
    void setNbPositives  (u_int32_t nb)  { _nbPositives  = nb; }
    void setNbMisses     (u_int32_t nb)  { _nbMisses     = nb; }

    /**********************************************************************
     * Getters and setters (generic).
     **********************************************************************/
    const database::ISequence*  getSequence   (DbKind kind) const  { return kind==SUBJECT ? getSbjSequence()  : getQrySequence();  }
    const misc::Range32&        getRange      (DbKind kind) const  { return kind==SUBJECT ? getSbjRange()     : getQryRange();     }
    u_int16_t                   getNbGaps     (DbKind kind) const  { return kind==SUBJECT ? getSbjNbGaps()    : getQryNbGaps();    }
    double                      getCoverage   (DbKind kind) const  { return kind==SUBJECT ? getSbjCoverage()  : getQryCoverage();  }

    void setSequence  (DbKind kind, database::ISequence* seq)
    {
        if (kind==SUBJECT)  { setSbjSequence (seq); }
        else                { setQrySequence (seq); }
    }

    void setRange  (DbKind kind, const misc::Range32& range)
    {
        if (kind==SUBJECT)  { setSbjRange (range); }
        else                { setQryRange (range); }
    }

    void setNbGaps (DbKind kind, u_int16_t nb)
    {
        if (kind==SUBJECT)  { setSbjNbGaps (nb); }
        else                { setQryNbGaps (nb); }
    }

    /**********************************************************************
     * Debug string.
     * \return a string.
     **********************************************************************/
    std::string toString ()  const
    {
        std::stringstream ss;
        ss << "ALIGN  "
           << "sbj [" << getSbjRange().begin << ":" << getSbjRange().end << "]  "
           << "qry [" << getQryRange().begin << ":" << getQryRange().end << "]  ";
        return ss.str ();
    }

private:

    /** */
    AlignSequenceInfo _sbjInfo;
    /** */
    AlignSequenceInfo _qryInfo;

    /** Length of the alignment. */
    u_int32_t _length;

    /** Expected value. */
    double _evalue;

    /** Bit score. */
    double _bitscore;

    /** HSP score. */
    u_int16_t _score;

    /** Number of identical residues in the alignment. */
    u_int32_t _nbIdentities;

    /** Number of positive residues alignments (ie substition score > 0). */
    u_int32_t _nbPositives;

    /** Number of missed residues alignments. */
    u_int32_t _nbMisses;
};

/********************************************************************************/
}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _ALIGNMENT_HPP_ */
