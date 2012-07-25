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
    METHOD_DEF (Request,        notifyStarted),
    METHOD_DEF (Request,        notifyFinished),
    METHOD_DEF (Request,        notifyCancelled),
    METHOD_DEF (Request,        notifyExecInfoAvailable),
    METHOD_DEF (Request,        notifyRequestResultAvailable),
    METHOD_DEF (Request,        getProperties),
    METHOD_DEF (Request,        getFactory),
    METHOD_DEF (Request,        cancel),
    METHOD_DEF (Request,        setExecInfoPeer),

    METHOD_DEF (PeerIterator,   init),
    METHOD_DEF (PeerIterator,   setPeer),

    METHOD_DEF (Properties,     setProperty),

    METHOD_DEF (IObjectFactory, createSequence),
    METHOD_DEF (IObjectFactory, createHsp),

    METHOD_DEF (RequestResult,  init),
    METHOD_DEF (QueryResult,    init),
    METHOD_DEF (Hit,            init),
    METHOD_DEF (Hsp,            init),
    METHOD_DEF (Properties,     init),
    METHOD_DEF (HspInfo,        init),

    METHOD_DEF (RequestController,  enableLibrary),

    METHOD_DEF (DisabledLibraryException,  init),

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
