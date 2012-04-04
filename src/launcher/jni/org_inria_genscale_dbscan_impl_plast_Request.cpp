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

#include "org_inria_genscale_dbscan_impl_plast_Request.h"

#include <designpattern/api/IProperty.hpp>

#include <algo/core/api/IAlgoEvents.hpp>
#include <algo/core/api/IAlgoEnvironment.hpp>

#include <alignment/visitors/impl/AdapterAlignmentVisitor.hpp>
#include <alignment/visitors/impl/ModelBuilderVisitor.hpp>

#include <launcher/core/PlastCmd.hpp>

#include <launcher/observers/AbstractProgressionObserver.hpp>

#include <launcher/jni/Helper.hpp>
#include <launcher/jni/Wrapper.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

using namespace dp;
using namespace dp::impl;

using namespace algo::core;

using namespace alignment::core;
using namespace alignment::visitors::impl;

using namespace launcher::core;
using namespace launcher::observers;
using namespace launcher::jni;

/********************************************************************************/

class RequestLink : public AbstractProgressionObserver
{
public:

    RequestLink (JNIEnv* env, jobject obj) : _env(env), _obj(obj) {}

    ~RequestLink ()  { }

    void update (dp::EventInfo* evt, dp::ISubject* subject)
    {
        /** We call parent method. */
        AbstractProgressionObserver::update (evt, subject);

        /** We cast the received event in the type we are interested in. */
        algo::core::AlgorithmReportEvent* e1 = dynamic_cast<algo::core::AlgorithmReportEvent*> (evt);
        if (e1 != 0)    {  updatePartFinished (e1);  return;  }

        AlgorithmConfigurationEvent* e2 = dynamic_cast<AlgorithmConfigurationEvent*> (evt);
        if (e2 != 0)
        {
                 if (e2->_current == 0 && e2->_total == 0)  {   updateStarted  ();   updateFinished();  }
            else if (e2->_current == 0)                     {   updateStarted  ();                      }
            else if (e2->_current == e2->_total)            {   updateFinished ();                      }
            return;
        }
    }

    /** Create a java Properties holding exec information. */
    jobject createExecInfoProperties ()
    {
        /** We create a java.util.Properties instance. */
        jobject props = _env->NewObject (ClassTable [Properties_e], MethodTable[Properties_init_e]);

        /** We add some entries into the map. */
        fillJavaProperties (props, "exec_percent",   _globalPercentage);
        fillJavaProperties (props, "exec_current",   _currentAlgo + 1);
        fillJavaProperties (props, "exec_total",     _totalAlgo);
        fillJavaProperties (props, "time_ellapsed",  _ellapsedTime);
        fillJavaProperties (props, "time_remaining", _remainingTime);
        fillJavaProperties (props, "nb_hsp_current", _currentNbAlignments);
        fillJavaProperties (props, "nb_hsp_total",   _nbAlignments);

        return props;
    }

protected:

    void dump () {}

    void updateStarted       ()  {  _env->CallObjectMethod (_obj, MethodTable[Request_notifyStarted_e]);   }
    void updateFinished      ()  {  _env->CallObjectMethod (_obj, MethodTable[Request_notifyFinished_e]);  }

    void updatePartFinished  (algo::core::AlgorithmReportEvent* evt)
    {
        /** Shortcut. */
        IAlignmentContainer* alignments = evt->getAlignmentContainer();

        /** We create a model for the container. */
        ModelBuilderVisitor v;
        alignments->accept (&v);

        /** Shortcut. */
        IRootLevel* model = v.getModel();

        if (!model)  { return;  }

        /** We use the model. */
        LOCAL (model);

        /** We initialize the iterator. */
        model->first();

        /** We retrieve the object factory. */
        jobject factory  = _env->CallObjectMethod (_obj, MethodTable [Request_getFactory_e]);

        /** We create a new RequestResult instance and give to it the container model. */
        jobject requestResult = _env->NewObject (
            ClassTable [RequestResult_e],
            MethodTable[RequestResult_init_e],
            (jlong)   model,
            (jobject) factory,
            (jint)    model->size()
        );

        /** We notify potential java listeners. */
        _env->CallObjectMethod (_obj, MethodTable [Request_notifyExecInfoAvailable_e],  createExecInfoProperties());

        /** We notify potential java listeners. */
        _env->CallObjectMethod (_obj, MethodTable [Request_notifyRequestResultAvailable_e], requestResult);
    }

private:

    JNIEnv* _env;
    jobject _obj;

    /** */
    template <class T> void fillJavaProperties (jobject props, const char* key, T value)
    {
        stringstream ss;
        ss << value;

        _env->CallObjectMethod (props, MethodTable[Properties_setProperty_e],
            _env->NewStringUTF(key),  _env->NewStringUTF (ss.str().c_str())
        );
    }
};

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_run (
    JNIEnv* env,
    jobject obj,
    jlong   peer,
    jobject factory
)
{
    DEBUG (("[JNI] Request::run\n"));

    /** We check that the provided peer exists. */
    if (peer == 0)  { return; }

    /** We retrieve the PlastCmd instance from the peer. */
    PlastCmd* cmd = (PlastCmd*) peer;

    /** We create a Java link to the request. */
    RequestLink* link = new RequestLink (env,obj);
    LOCAL (link);

    /** We keep a reference to this link from the java world as a specific peer. */
    env->CallObjectMethod (obj, MethodTable [Request_setExecInfoPeer_e], link);

    /** We attach the link to the plast request. */
    cmd->addObserver (link);

    /** We launch the request. */
    cmd->execute ();

    cmd->removeObserver (link);

    /** We reset the peers. */
    env->CallObjectMethod (obj, MethodTable [Request_setExecInfoPeer_e], 0);
    env->CallObjectMethod (obj, MethodTable [PeerIterator_setPeer_e],    0);

    /** We forget the peer. */
    cmd->forget ();
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_cancel (
    JNIEnv* env,
    jobject obj,
    jlong   peer,
    jboolean detach
)
{
    DEBUG (("CPP: Request_cancel\n"));

    PlastCmd* cmd = (PlastCmd*) peer;

    if (cmd != 0 &&  cmd->isRunning() == true)
    {
        cmd->cancel();

        /** We notify potential listeners. */
        env->CallObjectMethod (obj, MethodTable[Request_notifyCancelled_e]);
    }
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT jboolean JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_isRunning (
    JNIEnv* env,
    jobject obj,
    jlong   peer
)
{
    jboolean result = 0;

    DEBUG (("CPP: Request_isRunning \n"));

    PlastCmd* cmd = (PlastCmd*) peer;

    result = (cmd && cmd->isRunning());

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
JNIEXPORT jobject JNICALL Java_org_inria_genscale_dbscan_impl_plast_Request_getExecInfo (
    JNIEnv* env,
    jobject self,
    jlong   execInfoPeer
)
{
    jobject result = 0;

    /** We retrieve the RequestLink instance from the peer. */
    RequestLink* link = (RequestLink*) execInfoPeer;

    if (link != 0)
    {
        result = link->createExecInfoProperties ();
    }
    else
    {
        result = env->NewObject (ClassTable [Properties_e], MethodTable[Properties_init_e]);
    }

    return result;
}
