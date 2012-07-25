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

#include "JniObsfucation.h"
#include "org_inria_genscale_dbscan_impl_plast_RequestResult.h"

#include <launcher/jni/Helper.hpp>

#include <alignment/core/api/IAlignmentContainer.hpp>
#include <alignment/core/api/IAlignmentContainerModel.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;
using namespace dp;
using namespace database;
using namespace launcher::jni;
using namespace alignment::core;

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT jobject JNICALL Java_org_inria_genscale_dbscan_impl_plast_RequestResult_retrieveNext (
    JNIEnv* env,
    jobject obj,
    jlong   peer,
    jobject factory
)
{
    jobject result = 0;
    bool    isOk   = true;

    IRootLevel*      self        = 0;
    IQueryLevel*     currentItem = 0;
    const ISequence* sequence    = 0;

    DEBUG (("[C++] RequestResult_retrieveNext: obj=%p  peer=%p \n", obj, peer));

    /** We recover the actual C++ type of the peer. */
    if (isOk)
    {
        self = (IRootLevel*) peer;
        isOk = self && !self->isDone();
    }

    /** We retrieve the current item. */
    if (isOk)
    {
        currentItem = self->currentItem();
        isOk = (currentItem != 0);
    }

    /** We retrieve the sequence. */
    if (isOk)
    {
        sequence = currentItem->getSequence();
        isOk = (sequence != 0);
    }

    if (isOk)
    {
        /** We initialize the iterator. */
        currentItem->first();

        /** We retrieve information about the sequence. */
        char bufId[256];    size_t lenId  = sizeof(bufId);
        char bufDef[1024];  size_t lenDef = sizeof(bufDef);
        sequence->retrieveIdAndDefinition (bufId, lenId, bufDef, lenDef);

        /** We create a ISequence from the factory. */
        jobject seq = env->CallObjectMethod (
            factory,
            METHOD (IObjectFactory,createSequence),
            (jstring)env->NewStringUTF (bufId),
            (jstring)env->NewStringUTF (bufDef),
            (jint)sequence->getLength(),
            (jstring)env->NewStringUTF ("")
        );

        /** We build the current object. */
        result = env->NewObject (
            CLASS (QueryResult),
            METHOD (QueryResult,init),
            (jlong)currentItem,
            (jobject)factory,
            (jint)currentItem->size(),
            (jlong)seq
        );

        /** We go one step further in the iteration. */
        if (self->next () == dp::ITER_DONE)
        {
            /** We invalidate the peer in the java object. */
            env->CallObjectMethod (
				obj, 
				METHOD (PeerIterator,setPeer),
				(jlong)0
			);
        }
    }

    /** We return the created instance. */
    return result;
}

