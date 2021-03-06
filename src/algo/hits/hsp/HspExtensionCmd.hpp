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

/** \file HspExtensionCmd.hpp
 *  \brief
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _HSP_EXTENSION_CMD_HPP_
#define _HSP_EXTENSION_CMD_HPP_

/********************************************************************************/

#include <designpattern/api/ICommand.hpp>
#include <designpattern/impl/Observer.hpp>

#include <database/api/ISequenceDatabase.hpp>

#include <alignment/core/api/IHspContainer.hpp>
#include <alignment/core/api/IAlignmentContainer.hpp>

#include <alignment/tools/api/ISemiGappedAlign.hpp>

#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/stats/api/IStatistics.hpp>

/********************************************************************************/
namespace algo {
namespace hits {
namespace hsp  {
/********************************************************************************/

class HspExtensionCmd : public dp::ICommand, public dp::impl::Subject
{
public:

    /** */
    HspExtensionCmd (
        database::ISequenceDatabase*            db1,
        database::ISequenceDatabase*            db2,
        statistics::IQueryInformation*          queryInfo,
        alignment::core::IHspContainer*         hspContainer,
        alignment::core::IHspContainer*         hspContainer2,
        alignment::tools::ISemiGapAlign*        dynapro,
        algo::core::IParameters*                params,
        dp::IObserver* 							observer
    );

    /** */
    ~HspExtensionCmd ();

    /** */
    void execute ();

private:

    database::ISequenceDatabase* _db1;
    void setDb1 (database::ISequenceDatabase* db1)  { SP_SETATTR(db1); }

    database::ISequenceDatabase* _db2;
    void setDb2 (database::ISequenceDatabase* db2)  { SP_SETATTR(db2); }

    statistics::IQueryInformation* _queryInfo;
    void setQueryInfo (statistics::IQueryInformation* queryInfo)  { SP_SETATTR(queryInfo); }

    alignment::core::IHspContainer* _hspContainer;
    void setHspContainer (alignment::core::IHspContainer* hspContainer)  { SP_SETATTR(hspContainer); }

    alignment::core::IHspContainer* _hspContainer2;
    void setHspContainer2 (alignment::core::IHspContainer* hspContainer2)  { SP_SETATTR(hspContainer2); }

    alignment::tools::ISemiGapAlign* _dynapro;
    void setDynapro (alignment::tools::ISemiGapAlign* dynapro)  { SP_SETATTR(dynapro); }

    algo::core::IParameters* _parameters;
    void setParameters (algo::core::IParameters* parameters)  { SP_SETATTR (parameters); }
};

/********************************************************************************/
}}}; /* end of namespaces. */
/********************************************************************************/

#endif /* _HSP_EXTENSION_CMD_HPP_ */
