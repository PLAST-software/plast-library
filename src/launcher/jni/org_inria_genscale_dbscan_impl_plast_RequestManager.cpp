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

#include "org_inria_genscale_dbscan_impl_plast_RequestManager.h"

#include <launcher/jni/Helper.hpp>

#include <iostream>

using namespace std;
using namespace launcher::jni;

#define DEBUG(a)  //a

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_RequestManager_initIDs (
    JNIEnv* env,
    jclass clazz
)
{
    DEBUG (cout << "Java_org_inria_genscale_plast_impl_RequestManager_initIDs 1" << endl << flush);

#define INIT_CLASS(p,c)  ClassTable[c##_e] = (jclass) env->NewGlobalRef (env->FindClass (p#c));

    INIT_CLASS ("java/util/",   Properties);
    INIT_CLASS (PKG_API,        IObjectFactory);
    INIT_CLASS (PKG_COMMON,     HspInfo);
    INIT_CLASS (PKG_PLAST,      Request);
    INIT_CLASS (PKG_PLAST,      PeerIterator);
    INIT_CLASS (PKG_PLAST,      RequestResult);
    INIT_CLASS (PKG_PLAST,      QueryResult);
    INIT_CLASS (PKG_PLAST,      Hit);

#define INIT_METHOD(c,m,p)  MethodTable[c##_##m##_e] = env->GetMethodID (ClassTable[c##_e], #m,p);
#define INIT_CONSTR(c,m,p)  MethodTable[c##_##m##_e] = env->GetMethodID (ClassTable[c##_e], "<"#m">",p);

#define SIG(p,c)  "L" p c ";"
#define SIGPROPS  SIG("java/util/", "Properties")
#define SIGMAP    SIG("java/util/", "Map")
#define SIGHMAP   SIG("java/util/", "HashMap")
#define SIGOBJ    SIG("java/lang/", "Object")
#define SIGSTR    SIG("java/lang/", "String")
#define SIGFACTO  SIG(PKG_API,      "IObjectFactory")
#define SIGSEQ    SIG(PKG_API,      "ISequence")
#define SIGQRY    SIG(PKG_API,      "IQueryResult")
#define SIGHSP    SIG(PKG_API,      "IHsp")
#define SIGHSPI   SIG(PKG_API,      "IHspInfo")

    INIT_METHOD (Request,       notifyStarted,                  "()V");
    INIT_METHOD (Request,       notifyFinished,                 "()V");
    INIT_METHOD (Request,       notifyCancelled,                "()V");
    INIT_METHOD (Request,       notifyRequestResultAvailable,   "("  SIG(PKG_API,"IRequestResult") ")V");
    INIT_METHOD (Request,       getProperties,                  "()" SIGPROPS);
    INIT_METHOD (Request,       getFactory,                     "()" SIGFACTO);
    INIT_METHOD (Request,       cancel,                         "(JZ)V");

    INIT_METHOD (PeerIterator,   setPeer,            "(J)V");

    INIT_METHOD (Properties,     setProperty,        "(" SIGSTR SIGSTR")" SIGOBJ);

    INIT_METHOD (IObjectFactory,    createSequence,  "(" SIGSTR SIGSTR "I" SIGSTR     ")" SIGSEQ);
    INIT_METHOD (IObjectFactory,    createHsp,       "(" "IDDDIIII" SIGHSPI SIGHSPI ")" SIGHSP);

    INIT_CONSTR (RequestResult, init, "(J" SIGFACTO "I" SIGPROPS ")V");
    INIT_CONSTR (QueryResult,   init, "(J" SIGFACTO "I" SIGSEQ   ")V");
    INIT_CONSTR (Hit,           init, "(J" SIGFACTO "I" SIGSEQ SIGQRY ")V");
    INIT_CONSTR (Properties,    init, "()V");
    INIT_CONSTR (HspInfo,       init, "(" SIGSEQ "IIIID" ")V" );

    DEBUG (cout << "Java_org_inria_genscale_plast_impl_RequestManager_initIDs 2" << endl << flush);
}
