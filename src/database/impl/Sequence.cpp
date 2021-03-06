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

#include <database/api/ISequence.hpp>
#include <database/api/ISequenceDatabase.hpp>
#include <database/impl/DatabaseVisitorComment.hpp>

using namespace database::impl;

namespace database {
std::string ISequence::getComment() const
{

	if (database!=0)
	{
		std::string commentDest;
		DatabaseVisitorComment v(*this,commentDest);
		database->accept(v);
		return commentDest;
	}
	else
		return comment;
}
}
