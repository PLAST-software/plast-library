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

#ifndef WRAPPER_H_
#define WRAPPER_H_

/********************************************************************************/
#include <jni.h>
#include <stdio.h>
#include <map>
#include <string>

#include <designpattern/api/IProperty.hpp>

/********************************************************************************/

class Wrapper
{
public:

    Wrapper (JNIEnv* env)  : _env(env) {}

    dp::IProperties* convertProperties (jobject props);

private:
    JNIEnv* _env;

    class String
    {
    public:
        String (JNIEnv* env, jstring str)  : _env(env), _jstr(str), _str(0)
        {
            if (_env != 0)  {  _str = _env->GetStringUTFChars (_jstr, NULL);  }
        }

        ~String ()
        {
            if (_env != 0)  { _env->ReleaseStringUTFChars (_jstr, _str);  }
        }

        const char* c_str() { return _str; }

        std::string str() { return std::string(_str); }

    private:
        JNIEnv*     _env;
        jstring     _jstr;
        const char* _str;
    };
};

/********************************************************************************/

#endif /* WRAPPER_H_ */
