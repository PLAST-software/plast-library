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
