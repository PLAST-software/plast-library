/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.0, released May 2015                                          *
 *   Copyright (c) 2015                                                      *
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

/** \file 	FakeDatabaseNucleotideIndex.hpp
 *  \specific implementation of interfaces for close homologs nucleic database indexation.
 *  \date 	05/05/2015
 *  \author lantin
 */

#include <index/impl/AbstractDatabaseIndex.hpp>

/********************************************************************************/
namespace indexation {
/** \specific implementation of interfaces for close homologs nucleic database indexation. */
namespace impl {
/********************************************************************************/

/** \specific implementation of interfaces for close homologs nucleic database indexation.
 *
 * This implementation of the DatabaseIndex is a fake one. It will not really
 * create an index but just create a seed mask to pass it to the two indexes which
 * will index the query and the subject.
 *
 * The aim of this mask is to keep only some seeds to optimize a plastN comparison
 * against a close homologs databank (like ARN-16s ones - silva or rdp for instance)
 */
class FakeDatabaseNucleotideIndex : public AbstractDatabaseIndex
{
public:

	/** Constructor.
	 * \param[in] database : the database to be indexed
	 * \param[in] model : the seed model whose seeds are index keys
	 * */
    FakeDatabaseNucleotideIndex (database::ISequenceDatabase* database,
        seed::ISeedModel* model,
        std::string kmersBitsetPath);

	virtual ~FakeDatabaseNucleotideIndex ();

	/** \copydoc AbstractDatabaseIndex::build */
	void build ();

	/** the only data created by this fake index */
	u_int8_t* getMask ()  { return (u_int8_t*) _maskOut; }

	/** */
	typedef u_int64_t word_t;


	/*****************************************************************/
	/** 				NOT IMPLEMENTED METHODS 					**/
	/*****************************************************************/
	/** \copydoc AbstractDatabaseIndex::createOccurrenceIterator */
	IOccurrenceIterator* createOccurrenceIterator (const seed::ISeed* seed, size_t neighbourhoodSize=0) { return 0; }

	/** \copydoc AbstractDatabaseIndex::createOccurrenceBlockIterator */
	IOccurrenceBlockIterator* createOccurrenceBlockIterator (
	 const seed::ISeed* seed,
	 size_t neighbourhoodSize,
	 size_t blockSize
	)  { return 0; }

	/** \copydoc AbstractDatabaseIndex::getEntry */
	IndexEntry& getEntry (const seed::ISeed* seed);

	/** \copydoc AbstractDatabaseIndex::getOccurrenceNumber */
	size_t getOccurrenceNumber (const seed::ISeed* seed);

	/** \copydoc AbstractDatabaseIndex::getTotalOccurrenceNumber */
	u_int64_t getTotalOccurrenceNumber ();

	/** \copydoc AbstractDatabaseIndex::merge */
	void merge (void) {}

protected:

	word_t* _maskOut;
	size_t  _maskSize;
    std::string kmersBitsetPath;

}; // end of FakeDatabaseNucleotideIndex class

/********************************************************************************/
} } /* end of namespaces. */
/********************************************************************************/
