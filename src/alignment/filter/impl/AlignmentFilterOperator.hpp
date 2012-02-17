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

/** \file OperatorAlignmentFilter.hpp
 *  \brief Concepts about alignments filtering.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _OPERATOR_ALIGNMENT_FILTER_HPP_
#define _OPERATOR_ALIGNMENT_FILTER_HPP_

/********************************************************************************/

#include <alignment/filter/api/IAlignmentFilter.hpp>

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/** We define a class only for naming filters with one unary operator. */
template <typename T> class AlignmentFilterUnaryOperator: public IAlignmentFilter
{
public:

    AlignmentFilterUnaryOperator () : _operator(UNKNOWN), _value(0)  {}

    AlignmentFilterUnaryOperator (const std::vector<std::string>& args) : _operator(UNKNOWN), _value(0)
    {
        static const char* convert[] = { "?", "=", "!=", "<", ">", "<=", ">=", 0 };

        for (size_t i=0; (_operator==UNKNOWN) && (convert[i] != 0); i++)
        {
            if (args[0].compare(convert[i])==0)
            {
                _operator    = (OP) i;
                _operatorStr = convert[i];
            }
        }

        std::stringstream ss (args[1]);
        ss >> _value;
    }

    std::string toString ()
    {
        std::stringstream ss;
        ss << getName () << " " << _operatorStr << " " << _value;
        return ss.str ();
    }

protected:

    /** Define an enumeration for basic operators.  */
    enum OP  {  UNKNOWN, EQ, DIFF, LT, GT, LTE, GTE };

    OP _operator;
    T  _value;

    std::string _operatorStr;
};

/********************************************************************************/

/** We use a macro for code factorization. Indeed, we will need a filter per attribute
 * of the Alignment class (no introspection in c++...)
 */
#define DEFINE_ALIGNMENT_EXP_FILTER(tag,name,type,exp)  \
    class AlignmentFilter_##name : public AlignmentFilterUnaryOperator<type>   \
    { \
    public: \
        AlignmentFilter_##name ()   {} \
        AlignmentFilter_##name (const std::vector<std::string>& args) : AlignmentFilterUnaryOperator<type>(args)  {} \
        bool isOk (const core::Alignment& a) const { \
            switch(_operator) \
            { \
                case EQ:    return (exp) == _value; \
                case DIFF:  return (exp) != _value; \
                case LT:    return (exp) <  _value; \
                case GT:    return (exp) >  _value; \
                case LTE:   return (exp) <= _value; \
                case GTE:   return (exp) >= _value; \
                default:    return false; \
            } \
        } \
        IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilter_##name (args); } \
        std::string getName() {  return std::string(tag); } \
    };


/** Now, we define a bunch of filter classes */
DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit from",            HitFrom,            u_int32_t,  a.getSbjRange().begin);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit to",              HitTo,              u_int32_t,  a.getSbjRange().end);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query from",          QueryFrom,          u_int32_t,  a.getQryRange().begin);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query to",            QueryTo,            u_int32_t,  a.getQryRange().end);

DEFINE_ALIGNMENT_EXP_FILTER ("HSP alignment length",    Length,             u_int32_t,  a.getLength());

DEFINE_ALIGNMENT_EXP_FILTER ("HSP E­‐Value",             Evalue,             double,     a.getEvalue());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP bit score",           Bitscore,           double,     a.getBitScore());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP score",               Score,              u_int16_t,  a.getScore());

DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of identities",     NbIdentities,       int32_t,    a.getNbIdentities());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of identities",     PercentIdentities,  double,     a.getPercentIdentities() * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("Query Coverage",          QueryCoverage,      double,     a.getQryCoverage()       * 100.0);
DEFINE_ALIGNMENT_EXP_FILTER ("Hit Coverage",            SubjectCoverage,    double,     a.getSbjCoverage()       * 100.0);

//DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of misses",         NbMissses,          int32_t,    a.getNbMisses());
//DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of gaps",           PercentGaps,        double,     a.getPercentGaps()       * 100.0);

/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentFilterLogical : public IAlignmentFilter
{
public:

    AlignmentFilterLogical (IAlignmentFilter* f1, IAlignmentFilter* f2) : _f1(f1), _f2(f2)  {}

    virtual ~AlignmentFilterLogical () {}

    std::string toString () {
        std::stringstream ss;
        ss << "(" << _f1->toString()   << ")"
           << " " << operatorString()  << " "
           << "(" << _f2->toString()   << ")";
        return ss.str();
    }

protected:
    virtual std::string operatorString () = 0;
    IAlignmentFilter* _f1;
    IAlignmentFilter* _f2;
};

/********************************************************************************/
/** \brief Definition of an alignment filtering
 */
class AlignmentFilterAnd : public AlignmentFilterLogical
{
public:
    AlignmentFilterAnd (IAlignmentFilter* f1, IAlignmentFilter* f2) : AlignmentFilterLogical (f1,f2)  {}
    bool isOk (const core::Alignment& align) const { return _f1->isOk(align) && _f2->isOk(align);  }
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterAnd (_f1, _f2); }
    virtual std::string getName  () { return "FilterAnd"; }
protected:
    std::string operatorString () { return std::string("AND"); }
};

/********************************************************************************/
/** \brief Definition of an alignment filtering
 */
class AlignmentFilterOr : public AlignmentFilterLogical
{
public:
    AlignmentFilterOr (IAlignmentFilter* f1, IAlignmentFilter* f2) : AlignmentFilterLogical (f1,f2)  {}
    bool isOk (const core::Alignment& align) const  { return _f1->isOk(align) || _f2->isOk(align);  }
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterOr (_f1, _f2); }
    virtual std::string getName  () { return "FilterOr"; }
protected:
    std::string operatorString () { return std::string("OR"); }
};

/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentFilterFalse : public IAlignmentFilter
{
public:
    bool isOk (const core::Alignment& align) const  { return false; }
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterFalse (); }
    virtual std::string toString () { return "FALSE"; }
    virtual std::string getName  () { return "FilterFalse"; }
};

/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentFilterTrue : public IAlignmentFilter
{
public:
    bool isOk (const core::Alignment& align) const  { return true; }
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterTrue (); }
    virtual std::string toString () { return "TRUE"; }
    virtual std::string getName  () { return "FilterTrue"; }
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _OPERATOR_ALIGNMENT_FILTER_HPP_ */
