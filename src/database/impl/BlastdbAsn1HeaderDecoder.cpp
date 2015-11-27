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

#include <misc/api/PlastStrings.hpp>
#include <database/impl/BlastdbAsn1HeaderDecoder.hpp>
#include <misc/api/macros.hpp>

#include <stdarg.h>

#define DEBUG(a)  //printf a

using namespace std;


/********************************************************************************/
namespace database { namespace impl {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbAsn1HeaderDecoder::BlastdbAsn1HeaderDecoder (size_t maxSize)
:_commentMaxSize(maxSize),_removeLastCar(false)
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
BlastdbAsn1HeaderDecoder::~BlastdbAsn1HeaderDecoder  ()
{
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
string BlastdbAsn1HeaderDecoder::getDecodeSeqid(const char *ptr, u_int32_t size)
{
	ASNINFO  asn;
	std::string	tempComment;

	asn.ptr       		= (unsigned char*)ptr;
	asn.remaining 		= size;
	asn.name      		= (char*)"";
	asn.decodedString 	= "";

	// permits to manage the formatdb -o T case for the string
	if (asn.ptr[0] == 0x1A)
		parseVisibleString(&asn);
	else
		parseSequenceOf(&asn, &BlastdbAsn1HeaderDecoder::parseSeqId);
    return asn.decodedString + " ";
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
unsigned char BlastdbAsn1HeaderDecoder::get(ASNINFO *asn)
{
	unsigned char t;

	if (asn->remaining <= 0)
	{
		throw MSG_FILE_BLAST_MSG3;
	}

	t = *asn->ptr++;
	--asn->remaining;

	return t;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int32_t BlastdbAsn1HeaderDecoder::accept(ASNINFO *asn, const unsigned char *str, int32_t len)
{
	int32_t i;
	unsigned char *t;

	/* if there are not enough buffer left, dont even try */
	if (asn->remaining < len) return 0;

	t = asn->ptr;
	for (i = 0; i < len; ++i)
	{
		if (*t++ != *str++) break;
	}

	/* check for a match */
	if (i == len)
	{
		asn->ptr += len;
		asn->remaining -= len;
	}

	return (i == len);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
int32_t BlastdbAsn1HeaderDecoder::expect(ASNINFO *asn, const unsigned char *str, int32_t len)
{
	asn->ptr += len;
	asn->remaining -= len;

	return 1;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseVisibleString(ASNINFO *asn)
{
	int32_t i, s;
	int32_t len;

	unsigned char c;

	/* make sure we are dealing with a visible string which start
	 * with a 0x1a followed by the length of the string, then the
	 * string.  NOTE: the strings are NOT zero terminated.
	 */
	expect(asn, (unsigned char *) "\x1a", 1);

	/* parse the length.  if the most significant bit is not set, i.e. the
	 * length is less than 128, then the length of the string is encoded in
	 * the single byte.  otherwise, the 7 least significant bits encode the
	 * number of bytes used to encode the length of the string.
	 */
	c = get(asn);
	if (c < 128)
	{
		len = c;
	}
	else
	{
		/* figure out how many bytes the length takes up */
		s = c - 128;
		if (s > (int32_t)sizeof(len))
		{
			throw MSG_FILE_BLAST_MSG3;
		}

		/* read in the length one byte at a time, most significant bytes
		 * are first. */
		len = 0;
		for (i = 0; i < s; ++i)
		{
			c = get(asn);
			len = (len << 8) + (u_int32_t)c;
		}
	}

	/* print the string */
	while ((len > 0)&&(asn->decodedString.size()<_commentMaxSize))
	{
		if (asn->optional==2)
			get(asn);
		else
			asn->decodedString += get(asn);
		--len;
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseInteger(ASNINFO *asn)
{
	int32_t i;
	int32_t len;
	int32_t value;
	char tempValue[10];

	unsigned char c;

	/* make sure we are dealing with an integer.  integers start
	 * with a 0x02 followed by the integers length.
	 */
	expect(asn, (unsigned char *) "\x02", 1);

	len = get(asn);
	if (len > (int32_t)sizeof(value))
	{
		throw MSG_FILE_BLAST_MSG3;
	}

	/* most significat bytes first */
	value = 0;
	for (i = 0; i < len; ++i)
	{
		c = get(asn);
		value = (value << 8) + (unsigned int)c;
	}
	if (asn->optional!=2)
	{
		sprintf(tempValue,"%d", value);
		asn->decodedString.append(tempValue);
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseChoice(ASNINFO *asn, const ASN_TABLE *table)
{
	int32_t i;
	unsigned char hdr[2];

	char *name = asn->name;

	/* which choice is indicated by the first byte, 0xa0 for the
	 * first item, 0xa1 for the second, 0xa2 for the third, etc.
	 */
	hdr[0] = '\xa0';
	hdr[1] = '\x80';

	/* parse the entries of the choice.  go through the list of
	 * choices until we find a match or we hit the end of the
	 * list.
	 */
	for (i = 0; table[i].parser != NULL; ++i)
	{
		hdr[0] = 0xa0 + i;
		if (accept(asn, hdr, 2))
		{

			asn->decodedString.append(table[i].name);

			asn->name = table[i].name;
			asn->optional = table[i].optional;
			(this->*table[i].parser)(asn);
			asn->name = name;

			expect(asn, (unsigned char *) "\x00\x00", 2);
			break;
		}
	}
	//asn->decodedString.append("|");

	/* make sure we found a choice */
	if (table[i].parser == NULL)
	{
		throw MSG_FILE_BLAST_MSG3;
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseSequence(ASNINFO *asn, const ASN_TABLE *table)
{
	int32_t i;
	unsigned char hdr[2];

	char *name = asn->name;

	/* which item of the sequence is indicated by the first byte,
	 * 0xa0 for the first item, 0xa1 for the second, 0xa2 for the
	 * third, etc.
	 */
	hdr[0] = '\xa0';
	hdr[1] = '\x80';

	/* beginning of the sequence starts with a 0x3080 */
	expect(asn, (unsigned char *) "\x30\x80", 2);

	/* parse the entries of the sequence going from item to item.
	 * if the item is missing and it is NOT optional, through an
	 * error reporting the missing item.
	 */
	for (i = 0; table[i].parser != NULL; ++i)
	{
		hdr[0] = 0xa0 + i;
		if (accept(asn, hdr, 2))
		{
			asn->decodedString.append(table[i].name);

			asn->name = table[i].name;
			asn->optional = table[i].optional;
			(this->*table[i].parser)(asn);
			asn->name = name;

			expect(asn, (unsigned char *) "\x00\x00", 2);
		}
		else if (!table[i].optional)
		{
			throw MSG_FILE_BLAST_MSG3;
		}
	}
	//asn->decodedString.append("|");

	/* end if the sequence ends with 0x0000 */
	expect(asn, (unsigned char *) "\x00\x00", 2);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseSequenceOf(ASNINFO *asn, const asnParserFn parser)
{
	_removeLastCar = false;
	/* beginning of the sequence */
	expect(asn, (unsigned char *) "\x30\x80", 2);

	/* parse the entries of the sequence of.  the sequence of structure is
	 * terminated by two NULL bytes.
	 */
	while ((!accept(asn, (unsigned char *) "\x00\x00", 2))&&(asn->decodedString.size()<_commentMaxSize))
	{
		/* while not at the end of the list, call the designated parser */
		(this->*parser)(asn);
		if (!_removeLastCar)
			asn->decodedString.append("|");
	}
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseDateStd(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",        	&BlastdbAsn1HeaderDecoder::parseInteger,         0 }, //year
			{ (char*)"|",       	&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //month
			{ (char*)"|",        	&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //day
			{ (char*)"|",      		&BlastdbAsn1HeaderDecoder::parseVisibleString,   1 }, //season
			{ (char*)"|",        	&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //hour
			{ (char*)"|",      		&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //minute
			{ (char*)"|",      		&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //second
			{ 0,                	0,							                     0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseDate(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",         	&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //str
			{ (char*)"|",         	&BlastdbAsn1HeaderDecoder::parseDateStd,         0 }, //std
			{ 0,                    0,							                     0 },
	};

	parseChoice(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseObjectId(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"",          	&BlastdbAsn1HeaderDecoder::parseInteger,         0 }, //id
			{ (char*)"",         	&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //str
			{ 0,                    0, 							                     0 },
	};
	_removeLastCar = true;
	parseChoice(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseGiimportId(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",          	&BlastdbAsn1HeaderDecoder::parseInteger,         0 }, //id
			{ (char*)"|",	        &BlastdbAsn1HeaderDecoder::parseVisibleString,   1 }, //db
			{ (char*)"|",     		&BlastdbAsn1HeaderDecoder::parseVisibleString,   1 }, //release
			{ 0,                    0, 							                     0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseTextseqId(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",   	     	&BlastdbAsn1HeaderDecoder::parseVisibleString,   1 }, 		//name
			{ (char*)"|",	  			&BlastdbAsn1HeaderDecoder::parseVisibleString,   1 },		//accession
			{ (char*)"",		     	&BlastdbAsn1HeaderDecoder::parseVisibleString,   2 },		//release
			{ (char*)".",		     	&BlastdbAsn1HeaderDecoder::parseInteger,         1 },		//version
			{ 0,                		0,                                               0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseIdPatNumber(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",      		&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //number
			{ (char*)"|",  			&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //app-number
			{ 0,                    0, 							                     0 },
	};

	parseChoice(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseIdPat(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",     		&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //country
			{ (char*)"|",          	&BlastdbAsn1HeaderDecoder::parseIdPatNumber,     0 }, //id
			{ (char*)"|",    		&BlastdbAsn1HeaderDecoder::parseVisibleString,   1 }, //doc-type
			{ 0,                    0, 								                 0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parsePatentSeqId(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|",       		&BlastdbAsn1HeaderDecoder::parseInteger,         0 }, //seqid
			{ (char*)"|",        	&BlastdbAsn1HeaderDecoder::parseIdPat,           0 }, //cit
			{ 0,                    0, 							                     0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseDbtag(ASNINFO *asn)
{
	const ASN_TABLE table[] = {
			{ (char*)"|", 	        &BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //db
			{ (char*)"|",    	    &BlastdbAsn1HeaderDecoder::parseObjectId,        0 }, //tag
			{ 0,                    0,							                     0 },
	};

	parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parsePdbSeqId(ASNINFO *asn)
{
  const ASN_TABLE table[] = {
    { (char*)"|",         	&BlastdbAsn1HeaderDecoder::parseVisibleString,   0 }, //mol
    { (char*)"|",       	&BlastdbAsn1HeaderDecoder::parseInteger,         1 }, //chain
    { (char*)"|",         	&BlastdbAsn1HeaderDecoder::parseDate,            1 }, //rel
    { 0,                    0,  							                 0 },
  };

  parseSequence(asn, table);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void BlastdbAsn1HeaderDecoder::parseSeqId(ASNINFO *asn)
{
  const ASN_TABLE table[] = {
    { (char*)"",           			&BlastdbAsn1HeaderDecoder::parseObjectId,        0 }, 	//local
    { (char*)"bbs|",                &BlastdbAsn1HeaderDecoder::parseInteger,         0 },
    { (char*)"gibbmt|",             &BlastdbAsn1HeaderDecoder::parseInteger,         0 },
    { (char*)"giim",	            &BlastdbAsn1HeaderDecoder::parseGiimportId,      0 },
    { (char*)"gb",	     		    &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"embl",	            &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"pir",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"sp",           		&BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"pat", 	            &BlastdbAsn1HeaderDecoder::parsePatentSeqId,     0 },
    { (char*)"ref",               	&BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"gnl",                 &BlastdbAsn1HeaderDecoder::parseDbtag,           0 },
    { (char*)"gi|",                 &BlastdbAsn1HeaderDecoder::parseInteger,         0 },
    { (char*)"dbj",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"prf",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"pdb",                 &BlastdbAsn1HeaderDecoder::parsePdbSeqId,        0 },
    { (char*)"tpg",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"tpe",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"tpd",                 &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"gpipe",               &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { (char*)"named-annot",         &BlastdbAsn1HeaderDecoder::parseTextseqId,       0 },
    { 0,                    	    0,                   							 0 },
  };

  parseChoice(asn, table);
}

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/

