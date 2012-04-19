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

/** \file AlignmentResultVisitors.hpp
 *  \brief A few implementations of IAlignmentResultVisitor interface.
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _OSTREAM_ALIGNMENT_CONTAINER_VISITOR_HPP_
#define _OSTREAM_ALIGNMENT_CONTAINER_VISITOR_HPP_

/********************************************************************************/

#include <alignment/visitors/impl/HierarchyAlignmentVisitor.hpp>

#include <iostream>
#include <fstream>

/********************************************************************************/
namespace alignment {
namespace visitors  {
namespace impl      {
/********************************************************************************/

/********************************************************************************/
/** \brief Abstract implementation of IAlignmentContainerVisitor with output stream management
 *
 * This is a factorization class that knows how to open/close/write a file. Note that we use
 * the operating system abstraction layer IFile for managing the file.
 *
 * It is intended to be subclassed by file dump visitors.
 */
class OstreamVisitor : public HierarchyAlignmentResultVisitor
{
public:

    /** Constructor.
     * \param[in] uri : path of the file to be used.
     */
    OstreamVisitor (std::ostream* ostream) : _ostream(ostream), _file(0)  {}

    /** Constructor.
     * \param[in] uri : path of the file to be used.
     */
    OstreamVisitor (const std::string& uri) : _ostream (0), _file (new std::ofstream())
    {
        _file->open (uri.c_str());
        _ostream = _file;
    }

    /** Destructor. */
    virtual ~OstreamVisitor()
    {
        if (_file != 0)
        {
            _file->close();
            delete _file;
        }
    }

    /** \copydoc IAlignmentResultVisitor::finish */
    void postVisit (core::IAlignmentContainer* result)
    {
        /** We should flush the stream. */
        getStream().flush ();
    }

    /** \copydoc IAlignmentResultVisitor::getPosition */
    u_int64_t getPosition ()  { return getStream().tellp(); }

protected:

    /** */
    std::ostream& getStream ()  { return *_ostream; }

private:

    /** Reference on the output stream. */
    std::ostream* _ostream;

    /** */
    std::ofstream* _file;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _OSTREAM_ALIGNMENT_CONTAINER_VISITOR_HPP_ */
