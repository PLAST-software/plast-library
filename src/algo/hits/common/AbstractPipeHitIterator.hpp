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

/** \file AbstractPipeHitIterator.hpp
 *  \brief Abstract implementation of IHitIterator interface
 *  \date 07/11/2011
 *  \author edrezen
 */

#ifndef _ABSTRACT_PIPE_HITITERATOR_HPP_
#define _ABSTRACT_PIPE_HITITERATOR_HPP_

/********************************************************************************/

#include <algo/hits/common/AbstractHitIterator.hpp>

#include <seed/api/ISeedModel.hpp>

#include <algo/core/api/IScoreMatrix.hpp>
#include <algo/core/api/IAlgoParameters.hpp>

#include <algo/align/api/IAlignmentResult.hpp>

/********************************************************************************/
namespace algo   {
namespace hits   {
/** \brief Abstract implementations for IHitIterator interface. */
namespace common {
/********************************************************************************/

/** \brief  Abstract implementation of IHitIterator interface for linking hits iterators.
 *
 * This class implements effectively the iterate() method of the Iterator interface; the other
 * way of iteration (first/isDone/next) is implemented as a null implementation since we prefer
 * to use iterate() for optimization issues (less methods call).
 *
 * This implementation provides the interface allowing to use hits iterators like unix pipes.
 * This is done through a callback mechanism: the iterate() implementation calls the iterate
 * method on the provided source iterator with a iterateMethod() callback that will be called
 * each time the source iterator iterates a hit.
 *
 * In other word, when we call iterate() on an instance 'it' having a source iterator,
 * it will call iterate() on the source iterator that will chain its iteration with the 'it'
 * instance. This mechanism can be recursive which allows to have a unix pipe behaviour.
 *
 * This implementation is still abstract. Now, the concrete iteration work has not to be implemented
 * in iterate() but in iterateMethod(). One can see that iterate() is now a Template Method (see
 * GOF[94]) with iterateMethod() as primitive.
 *
 * This class also provides an implementation for the split() method since we have
 * to take into account our pipe mechanism. This is done by splitting the source iterator in N
 * parts and then cloning the 'this' instance N times with a source instance being one of the N
 * source-split iterators.
 */
class AbstractPipeHitIterator : public AbstractHitIterator
{
public:

    /** Constructor.
     * \param[in]  sourceIterator : iterator to be used as source for the current one.
     * \param[in]  model          : seed model
     * \param[in]  scoreMatrix    : score matrix for score based algorithm pieces
     * \param[in]  parameters     : used for configuration of instances
     * \param[out] ungapResult    : list of ungap alignments filled during algorithm execution
     */
    AbstractPipeHitIterator (
        IHitIterator*                   sourceIterator,
        ::seed::ISeedModel*             model,
        algo::core::IScoreMatrix*       scoreMatrix,
        algo::core::IParameters*        parameters,
        algo::align::IAlignmentResult* ungapResult
    );

    /** Destructor. */
    virtual ~AbstractPipeHitIterator ();

    /** Null implementation. */
    void first() {}

    /** Null implementation. */
    dp::IteratorStatus next() { return dp::ITER_UNKNOWN; }

    /** Null implementation. In particular, says that the iteration is always finished.
     * \return always true.
     */
    bool isDone() { return true; }

    /** Null implementation.
     * \return always NULL
     */
    Hit* currentItem() { return 0; }

    /** \copydoc AbstractHitIterator::iterate
     * Implemented as Template Method with primitive iterateMethod()
     */
    void iterate (void* aClient, Method method);

    /** \copydoc AbstractHitIterator::split
     * Implemented as Template Method with primitive clone()
     */
    std::vector<IHitIterator*> split (size_t nbSplit);

    /** \copydoc AbstractHitIterator::getSourceIterator */
    IHitIterator* getSourceIterator ()  { return _sourceIterator; }

protected:

    /** The source iterator. */
    algo::hits::IHitIterator*       _sourceIterator;

    /** The seed model. */
    ::seed::ISeedModel*             _model;

    /** The score matrix (used of parts computing similarity scores). */
    algo::core::IScoreMatrix*       _scoreMatrix;

    /** Parameters for various configurations. */
    algo::core::IParameters*        _parameters;

    /** Ungap alignments list to be filled throughout the algorithm execution. */
    algo::align::IAlignmentResult* _ungapResult;

    /* Shortcuts. */
    int8_t** _matrix;
    int8_t*  _matrixAsVector;
    size_t   _span;

    /** Reference on a client to be called back during source iteration. */
    IteratorClient*  _client;

    /** Reference on the iteration callback . */
    Method           _method;

    /** Primitive of template method 'iterate'.
     * \param[in] hit : the current hit from the source iterator.
     */
    virtual void iterateMethod  (Hit* hit) = 0;

    /** Primitive of template method 'split'.
     * \param[in] sourceIterator : the source iterator to be used as initial set of hits
     * \return the cloned instance
     */
    virtual AbstractPipeHitIterator* clone (IHitIterator* sourceIterator) = 0;

    /* Statistics. */
    u_int64_t _scoreOK;
    u_int64_t _scoreKO;
};

/********************************************************************************/
}}} /* end of namespaces. */
/********************************************************************************/

#endif /* _ABSTRACT_PIPE_HITITERATOR_HPP_ */
