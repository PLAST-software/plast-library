#ifndef _FASTA_SEQUENCE_CONFITIONAL_PURE_ITERATOR_FACTORY_HPP_
#define _FASTA_SEQUENCE_CONFITIONAL_PURE_ITERATOR_FACTORY_HPP_


#include <algo/core/api/IAlgoParameters.hpp>

#include <database/impl/FastaSequencePureIterator.hpp>

#include <misc/api/types.hpp>

#include <iostream>

/********************************************************************************/
namespace database {
/** \brief Implementation of concepts related to genomic databases. */
namespace impl {

/**
 * Factory that creates a ISequenceIterator depending of its uri.
 *
 * For a specific uri, FastaSequencePureIterator instance is created. For every
 * other uri, FastaSequenceIterator is created.
 */
class FastaSequenceConditionalPureIteratorFactory : public ISequenceIteratorFactory
{
public:
    FastaSequenceConditionalPureIteratorFactory(algo::core::IParameters* params)
    : _uri(params->queryUri) {}

    /** \copydoc ISequenceIteratorFactory::createSequenceIterator */
    virtual ISequenceIterator* createSequenceIterator (const std::string& uri, const misc::Range64& range) {
        if (uri.compare(_uri) == 0) {
            return new FastaSequencePureIterator (uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end);
        }

        return new FastaSequenceIterator(uri.c_str(), SEQUENCE_MAX_COMMENT_SIZE, range.begin, range.end);
    }

private:
    const std::string& _uri;
};

} // impl
} // database

#endif /* _FASTA_SEQUENCE_CONFITIONAL_PURE_ITERATOR_FACTORY_HPP_ */
