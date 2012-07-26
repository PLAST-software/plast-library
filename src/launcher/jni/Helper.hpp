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

#ifndef JNI_HELPER_H_
#define JNI_HELPER_H_

/********************************************************************************/
#include <jni.h>
#include <map>
#include <list>
#include <string>

/********************************************************************************/
namespace launcher  {
namespace jni       {
/********************************************************************************/

#define CLASS_DEF(c)  c##_e

enum Class_e
{
    CLASS_DEF (RequestManager),
    CLASS_DEF (Request),
    CLASS_DEF (PeerIterator),
    CLASS_DEF (RequestResult),
    CLASS_DEF (QueryResult),
    CLASS_DEF (Hit),
    CLASS_DEF (Hsp),
    CLASS_DEF (Properties),
    CLASS_DEF (IObjectFactory),
    CLASS_DEF (HspInfo),
    CLASS_DEF (RequestController),
    CLASS_DEF (DisabledLibraryException),
    ClassLast_e
};

/********************************************************************************/

#define METHOD_DEF(c,m)  c##_##m##_e

enum Method_e
{
    /*  0  */  METHOD_DEF (Request,        notifyStarted),
    /*  1  */  METHOD_DEF (Request,        notifyFinished),
    /*  2  */  METHOD_DEF (Request,        notifyCancelled),
    /*  3  */  METHOD_DEF (Request,        notifyExecInfoAvailable),
    /*  4  */  METHOD_DEF (Request,        notifyRequestResultAvailable),
    /*  5  */  METHOD_DEF (Request,        getProperties),
    /*  6  */  METHOD_DEF (Request,        getFactory),
    /*  7  */  METHOD_DEF (Request,        cancel),
    /*  8  */  METHOD_DEF (Request,        setExecInfoPeer),

    /*  9  */  METHOD_DEF (PeerIterator,   init),
    /*  10 */  METHOD_DEF (PeerIterator,   setPeer),

    /*  11 */  METHOD_DEF (Properties,     setProperty),

    /*  12 */  METHOD_DEF (IObjectFactory, createSequence),
    /*  13 */  METHOD_DEF (IObjectFactory, createHsp),

    /*  14 */  METHOD_DEF (RequestResult,  init),
    /*  15 */  METHOD_DEF (QueryResult,    init),
    /*  16 */  METHOD_DEF (Hit,            init),
    /*  17 */  METHOD_DEF (Hsp,            init),
    /*  18 */  METHOD_DEF (Properties,     init),
    /*  19 */  METHOD_DEF (HspInfo,        init),

    /*  20 */  METHOD_DEF (RequestController,  enableLibrary),

    /*  21 */  METHOD_DEF (DisabledLibraryException,  init),

    MethodLast_e
};

/********************************************************************************/

extern jclass    ClassTable[];
extern jmethodID MethodTable[];

/********************************************************************************/
#define CLASS(c)     ClassTable  [c##_e]
#define METHOD(c,m)  MethodTable [c##_##m##_e]

/********************************************************************************/
#define INIT_CLASS(p,c,name)     CLASS(c)       = (jclass) env->NewGlobalRef (env->FindClass (p name));
#define INIT_METHOD(c,m,name,p)  METHOD(c,m)    = env->GetMethodID (ClassTable[c##_e], name,p);
#define INIT_CONSTR(c,p)         METHOD(c,init) = env->GetMethodID (ClassTable[c##_e], "<init>",p);

/********************************************************************************/
}}
/********************************************************************************/

#endif /* JNI_HELPER_H_ */
