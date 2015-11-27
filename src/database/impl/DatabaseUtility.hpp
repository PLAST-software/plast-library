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

/** \file DatabaseUtility.hpp
 *  \brief Utility class to implement some function to help on the database reader
 *  \date 27/03/2014
 *  \author sbrillet
 *
 *  Define some utility function for find the database type
 */

#ifndef _DATABASE_UTILITY_HPP_
#define _DATABASE_UTILITY_HPP_

/********************************************************************************/

#include <os/api/IFile.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <os/impl/CommonOsImpl.hpp>
#include <designpattern/impl/TokenizerIterator.hpp>


/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of function to find the database type
  */
class DatabaseLookupType
{
public:
   /** */
	enum QuickReaderType_e
	{
		ENUM_BLAST_PAL,
		ENUM_BLAST_NAL,
		ENUM_BLAST_PIN,
		ENUM_BLAST_NIN,
		ENUM_FASTA_PAL,
		ENUM_FASTA_NAL,
		ENUM_FASTA_FA,
		ENUM_TYPE_UNKNOWN
	};

	/** */

	static QuickReaderType_e quickReaderType (const std::string& filename)

	{
		std::string	Extension;
		size_t foundPoint;

		dp::impl::TokenizerIterator tokenizer (filename.c_str(), ",");
		tokenizer.first();
		foundPoint = std::string(tokenizer.currentItem()).find_last_of(".");
		if (foundPoint!=std::string::npos)
		{
			Extension = std::string(tokenizer.currentItem()).substr(foundPoint, std::string(tokenizer.currentItem()).size());
			if (Extension==".pal") return ENUM_BLAST_PAL;
			if (Extension==".nal") return ENUM_BLAST_NAL;
			if (Extension==".pin") return ENUM_BLAST_PIN;
			if (Extension==".nin") return ENUM_BLAST_NIN;
			if (Extension==".fa") return ENUM_FASTA_FA;
		}

		//return ENUM_TYPE_UNKNOWN;
		return ENUM_FASTA_FA;
    }


};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _DATABASE_UTILITY_HPP_  */
