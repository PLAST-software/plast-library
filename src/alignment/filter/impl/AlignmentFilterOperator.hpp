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
#include <designpattern/impl/Property.hpp>
#include <stdarg.h>
#include <boost/regex.hpp>

/********************************************************************************/
namespace alignment {
namespace filter    {
namespace impl      {
/********************************************************************************/

/** Some factorization. */
class AbstractAlignmentFilter : public IAlignmentFilter
{
public:
    std::string getTitle () { return _title; }
    void setTitle (const std::string& title)  { _title = title; }

private:
    std::string _title;
};

/********************************************************************************/

/** \brief Definition of an alignment filtering
 */
class AlignmentFilterBinaryOperator : public AbstractAlignmentFilter
{
public:

    AlignmentFilterBinaryOperator (std::list<IAlignmentFilter*>& filters);
    virtual ~AlignmentFilterBinaryOperator ();

    bool isOk (const core::Alignment& align) const;

    dp::IProperties* getProperties ();

    std::string toString ();

protected:
    std::list<IAlignmentFilter*> _filters;

    virtual bool getResult (bool currentResult, IAlignmentFilter* currentFilter, const core::Alignment& align) const = 0;
};

/********************************************************************************/
/** \brief Definition of an alignment filtering
 */
class AlignmentFilterAnd : public AlignmentFilterBinaryOperator
{
public:
    AlignmentFilterAnd (std::list<IAlignmentFilter*> filters) : AlignmentFilterBinaryOperator (filters)  {}
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterAnd (_filters); }
    virtual std::string getName  () { return "AND"; }

protected:
    bool getResult (bool result, IAlignmentFilter* currentFilter, const core::Alignment& align) const
    {
        return result && currentFilter->isOk(align);
    }
};

/********************************************************************************/
/** \brief Definition of an alignment filtering
 */
class AlignmentFilterOr : public AlignmentFilterBinaryOperator
{
public:
    AlignmentFilterOr (std::list<IAlignmentFilter*> filters) : AlignmentFilterBinaryOperator (filters)  {}
    IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilterOr (_filters); }
    virtual std::string getName  () { return "OR"; }
protected:
    bool getResult (bool result, IAlignmentFilter* currentFilter, const core::Alignment& align) const
    {
        return result || currentFilter->isOk(align);
    }
};

/********************************************************************************/

/** We define a class only for naming filters with one unary operator. */
template <typename T> class AlignmentFilterUnaryOperator : public AbstractAlignmentFilter
{
public:

    /** */
    AlignmentFilterUnaryOperator () : _operator(UNKNOWN) {}

    /** */
    AlignmentFilterUnaryOperator (const std::vector<std::string>& args) : _operator(UNKNOWN)//, _value(0)
    {
        configure (args);
    }

    /** */
    dp::IProperties* getProperties ()
    {
        dp::IProperties* result = new dp::impl::Properties ();
        result->add (0, "filter", toString ());
        return result;
    }

    /** */
    std::string toString ()
    {
        std::stringstream ss;

        ss << getName () << " " << _operatorStr << " ";
        for (size_t i=0; i<_values.size(); i++)  {  ss << _values[i] << " ";  }

        return ss.str ();
    }

protected:

    /** Define an enumeration for basic operators.  */
    enum OP  {  UNKNOWN, EQ, DIFF, LT, GT, LTE, GTE, IN_RANGE, NOT_IN_RANGE, HOLD, NO_HOLD };

    OP            _operator;
    T             _value;
    std::vector<T>  _values;

    std::string _operatorStr;

    /** */
    void configure (const char* op, va_list ap)
    {
        std::vector<std::string> args;
        args.push_back (op);
        for (const char* val=0; (val = va_arg(ap, const char*)) != 0 ; )  { args.push_back (val); }
        configure (args);
    }

    /** */
    void configure (const std::vector<std::string>& args)
    {
        static const char* convert[] = {
            "?", "==", "!=", "<", ">", "<=", ">=", "[]", "][", "::", "!:", 0
        };

        for (size_t i=0; (_operator==UNKNOWN) && (convert[i] != 0); i++)
        {
            if (args[0].compare(convert[i])==0)
            {
                _operator    = (OP) i;
                _operatorStr = convert[i];
            }
        }

        T tmp;
        for (size_t i=1; i<args.size(); i++)
        {
            std::stringstream ss (args[i], std::stringstream::in);
            ss >> tmp;
            _values.push_back (tmp);
        }

        if (_values.empty() == false)  {  _value = _values[0];  }
    }
};

/********************************************************************************/

/**  */
class AlignmentFilterRegexOperator : public AlignmentFilterUnaryOperator<std::string>
{
public:

    /** */
    AlignmentFilterRegexOperator (const std::vector<std::string>& args);

    AlignmentFilterRegexOperator () : AlignmentFilterUnaryOperator<std::string>(), _reg(0)  {}

    ~AlignmentFilterRegexOperator ();

protected:
    boost::regex* _reg;
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
        AlignmentFilter_##name (const char* op, ...) \
        {   va_list ap;   va_start(ap, op);   configure (op,ap);   va_end(ap);   } \
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
                case IN_RANGE:      return ((exp) >= _values[0])  &&  ((exp) <= _values[1]) ; \
                case NOT_IN_RANGE:  return ((exp) <  _values[0])  ||  ((exp) >  _values[1]) ; \
                default:    return false; \
            } \
        } \
        IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilter_##name (args); } \
        std::string getName() {  return std::string(tag); } \
    };

/********************************************************************************/

#define DEFINE_ALIGNMENT_REGEXP_FILTER(tag,name,regexp)  \
    class AlignmentFilter_##name : public AlignmentFilterRegexOperator   \
    { \
    public: \
        AlignmentFilter_##name ()   {} \
        AlignmentFilter_##name (const std::vector<std::string>& args) : AlignmentFilterRegexOperator (args)  {} \
        bool isOk (const core::Alignment& a) const { \
            switch(_operator) \
            { \
                case HOLD:      return boost::regex_search (regexp,*_reg) == true;\
                case NO_HOLD:   return boost::regex_search (regexp,*_reg) == false;\
                default:        return false; \
            } \
        } \
        IAlignmentFilter* clone (const std::vector<std::string>& args) { return new AlignmentFilter_##name (args); } \
        std::string getName() {  return std::string(tag); } \
    };


/********************************************************************************/

/** Now, we define a bunch of filter classes */
DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit from",            HitFrom,            int32_t,  1+a.getRange(alignment::core::Alignment::SUBJECT).begin);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit to",              HitTo,              int32_t,  1+a.getRange(alignment::core::Alignment::SUBJECT).end);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query from",          QueryFrom,          int32_t,  1+a.getRange(alignment::core::Alignment::QUERY).begin);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query to",            QueryTo,            int32_t,  1+a.getRange(alignment::core::Alignment::QUERY).end);

DEFINE_ALIGNMENT_EXP_FILTER ("HSP alignment length",    Length,             u_int32_t,  a.getLength());

DEFINE_ALIGNMENT_EXP_FILTER ("HSP E­‐Value",             Evalue,             double,     a.getEvalue());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP bit score",           Bitscore,           double,     a.getBitScore());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP score",               Score,              u_int16_t,  a.getScore());

DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of identities",     NbIdentities,       u_int32_t,  a.getNbIdentities());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of identities",     PercentIdentities,  double,     a.getPercentIdentities() * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of positive",       NbPositives,        u_int32_t,  a.getNbPositives());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of positives",      PercentPositives,   double,     a.getPercentPositives() * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of misses",         NbMissses,          u_int32_t,  a.getNbMisses());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of misses",         PercentMisses,      double,     a.getPercentMisses() * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("HSP # of gaps",           NbGaps,             u_int32_t,  a.getNbGaps());
DEFINE_ALIGNMENT_EXP_FILTER ("HSP % of gaps",           PercentGaps,        double,     a.getPercentGaps()   * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("Query Coverage",          QueryCoverage,      double,     a.getCoverage(alignment::core::Alignment::QUERY)       * 100.0);
DEFINE_ALIGNMENT_EXP_FILTER ("Hit Coverage",            SubjectCoverage,    double,     a.getCoverage(alignment::core::Alignment::SUBJECT)       * 100.0);

DEFINE_ALIGNMENT_EXP_FILTER ("Hit Length",              HitLength,          u_int32_t,  a.getSequence(alignment::core::Alignment::SUBJECT)->getLength() );

DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit gaps",            HitsGaps,           u_int32_t,  a.getNbGaps(alignment::core::Alignment::SUBJECT));
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query gaps",          QueryGaps,          u_int32_t,  a.getNbGaps(alignment::core::Alignment::QUERY));

/** IMPORTANT ! since we use stringstream, we must use other type than char, otherwise a string holding one digit (like "1") will
 *  be read as its ASCII code (ie. 49 for "1") and won't have the desired value. The trick is so to use a type that will not be
 *  interpreted as ASCII string.
 */
DEFINE_ALIGNMENT_EXP_FILTER ("HSP hit frame",           HitFrame,           int16_t,    a.getFrame (alignment::core::Alignment::SUBJECT));
DEFINE_ALIGNMENT_EXP_FILTER ("HSP query frame",         QueryFrame,         int16_t,    a.getFrame (alignment::core::Alignment::QUERY));

DEFINE_ALIGNMENT_EXP_FILTER ("Number of HSPs",          HSPNumber,          u_int32_t,  a.getAlignProgress().number);
DEFINE_ALIGNMENT_EXP_FILTER ("HSP rank",                HSPRank,            u_int32_t,  a.getAlignProgress().rank);

DEFINE_ALIGNMENT_EXP_FILTER ("Number of hits",          HitsNumber,         u_int32_t,  a.getSbjProgress().number);
DEFINE_ALIGNMENT_EXP_FILTER ("Hit rank",                HitRank,            u_int32_t,  a.getSbjProgress().rank);

DEFINE_ALIGNMENT_EXP_FILTER ("Number of queries",       QueryNumber,        u_int32_t,  a.getQryProgress().number);
DEFINE_ALIGNMENT_EXP_FILTER ("Query rank",              QueryRank,          u_int32_t,  a.getQryProgress().rank);

DEFINE_ALIGNMENT_REGEXP_FILTER ("Query definition",     QueryDefinition,    a.getSequence(alignment::core::Alignment::QUERY)->comment);
DEFINE_ALIGNMENT_REGEXP_FILTER ("Hit definition",       HitDefinition,      a.getSequence(alignment::core::Alignment::SUBJECT)->comment);

DEFINE_ALIGNMENT_REGEXP_FILTER ("Query identifier",     QueryIdentifier,    a.getSequence(alignment::core::Alignment::QUERY)->getIdentifier());
DEFINE_ALIGNMENT_REGEXP_FILTER ("Hit identifier",       HitIdentifier,      a.getSequence(alignment::core::Alignment::SUBJECT)->getIdentifier());

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _OPERATOR_ALIGNMENT_FILTER_HPP_ */
