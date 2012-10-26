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
#include "org_inria_genscale_dbscan_impl_plast_RequestManager.h"

#include <misc/api/version.hpp>

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

extern "C" void iLE (char* isAllowed);

#define CONTROL_CHECKS 1

static bool isVersionNumberValid = false;

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
static bool CheckCode (const char* code)
{
    if (!code)  { return false; }

    char* dup = strdup (code);
    if (!dup)   { return false; }

    size_t len = strlen (dup);

    /** Here, the idea is to parse the provided code and for each found digit,
     *  change the character by decrementing the value.
     *  For instance, "V3.2.1b" should become "V2.1.0b"  */
    for (size_t i=0; i<len; i++)
    {
        char c = dup[i];

        if (c>='0' && c<='9')  {  dup[i] = ((c - '0' + 9) % 10) + '0';  }
    }

    /** We compute the result. */
    bool result = strcmp (dup, PLAST_VERSION) == 0;

    /** We release resources. */
    free (dup);

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
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_RequestManager_initIDs (
    JNIEnv* env,
    jclass clazz,
    jstring code
)
{
    DEBUG (cout << "Java_org_inria_genscale_plast_impl_RequestManager_initIDs 1" << endl << flush);

    INIT_CLASS ("java/util/",   Properties,                     Properties_name);
    INIT_CLASS (PKG_API,        IObjectFactory,                 IObjectFactory_name);
    INIT_CLASS (PKG_COMMON,     HspInfo,                        HspInfo_name);
    INIT_CLASS (PKG_PLAST,      Request,                        Request_name);
    INIT_CLASS (PKG_PLAST,      PeerIterator,                   PeerIterator_name);
    INIT_CLASS (PKG_PLAST,      RequestResult,                  RequestResult_name);
    INIT_CLASS (PKG_PLAST,      QueryResult,                    QueryResult_name);
    INIT_CLASS (PKG_PLAST,      Hit,                            Hit_name);
    INIT_CLASS (PKG_PLAST,      DisabledLibraryException,       DisabledLibraryException_name);

    INIT_METHOD (Request,           getProperties,                  getProperties_name,                 "()" SIGPROPS);
    INIT_METHOD (Request,           getFactory,                     getFactory_name,                    "()" SIGFACTO);
    INIT_METHOD (Request,           cancel,                         cancel_name,                        "(JZ)V");
    INIT_METHOD (Request,           setExecInfoPeer,                setExecInfoPeer_name,               "(J)V");
    INIT_METHOD (Request,           notifyStarted,                  notifyStarted_name,                 "()V");
    INIT_METHOD (Request,           notifyFinished,                 notifyFinished_name,                "()V");
    INIT_METHOD (Request,           notifyCancelled,                notifyCancelled_name,               "()V");
    INIT_METHOD (Request,           notifyExecInfoAvailable,        notifyExecInfoAvailable_name,       "("  SIGPROPS ")V");
    INIT_METHOD (Request,           notifyRequestResultAvailable,   notifyRequestResultAvailable_name,  "("  SIGIRR ")V");
    INIT_METHOD (PeerIterator,      setPeer,                        setPeer_name,          "(J)V");
    INIT_METHOD (IObjectFactory,    createSequence,                 createSequence_name,   "(" SIGSTR SIGSTR "I" SIGSTR   ")" SIGSEQ);
    INIT_METHOD (IObjectFactory,    createHsp,                      createHsp_name,        "(" "IDDDIIII" SIGHSPI SIGHSPI ")" SIGHSP);
    INIT_METHOD (Properties,        setProperty,                    setProperty_name,      "(" SIGSTR SIGSTR")" SIGOBJ);

    INIT_CONSTR (RequestResult, "(J" SIGFACTO "I" ")V");
    INIT_CONSTR (QueryResult,   "(J" SIGFACTO "I" SIGSEQ   ")V");
    INIT_CONSTR (Hit,           "(J" SIGFACTO "I" SIGSEQ SIGQRY ")V");
    INIT_CONSTR (Properties,    "()V");
    INIT_CONSTR (HspInfo,       "(" SIGSEQ "IIIID" ")V" );
    INIT_CONSTR (DisabledLibraryException,       "(" SIGSTR ")V" );

    DEBUG (for (size_t i=0; i<MethodLast_e; i++)  {  printf ("METHOD[%d] = %p\n", i, MethodTable[i]);  })

#ifdef CONTROL_CHECKS
    const char* codeBuffer = env->GetStringUTFChars (code, NULL);
    if (codeBuffer)
    {
        /** We see whether the version number is good or not. */
        isVersionNumberValid = CheckCode (codeBuffer) == true;

        /** We release resources. */
        env->ReleaseStringUTFChars (code, codeBuffer);
    }

    if (isVersionNumberValid == false)
    {
        /** LIBRARY NOT ENABLED !!! We launch an exception. */
        env->ThrowNew (CLASS(DisabledLibraryException), "Library disabled... Bad version...");
    }

#endif

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
#ifdef CONTROL_CHECKS

    /** We check whether the version number is good or not. */
    if (isVersionNumberValid == false)
    {
        /** LIBRARY NOT ENABLED !!! We launch an exception. */
        env->ThrowNew (CLASS(DisabledLibraryException), "Library disabled... Bad version...");

        /** We return a null object as a result. */
        return 0;
    }

    /** We check whether the library is enabled or not. */
    char isEnabled=0;   iLE (&isEnabled);

    if (isEnabled != 64)
    {
        /** LIBRARY NOT ENABLED !!! We launch an exception. */
        env->ThrowNew (CLASS(DisabledLibraryException), "Library disabled...");

        /** We return a null object as a result. */
        return 0;
    }
#endif

    /** We convert the JAVA properties as C++ properties. */
    IProperties* props = Wrapper(env).convertProperties (javaProps);

#ifdef WITH_PLASTRC
    /** We try to see if we have a provided rc file. */
    string plastrc = getenv ("HOME") ? string (getenv("HOME")) + string("/.plastrc") : "";

    /** We read properties from the init file (if any). */
    props->add (0, new Properties (plastrc.c_str()));
#endif

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
            _env->CallObjectMethod (_javaprops, METHOD (Properties,setProperty),
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

