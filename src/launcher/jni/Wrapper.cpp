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

#include "Wrapper.hpp"

#include <designpattern/impl/Property.hpp>

using namespace std;
using namespace dp;
using namespace dp::impl;

/*********************************************************************
 ** METHOD  :
 ** PURPOSE :
 ** INPUT   :
 ** OUTPUT  :
 ** RETURN  :
 ** REMARKS :
 *********************************************************************/
dp::IProperties*  Wrapper::convertProperties (jobject props)
{
    dp::IProperties* res = new Properties ();

    if (props != 0)
    {
        /** We get the class of the callee object. */
        jclass properties_class = _env->GetObjectClass (props);

        /** We retrieve the "keys" method. */
        jmethodID keys_id        = _env->GetMethodID (properties_class, "keys",        "()Ljava/util/Enumeration;");
        jmethodID getProperty_id = _env->GetMethodID (properties_class, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");

        /** We call the "keys" method. */
        jobject keys_o = _env->CallObjectMethod (props, keys_id);

        jclass enumeration_class = _env->GetObjectClass (keys_o);

        jmethodID hasMoreElements_id = _env->GetMethodID (enumeration_class, "hasMoreElements", "()Z");
        jmethodID hasNextElement_id  = _env->GetMethodID (enumeration_class, "nextElement",     "()Ljava/lang/Object;");

        jboolean hasMoreElements_res;
        while ( (hasMoreElements_res = _env->CallBooleanMethod (keys_o, hasMoreElements_id)) != 0)
        {
            jstring currentKey     = (jstring) _env->CallObjectMethod (keys_o, hasNextElement_id);
            jstring currentElement = (jstring) _env->CallObjectMethod (props,  getProperty_id, currentKey, 0);

            String key (_env, currentKey);
            String val (_env, currentElement);

            res->add (0, key.str(), val.str());
        }
    }

    return res;
}
