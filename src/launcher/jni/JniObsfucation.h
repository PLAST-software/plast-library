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

/********************************************************************************/
#define PKG_API     "o/i/g/d/a/"        // "org/inria/genscale/dbscan/api/"
#define PKG_COMMON  "o/i/g/d/i/c/"      // "org/inria/genscale/dbscan/impl/common/"
#define PKG_PLAST   "o/i/g/d/i/p/"      // "org/inria/genscale/dbscan/impl/plast/"

/********************************************************************************/
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
#define Properties_name                 "Properties"   // no obsfucation

/********************************************************************************/
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

