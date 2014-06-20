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

/** \file BlastdbAsn1HeaderDecoder.hpp
 *  \brief Asn1 decoder for the header comment the Blast db format
 *  \date 26/03/2014
 *  \author sbrillet
 *
 *  Define a decoder to decode the header comment of the Blast format
 */

#ifndef _BLASTDB_ASN1_HEADER_DECODER_HPP_
#define _BLASTDB_ASN1_HEADER_DECODER_HPP_

/********************************************************************************/

#include <misc/api/types.hpp>
#include <string.h>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {
/********************************************************************************/

/** \brief Implementation of an interface to decode the comment of the the blast header
  */
class BlastdbAsn1HeaderDecoder
{
public:

	/** Constructor.
     */
	BlastdbAsn1HeaderDecoder (size_t _commentMaxSize);

    /** Destructor. */
    virtual ~BlastdbAsn1HeaderDecoder  ();

    /** get the seqid decoded string thanks to the asn structure.
     *  \param[in] ptr : pointer on the header file data
     *  \param[in] size : header data size to read (maximum value)
     *  \return decoded string of the seqId
     */
    std::string getDecodeSeqid(const char *ptr, u_int32_t size);

private:
	typedef struct _ASNINFO {
		int32_t         remaining;
		unsigned char 	*ptr;
		char          	*name;
		int32_t      	optional;
		std::string 	decodedString;
	} ASNINFO;

	typedef void (BlastdbAsn1HeaderDecoder::*asnParserFn)(ASNINFO *asn);

	typedef struct _ASN_TABLE
	{
		char        *name;
		BlastdbAsn1HeaderDecoder::asnParserFn  parser;
		int32_t      optional; // 0 mandatory, 1 optional, 2 not decoded
	} ASN_TABLE;

	size_t _commentMaxSize;
	bool	_removeLastCar;

    unsigned char get(ASNINFO *asn);

	int32_t accept(ASNINFO *asn, const unsigned char *str, int32_t len);

	int32_t expect(ASNINFO *asn, const unsigned char *str, int32_t len);

	void parseVisibleString(ASNINFO *asn);

	void parseInteger(ASNINFO *asn);

	void parseChoice(ASNINFO *asn, const ASN_TABLE *table);

	void parseSequence(ASNINFO *asn, const ASN_TABLE *table);

	void parseSequenceOf(ASNINFO *asn, const asnParserFn parser);

	void parseDateStd(ASNINFO *asn);

	void parseDate(ASNINFO *asn);

	void parseObjectId(ASNINFO *asn);

	void parseGiimportId(ASNINFO *asn);

	void parseTextseqId(ASNINFO *asn);

	void parseIdPatNumber(ASNINFO *asn);

	void parseIdPat(ASNINFO *asn);

	void parsePatentSeqId(ASNINFO *asn);

	void parseDbtag(ASNINFO *asn);

	void parsePdbSeqId(ASNINFO *asn);

	void parseSeqId(ASNINFO *asn);

};

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

#endif /* _BLASTDB_ASN1_HEADER_DECODER_HPP_  */
