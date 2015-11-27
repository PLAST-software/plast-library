/*****************************************************************************
 *                                                                           *
 *   PLAST : Parallel Local Alignment Search Tool                            *
 *   Version 2.3, released November 2015                                     *
 *   Copyright (c) 2009-2015 Inria-Cnrs-Ens                                  *
 *                                                                           *
 *   PLAST is free software; you can redistribute it and/or modify it under  *
 *   the Affero GPL ver 3 License, that is compatible with the GNU General   *
 *   Public License                                                          *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *   Affero GPL ver 3 License for more details.                              *
 *****************************************************************************/

#include "JniObsfucation.h"
#include "org_inria_genscale_dbscan_impl_plast_RequestController.h"

#include <launcher/jni/Helper.hpp>

#include <stdio.h>
#define DEBUG(a)  //printf a

using namespace std;

#define OK 64
#define KO 32

/** */
static char isEnabled[] = { KO, KO, KO, KO, KO, KO, KO, KO};

/** Shortcut. */
#define LEN  (sizeof(isEnabled)/sizeof(isEnabled[0]))

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
JNIEXPORT void JNICALL Java_org_inria_genscale_dbscan_impl_plast_RequestController_enableLibrary (
    JNIEnv*    env,
    jclass     clazz,
    jboolean   enable
)
{
    isEnabled [LEN/2] = enable ? OK : KO;
}

/*********************************************************************
** METHOD  :
** PURPOSE :
** INPUT   :
** OUTPUT  :
** RETURN  :
** REMARKS :
*********************************************************************/
extern "C" void iLE (char* isAllowed)   // isLibraryEnabled
{
    *isAllowed = isEnabled [LEN/2];
}
