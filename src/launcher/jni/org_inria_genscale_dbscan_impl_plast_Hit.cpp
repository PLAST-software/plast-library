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

#include "org_inria_genscale_dbscan_impl_plast_Hit.h"

#include <launcher/jni/Helper.hpp>

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
JNIEXPORT jobject JNICALL Java_org_inria_genscale_dbscan_impl_plast_Hit_retrieveNext (
    JNIEnv* env,
    jobject obj,
    jlong   peer,
    jobject factory,
    jobject querySeq,
    jobject hitSeq
)
{
    jobject result = 0;
    bool    isOk   = true;

    ISubjectLevel*   self        = 0;
    Alignment*       currentItem = 0;

    DEBUG (("[C++] Hit_retrieveNext: obj=%p  peer=%p \n", obj, peer));

    /** We recover the actual C++ type of the peer. */
    if (isOk)
    {
        self = (ISubjectLevel*) peer;
        isOk = self && !self->isDone();
    }

    /** We retrieve the current item. */
    if (isOk)
    {
        currentItem = self->currentItem();
        isOk = (currentItem != 0);
    }

    if (isOk)
    {
        /** We create a IHspInfo for the query. */
        jobject qryInfo = env->NewObject (ClassTable[HspInfo_e], MethodTable[HspInfo_init_e],
            (jobject) querySeq,
            (jint)   (currentItem->getRange    (Alignment::QUERY).begin + 1),
            (jint)   (currentItem->getRange    (Alignment::QUERY).end   + 1),
            (jint)   (currentItem->getNbGaps   (Alignment::QUERY)),
            (jint)   (currentItem->getFrame    (Alignment::QUERY)),
            (jdouble)(currentItem->getCoverage (Alignment::QUERY))
        );

        /** We create a IHspInfo for the query. */
        jobject hitInfo = env->NewObject (ClassTable[HspInfo_e], MethodTable[HspInfo_init_e],
            (jobject) hitSeq,
            (jint)   (currentItem->getRange    (Alignment::SUBJECT).begin + 1),
            (jint)   (currentItem->getRange    (Alignment::SUBJECT).end   + 1),
            (jint)   (currentItem->getNbGaps   (Alignment::SUBJECT)),
            (jint)   (currentItem->getFrame    (Alignment::SUBJECT)),
            (jdouble)(currentItem->getCoverage (Alignment::SUBJECT))
        );

        /** We create the Hsp instance through the factory. */
        result = env->CallObjectMethod (
            factory,
            MethodTable[IObjectFactory_createHsp_e],
            (jint)0, //num,
            (jdouble)currentItem->getBitScore(),
            (jdouble)currentItem->getScore(),
            (jdouble)currentItem->getEvalue(),
            (jint)currentItem->getNbIdentities(),
            (jint)currentItem->getNbPositives(),
            (jint)currentItem->getNbMisses(),
            (jint)currentItem->getLength(),
            (jobject)qryInfo,
            (jobject)hitInfo
        );

        /** We go one step further in the iteration. */
        if (self->next () == dp::ITER_DONE)
        {
            /** We may invalidate the peer in the java object. */
            env->CallObjectMethod (
				obj, 
				MethodTable[PeerIterator_setPeer_e], 
				(jlong)0
			);
        }
    }

    /** We return the created instance. */
    return result;
}
