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

#include "AbstractScoredHitIterator.hpp"

#include "ITime.hpp"
#include "DefaultOsFactory.hpp"

using namespace std;
using namespace os;
using namespace database;
using namespace seed;
using namespace indexation;

#include <stdio.h>
#define DEBUG(a)  //printf a

/********************************************************************************/
namespace algo  {
/********************************************************************************/

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractScoredHitIterator::AbstractScoredHitIterator (
    indexation::IHitIterator*   realIterator,
    seed::ISeedModel*           model,
    algo::IScoreMatrix*         scoreMatrix,
    algo::IParameters*          parameters,
    algo::IAlignmentResult*     ungapResult
)
    : AbstractHitIterator (1,1),
      _realIterator (realIterator), _model(model), _scoreMatrix(scoreMatrix),  _parameters(parameters), _ungapResult(ungapResult),
      _matrix(0), _matrixAsVector(0),
      _client(0), _method(0),
      _scoreOK(0),_scoreKO(0)
{
    if (_realIterator)  { _realIterator->use ();    }
    if (_model)         { _model->use ();           }
    if (_scoreMatrix)   { _scoreMatrix->use ();     }
    if (_parameters)    { _parameters->use ();      }
    if (_ungapResult)   { _ungapResult->use ();     }

    /** Some little shortcuts. */
    if (_scoreMatrix)
    {
        _matrix         = _scoreMatrix->getMatrix();
        _matrixAsVector = _scoreMatrix->getMatrixAsVector();
    }

    if (_model) {  _span   = _model->getSpan();  }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
AbstractScoredHitIterator::~AbstractScoredHitIterator ()
{
    if (_realIterator)  { _realIterator->forget ();    }
    if (_model)         { _model->forget ();           }
    if (_scoreMatrix)   { _scoreMatrix->forget ();     }
    if (_parameters)    { _parameters->forget ();      }
    if (_ungapResult)   { _ungapResult->forget ();     }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
std::vector<indexation::IHitIterator*> AbstractScoredHitIterator::split (size_t nbSplit)
{
    std::vector<indexation::IHitIterator*> result;

    /** We split the real iterator. */
    std::vector<indexation::IHitIterator*> realSplit = _realIterator->split (nbSplit);

    /** We create (for each real split) a new UngapAlignement iterator. */
    for (size_t i=0; i<realSplit.size(); i++)
    {
        /** We add a clone to the result. */
        result.push_back (this->clone (realSplit[i]));
    }

    /** We have to link the created instances to the creator instance. */
    this->setSplitIterators (result);

    /** We return the result. */
    return result;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
void AbstractScoredHitIterator::iterate (void* client, Method method)
{
    DEBUG (("AbstractScoredHitIterator::iterate \n"));

    /** We iterate the real subject. */
    if (_realIterator != 0)
    {
        /** We memorize the client and its callback. */
        _client = (IteratorClient*)client;
        _method = method;

        /** We reset the number of iterations. */
        _inputHitsNumber  = 0;
        _outputHitsNumber = 0;

        /** We iterate the real iterator. */
        _realIterator->iterate (this, (Method) & AbstractScoredHitIterator::iterateMethod);
    }
}

/********************************************************************************/
} /* end of namespaces. */
/********************************************************************************/
