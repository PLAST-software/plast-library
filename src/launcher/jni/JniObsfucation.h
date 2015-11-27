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

#ifdef JNI_OBSFUCATION

    #define Java_org_inria_genscale_dbscan_impl_plast_Hit_retrieveNext  \
            Java_o_i_g_d_i_p_H_rN

    #define Java_org_inria_genscale_dbscan_impl_plast_QueryResult_retrieveNext  \
            Java_o_i_g_d_i_p_QR_rN

    #define Java_org_inria_genscale_dbscan_impl_plast_Request_run  \
            Java_o_i_g_d_i_p_R_r

    #define Java_org_inria_genscale_dbscan_impl_plast_Request_cancel  \
            Java_o_i_g_d_i_p_R_c

    #define Java_org_inria_genscale_dbscan_impl_plast_Request_isRunning  \
            Java_o_i_g_d_i_p_R_iR

    #define Java_org_inria_genscale_dbscan_impl_plast_Request_getExecInfo  \
            Java_o_i_g_d_i_p_R_gEI

    #define Java_org_inria_genscale_dbscan_impl_plast_RequestManager_initIDs  \
            Java_o_i_g_d_i_p_RM_iIDs

    #define Java_org_inria_genscale_dbscan_impl_plast_RequestManager_createPeerRequest  \
            Java_o_i_g_d_i_p_RM_cPR

    #define Java_org_inria_genscale_dbscan_impl_plast_RequestResult_retrieveNext  \
            Java_o_i_g_d_i_p_RR_rN

    #define Java_org_inria_genscale_dbscan_impl_plast_RequestController_enableLibrary  \
            Java_o_i_g_d_i_p_RC_eL
#endif

/********************************************************************************/
#ifdef JNI_OBSFUCATION
    #define PKG_API     "o/i/g/d/a/"        // "org/inria/genscale/dbscan/api/"
    #define PKG_COMMON  "o/i/g/d/i/c/"      // "org/inria/genscale/dbscan/impl/common/"
    #define PKG_PLAST   "o/i/g/d/i/p/"      // "org/inria/genscale/dbscan/impl/plast/"
#else
    #define PKG_API     "org/inria/genscale/dbscan/api/"
    #define PKG_COMMON  "org/inria/genscale/dbscan/impl/common/"
    #define PKG_PLAST   "org/inria/genscale/dbscan/impl/plast/"
#endif
/********************************************************************************/
#ifdef JNI_OBSFUCATION
    #define IObjectFactory_name             "IOF"
    #define HspInfo_name                    "HI"
    #define Request_name                    "R"
    #define PeerIterator_name               "PI"
    #define RequestResult_name              "RR"
    #define QueryResult_name                "QR"
    #define Hit_name                        "H"
    #define ISequence_name                  "IS"
    #define IQueryResult_name               "IQR"
    #define IHsp_name                       "IHs"
    #define IHspInfo_name                   "IHI"
    #define RequestController_name          "RC"
    #define DisabledLibraryException_name   "DLE"
    #define IRequestResult_name             "IRR"
    #define Properties_name                 "Properties"   // no obsfucation
#else
    #define IObjectFactory_name             "IObjectFactory"
    #define HspInfo_name                    "HspInfo"
    #define Request_name                    "Request"
    #define PeerIterator_name               "PeerIterator"
    #define RequestResult_name              "RequestResult"
    #define QueryResult_name                "QueryResult"
    #define Hit_name                        "Hit"
    #define ISequence_name                  "ISequence"
    #define IQueryResult_name               "IQueryResult"
    #define IHsp_name                       "IHsp"
    #define IHspInfo_name                   "IHspInfo"
    #define RequestController_name          "RequestController"
    #define DisabledLibraryException_name   "DisabledLibraryException"
    #define IRequestResult_name             "IRequestResult"
    #define Properties_name                 "Properties"   // no obsfucation
#endif

/********************************************************************************/
#ifdef JNI_OBSFUCATION
    #define notifyStarted_name                  "nS"
    #define notifyFinished_name                 "nF"
    #define notifyCancelled_name                "nC"
    #define notifyExecInfoAvailable_name        "nEIA"
    #define notifyRequestResultAvailable_name   "nRRA"
    #define getProperties_name                  "gP"
    #define getFactory_name                     "gF"
    #define cancel_name                         "c"
    #define setExecInfoPeer_name                "sEIP"
    #define setPeer_name                        "sP"
    #define createSequence_name                 "cS"
    #define createHsp_name                      "cH"
    #define enableLibrary_name                  "eL"
    #define setProperty_name                    "setProperty"   // no obsfucation
#else
    #define notifyStarted_name                  "notifyStarted"
    #define notifyFinished_name                 "notifyFinished"
    #define notifyCancelled_name                "notifyCancelled"
    #define notifyExecInfoAvailable_name        "notifyExecInfoAvailable"
    #define notifyRequestResultAvailable_name   "notifyRequestResultAvailable"
    #define getProperties_name                  "getProperties"
    #define getFactory_name                     "getFactory"
    #define cancel_name                         "cancel"
    #define setExecInfoPeer_name                "setExecInfoPeer"
    #define setPeer_name                        "setPeer"
    #define createSequence_name                 "createSequence"
    #define createHsp_name                      "createHsp"
    #define enableLibrary_name                  "enableLibrary"
    #define setProperty_name                    "setProperty"   // no obsfucation
#endif

/********************************************************************************/
#define SIG(p,c)  "L" p c ";"
#define SIGPROPS  SIG("java/util/", "Properties")
#define SIGMAP    SIG("java/util/", "Map")
#define SIGHMAP   SIG("java/util/", "HashMap")
#define SIGOBJ    SIG("java/lang/", "Object")
#define SIGSTR    SIG("java/lang/", "String")
#define SIGFACTO  SIG(PKG_API,      IObjectFactory_name)
#define SIGSEQ    SIG(PKG_API,      ISequence_name)
#define SIGQRY    SIG(PKG_API,      IQueryResult_name)
#define SIGHSP    SIG(PKG_API,      IHsp_name)
#define SIGHSPI   SIG(PKG_API,      IHspInfo_name)
#define SIGIRR    SIG(PKG_API,      IRequestResult_name)

