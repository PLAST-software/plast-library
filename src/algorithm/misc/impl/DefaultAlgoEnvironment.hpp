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

#ifndef _DEFAULT_ALGO_ENVIRONMENT_HPP_
#define _DEFAULT_ALGO_ENVIRONMENT_HPP_

/********************************************************************************/

#include "IAlgoEnvironment.hpp"

/********************************************************************************/
namespace algo  {
/********************************************************************************/

class DefaultEnvironment : public IEnvironment
{
public:

    /** */
    virtual ~DefaultEnvironment ()  {}

    /** */
    static DefaultEnvironment& singleton ()  { static DefaultEnvironment instance;  return instance; }

    /** */
    IConfiguration* createConfiguration (dp::IProperties* properties);

    /** */
    void run (dp::IProperties* properties);

protected:

    /** */
    IAlgorithm* createAlgorithm (
        IConfiguration*                 config,
        database::IDatabaseQuickReader* reader,
        IParameters*                    params,
        AlignmentResultVisitor*         resultVisitor
    );

    /** Implementation of IObserver interface. */
    void update (dp::EventInfo* evt, dp::ISubject* subject);

    /** */
    std::vector <std::pair <Range,Range> > buildUri (
        database::IDatabaseQuickReader* subjectReader,
        database::IDatabaseQuickReader* queryReader
    );

    std::vector<IParameters*> createParametersList (
        IConfiguration* config,
        dp::IProperties* properties,
        std::vector <std::pair <Range,Range> >& uri
    );
};

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/

#endif /* _DEFAULT_ALGO_ENVIRONMENT_HPP_ */
