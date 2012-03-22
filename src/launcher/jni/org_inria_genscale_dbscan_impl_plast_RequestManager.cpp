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

#include <designpattern/api/IProperty.hpp>

#include <launcher/jni/Helper.hpp>
#include <launcher/jni/Wrapper.hpp>

#include <launcher/core/PlastCmd.hpp>

#include <iostream>

using namespace std;

using namespace dp;
using namespace dp::impl;

using namespace launcher::jni;
using namespace launcher::core;

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
    INIT_METHOD (Request,       notifyExecInfoAvailable,        "("  SIGPROPS ")V");
    INIT_METHOD (Request,       notifyRequestResultAvailable,   "("  SIG(PKG_API,"IRequestResult") ")V");
    INIT_METHOD (Request,       getProperties,                  "()" SIGPROPS);
    INIT_METHOD (Request,       getFactory,                     "()" SIGFACTO);
    INIT_METHOD (Request,       cancel,                         "(JZ)V");
    INIT_METHOD (Request,       setExecInfoPeer,                "(J)V");

    INIT_METHOD (PeerIterator,   setPeer,            "(J)V");

    INIT_METHOD (Properties,     setProperty,        "(" SIGSTR SIGSTR")" SIGOBJ);

    INIT_METHOD (IObjectFactory,    createSequence,  "(" SIGSTR SIGSTR "I" SIGSTR     ")" SIGSEQ);
    INIT_METHOD (IObjectFactory,    createHsp,       "(" "IDDDIIII" SIGHSPI SIGHSPI ")" SIGHSP);

    INIT_CONSTR (RequestResult, init, "(J" SIGFACTO "I" ")V");
    INIT_CONSTR (QueryResult,   init, "(J" SIGFACTO "I" SIGSEQ   ")V");
    INIT_CONSTR (Hit,           init, "(J" SIGFACTO "I" SIGSEQ SIGQRY ")V");
    INIT_CONSTR (Properties,    init, "()V");
    INIT_CONSTR (HspInfo,       init, "(" SIGSEQ "IIIID" ")V" );

    DEBUG (cout << "Java_org_inria_genscale_plast_impl_RequestManager_initIDs 2" << endl << flush);
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT jlong JNICALL Java_org_inria_genscale_dbscan_impl_plast_RequestManager_createPeerRequest (
    JNIEnv* env,
    jobject self,
    jobject javaProps
)
{
    /** We convert the JAVA properties as C++ properties. */
    IProperties* props = Wrapper(env).convertProperties (javaProps);

    /** We create a Plast request. */
    PlastCmd* cmd = new PlastCmd (props);

    /** We use this command. */
    cmd->use ();

    /** We update the java props with the properties of the brand new command. */
    class UpdatePropsVisitor : public IPropertiesVisitor
    {
    public:
        UpdatePropsVisitor (JNIEnv* env, jobject javaprops) : _env(env), _javaprops(javaprops) {}

        void visitBegin () {}
        void visitEnd   () {}

        void visitProperty (IProperty* prop)
        {
            _env->CallObjectMethod (_javaprops, MethodTable[Properties_setProperty_e],
                _env->NewStringUTF(prop->key.c_str()),  _env->NewStringUTF (prop->getString())
            );
        }

    private:
        JNIEnv* _env;
        jobject _javaprops;
    };

    UpdatePropsVisitor v (env, javaProps);
    cmd->getProperties()->accept (&v);

    /** We return the command as a peer. */
    return (jlong) cmd;
}

