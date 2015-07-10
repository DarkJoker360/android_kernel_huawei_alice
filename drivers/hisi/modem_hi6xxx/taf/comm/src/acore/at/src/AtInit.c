/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : AtInit.c
  �� �� ��   : ����
  ��    ��   : ³��/l60609
  ��������   : 2012��1��22��
  ����޸�   :
  ��������   : AT����ڣ�ִ��PID�ĳ�ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��1��22��
    ��    ��   : ³��/l60609
    �޸�����   : �����ļ�

******************************************************************************/
/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "AtInit.h"

/* ������ͷ�ļ�����Ϊ������PS����صĶ��壬���Ǳ���Ŀδ�޸�PS����صģ�������Ҫ���� */
#include "AtDataProc.h"

/* Added by f00179208 for CCPU RESET, 2013-04-15, Begin */
#include "AcpuReset.h"
/* Added by f00179208 for CCPU RESET, 2013-04-15, End */

#if (FEATURE_ON == FEATURE_LTE)
#include "msp_nvim.h"
#include "msp_nv_id.h"
#include "msp_nv_def.h"
#include "LPsNvInterface.h"
#include "at_lte_common.h"      /* ADDED by c64416 for V9R1/V7R1 AT, 2013/09/18 */
#endif

/* Added by l00167671 for NV�����Ŀ , 2013-05-17, begin */
#include "GasNvInterface.h"
#include "NasNvInterface.h"
#include "TafNvInterface.h"
/* Added by l00167671 for NV�����Ŀ , 2013-05-17, end*/

/* add by j00174725 for V3R3 Cut Out Memory, 2013-11-06, Begin */
#include  "product_config.h"
/* add by j00174725 for V3R3 Cut Out Memory, 2013-11-06, End */

/* Added by j00174725 for UART-MODEM Project, 2013-09-21, begin */
#include "mdrv.h"
/* Added by j00174725 for UART-MODEM Project, 2013-09-21, end */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
/*lint -e767 -e960*/
#define    THIS_FILE_ID                 PS_FILE_ID_AT_INIT_C
/*lint +e767 +e960*/


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : AT_ReadPlatformNV
 ��������  : ƽ̨���뼼��NV��ȡ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��27��
    ��    ��   : z00220246
    �޸�����   : DSDA Phase II:�����ɺ���

*****************************************************************************/
VOS_VOID AT_ReadPlatformNV(VOS_VOID)
{
    MODEM_ID_ENUM_UINT16                enModemID;
    PLATAFORM_RAT_CAPABILITY_STRU       stPlatFormRat;
    VOS_UINT8                           ucRatIndex;
    AT_MODEM_SPT_RAT_STRU              *pstAtSptRatList = VOS_NULL_PTR;

    PS_MEM_SET(&stPlatFormRat, 0x00, sizeof(PLATAFORM_RAT_CAPABILITY_STRU));

    for (enModemID = 0; enModemID < MODEM_ID_BUTT; enModemID++)
    {
        pstAtSptRatList = AT_GetSptRatFromModemId(enModemID);

        /* ��ȡƽ̨NV�ɹ� */
        if(NV_OK == NV_ReadEx(enModemID, en_NV_Item_Platform_RAT_CAP, &stPlatFormRat,
                              sizeof(PLATAFORM_RAT_CAPABILITY_STRU)))
        {
            for (ucRatIndex = 0; ucRatIndex < stPlatFormRat.usRatNum; ucRatIndex++)
            {
#if(FEATURE_ON == FEATURE_LTE)
                /* ƽ̨֧��LTE */
                if (PLATFORM_RAT_LTE == stPlatFormRat.aenRatList[ucRatIndex])
                {
                    pstAtSptRatList->ucPlatformSptLte = VOS_TRUE;
                }
#endif
                /* ƽ̨֧��WCDMA */
                if (PLATFORM_RAT_WCDMA == stPlatFormRat.aenRatList[ucRatIndex])
                {
                    pstAtSptRatList->ucPlatformSptWcdma = VOS_TRUE;
                }

#if(FEATURE_ON == FEATURE_UE_MODE_TDS)
                /* ƽ̨֧��TDS*/
                if (PLATFORM_RAT_TDS == stPlatFormRat.aenRatList[ucRatIndex])
                {
                    pstAtSptRatList->ucPlatformSptUtralTDD = VOS_TRUE;
                }
#endif
                /* ƽ̨֧��GSM */
                if (PLATFORM_RAT_GSM == stPlatFormRat.aenRatList[ucRatIndex])
                {
                    pstAtSptRatList->ucPlatformSptGsm = VOS_TRUE;
                }
            }
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadClientConfigNV
 ��������  : ��ȡen_NV_Item_AT_CLIENT_CONFIG���ж������ĸ�modem
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��28��
    ��    ��   : l00227485
    �޸�����   : DSDA PhaseII: �����ɺ���
  2.��    ��   : 2014��04��23��
    ��    ��   : f00179208
    �޸�����   : DTS2014042304605
*****************************************************************************/
VOS_VOID AT_ReadClientConfigNV(VOS_VOID)
{
    /* Modified by f00179208 for DTS2014042304605, 2014-04-23, begin */
    TAF_AT_NVIM_AT_CLIENT_CONFIG_STRU   stAtClientCfg;
    AT_CLINET_CONFIG_DESC_STRU         *pstCfgDesc;
    AT_CLIENT_CONFIGURATION_STRU       *pstClientCfg;
    AT_CLIENT_CFG_MAP_TAB_STRU         *pstCfgMapTbl;
    VOS_UINT8                           i;

    PS_MEM_SET(&stAtClientCfg, 0, sizeof(TAF_AT_NVIM_AT_CLIENT_CONFIG_STRU));

    /* NV��en_NV_Item_AT_CLIENT_CONFIG��
       aucAtClientConfig[Index]
       Index = 0 -- AT_CLIENT_TAB_PCUI_INDEX
       Index = 1 -- AT_CLIENT_TAB_CTRL_INDEX
       ......
       ��index˳�����
       aucAtClientConfig[Index]
       BIT0-BIT1��Ӧһ��client�������ĸ�ModemId:00:��ʾmodem0 01:��ʾmodem1
       BIT2��Ӧһ��client�Ƿ������㲥:0:��ʾ������ 1:��ʾ����
    */
    if (VOS_OK != NV_ReadEx(MODEM_ID_0,
                            en_NV_Item_AT_CLIENT_CONFIG,
                            &stAtClientCfg,
                            sizeof(TAF_AT_NVIM_AT_CLIENT_CONFIG_STRU)))
    {
        return;
    }

    pstCfgDesc = (AT_CLINET_CONFIG_DESC_STRU *)&(stAtClientCfg.aucAtClientConfig[0]);

    for (i = 0; i < AT_GET_CLIENT_CFG_TAB_LEN(); i++)
    {
        pstCfgMapTbl = AT_GetClientCfgMapTbl(i);
        pstClientCfg = AT_GetClientConfig(pstCfgMapTbl->enClientId);

#if (FEATURE_ON == FEATURE_MULTI_MODEM)
        pstClientCfg->enModemId   = pstCfgDesc[pstCfgMapTbl->enNvIndex].ucModemId;
#endif
        pstClientCfg->ucReportFlg = pstCfgDesc[pstCfgMapTbl->enNvIndex].ucReportFlg;
    }
    /* Modified by f00179208 for DTS2014042304605, 2014-04-23, end */

    return;
}

/*****************************************************************************
 �� �� ��  : AT_GetCpmsMtMem
 ��������  : ��ȡCPMS�����<MT>��Ĭ������
 �������  : ��
 �������  : MODEM_ID_ENUM_UINT16                enModemId
             MN_MSG_MEM_STORE_ENUM_U8           *penSmMemStore  CPMS�����<MT>��Ĭ������
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��20��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_GetCpmsMtMem(
    MODEM_ID_ENUM_UINT16                enModemId,
    MN_MSG_MEM_STORE_ENUM_U8           *penSmMemStore
)
{
    /* modifyed by M00217266 for nv, 2013-05-20, begin */
    VOS_UINT32                          ulRet;
    TAF_NVIM_SMS_SERVICE_PARA_STRU      stSmsServicePara;
    /* ���Ž��մ洢���ʱ�����NV�����ļ���ʹ�ܱ�־
       VOS_TRUE        ���Ž��մ洢���ʱ�����NV�����ļ�
       VOS_FALSE       ���Ž��մ洢���ʲ�������NV�����ļ���ÿ���ϵ��ָ�ΪSM�洢 */
    VOS_BOOL                            bSmMemEnable;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    PS_MEM_SET(&stSmsServicePara, 0x00, sizeof(TAF_NVIM_SMS_SERVICE_PARA_STRU));

    *penSmMemStore = MN_MSG_MEM_STORE_SIM;

    ulRet = NV_ReadEx(enModemId, en_NV_Item_SMS_SERVICE_Para, &stSmsServicePara, MN_MSG_SRV_PARAM_LEN);
    if (NV_OK != ulRet)
    {
        AT_ERR_LOG("AT_GetCpmsMtMem: Read Service Parm From Nvim Failed");
        return;
    }

    bSmMemEnable                  = (stSmsServicePara.aucSmsServicePara)[MN_MSG_SRV_SM_MEM_ENABLE_OFFSET];

    pstSmsCtx                     = AT_GetModemSmsCtxAddrFromModemId(enModemId);

    if (MN_MSG_ME_STORAGE_ENABLE == pstSmsCtx->enMsgMeStorageStatus)
    {
        if (VOS_TRUE == bSmMemEnable)
        {
            *penSmMemStore = (stSmsServicePara.aucSmsServicePara)[MN_MSG_SRV_RCV_SM_MEM_STORE_OFFSET];
        }
    }

    return;
    /* modifyed by M00217266 for nv, 2013-05-20, end */
}

/*****************************************************************************
 �� �� ��  : AT_SendDomainNvimToProto
 ��������  : NVIM�з������ֵת��ΪЭ���еķ������ֵ
 �������  :
 �������  : ��
 �� �� ֵ  : ת����ķ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
AT_CGSMS_SEND_DOMAIN_ENUM_U8 AT_SendDomainNvimToProto(
    VOS_UINT32                           ulNvimSendDomain
)
{
    AT_CGSMS_SEND_DOMAIN_ENUM_U8        enProtoSendDomain;

    if (AT_NV_SMS_DOMAIN_PS_PREFERRED == ulNvimSendDomain)
    {
        enProtoSendDomain = MN_MSG_SEND_DOMAIN_PS_PREFERRED;
    }
    else if ((AT_NV_SMS_DOMAIN_PS_ONLY == ulNvimSendDomain))
    {
        enProtoSendDomain = MN_MSG_SEND_DOMAIN_PS;
    }
    else if (AT_NV_SMS_DOMAIN_CS_ONLY == ulNvimSendDomain)
    {
        enProtoSendDomain = MN_MSG_SEND_DOMAIN_CS;
    }
    else
    {
        enProtoSendDomain = MN_MSG_SEND_DOMAIN_CS_PREFERRED;
    }

    return enProtoSendDomain;
}

/*****************************************************************************
 �� �� ��  : AT_ReadSmsSendDomainNV
 ��������  : ��ȡSMS��������ص�NV��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��20��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_ReadSmsSendDomainNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-20, begin */
    VOS_UINT32                          ulRet;
    AT_NVIM_SEND_DOMAIN_STRU            stSendDomain;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstSmsCtx = AT_GetModemSmsCtxAddrFromModemId(enModemId);

        /* ��ȡʧ�ܺ�δ����һ������,�����ж�NVIM���Ƿ񼤻�,δ������ȡĬ��ֵ */
        PS_MEM_SET(&stSendDomain, 0, sizeof(stSendDomain));
        ulLength = 0;
        NV_GetLength(en_NV_Item_SMS_SEND_DOMAIN, &ulLength);
        ulRet = NV_ReadEx(enModemId, en_NV_Item_SMS_SEND_DOMAIN, &stSendDomain, ulLength);
        if ( (NV_OK == ulRet)
          && (VOS_TRUE == stSendDomain.ucActFlg))
        {
            /* ������NVIM�е�ȡֵ����ת��
               ����NVIM���е�ֵ��Э���е�ֵ��һ��,������Ҫ����һ��ת�� */
            pstSmsCtx->stCgsmsSendDomain.ucActFlg     = VOS_TRUE;
            pstSmsCtx->stCgsmsSendDomain.enSendDomain = AT_SendDomainNvimToProto((VOS_UINT32)stSendDomain.ucSendDomain);
        }
    }
    /* modifyed by M00217266 for nv, 2013-05-20, end */
    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadSmsMeStorageInfoNV
 ��������  : ��ȡME���Ŵ洢���ʵ�֧��״̬
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_ReadSmsMeStorageInfoNV(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MN_MSG_ME_STORAGE_PARM_STRU         stMeStorageParm;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstSmsCtx = AT_GetModemSmsCtxAddrFromModemId(enModemId);

        stMeStorageParm.enMeStorageStatus   = MN_MSG_ME_STORAGE_DISABLE;
        stMeStorageParm.usMeStorageNum      = 0;

        /* ��ȡME���Ŵ洢���ʵ�֧��״̬ */
        ulRet = NV_ReadEx(enModemId, en_NV_Item_Sms_Me_Storage_Info, &stMeStorageParm, sizeof(stMeStorageParm));
        if (NV_OK == ulRet)
        {
            pstSmsCtx->enMsgMeStorageStatus = stMeStorageParm.enMeStorageStatus;
        }

        /* ��ȡ���ż�״̬������մ洢����*/
        AT_GetCpmsMtMem(enModemId, &(pstSmsCtx->stCpmsInfo.stRcvPath.enSmMemStore));
        pstSmsCtx->stCpmsInfo.stRcvPath.enStaRptMemStore = pstSmsCtx->stCpmsInfo.stRcvPath.enSmMemStore;
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadSmsClass0TailorNV
 ��������  : ��ȡCLASS0����Ž����ϱ���ʽ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��20��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_ReadSmsClass0TailorNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-20, begin */
    VOS_UINT32                          ulRet;
    MN_MSG_NVIM_CLASS0_TAILOR_STRU      stClass0Tailor;
    VOS_UINT32                          ulLength;

    ulLength                            = 0;
    stClass0Tailor.ucActFlg             = MN_MSG_NVIM_ITEM_INACTIVE;
    stClass0Tailor.enClass0Tailor       = MN_MSG_CLASS0_DEF;

    NV_GetLength(en_NV_Item_SMS_CLASS0_TAILOR, &ulLength);
    /* ��ȡCLASS0����Ž����ϱ���ʽ */
    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_SMS_CLASS0_TAILOR, &stClass0Tailor, ulLength);
    if ( (NV_OK == ulRet)
      && (MN_MSG_NVIM_ITEM_ACTIVE == stClass0Tailor.ucActFlg))
    {
        g_enClass0Tailor = stClass0Tailor.enClass0Tailor;
    }
    else
    {
        g_enClass0Tailor = MN_MSG_CLASS0_DEF;
    }
    /* modifyed by M00217266 for nv, 2013-05-20, end */

    return;
}

/* modified by l65478 for DTS2013052304352 2013-05-23 begin */
/*****************************************************************************
 �� �� ��  : AT_ReadSmsClosePathNV
 ��������  : ��ȡ����close path��NV��ֵ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��7��17��
    ��    ��   : z00234330
    �޸�����   : DTS2013052304352���ⵥ����
  2.��    ��   : 2013��10��08��
    ��    ��   : j00174725
    �޸�����   : TQE
*****************************************************************************/
VOS_VOID AT_ReadSmsClosePathNV(VOS_VOID)
{
    TAF_NVIM_SMS_CLOSE_PATH_CFG_STRU    stClosePath;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;
    MODEM_ID_ENUM_UINT16                enModemId;
    VOS_UINT32                          ulRet;

    /* Modified by j00174725 for TQE, 2013-10-08, begin */
    PS_MEM_SET(&stClosePath, 0x00, sizeof(stClosePath));
    /* Modified by j00174725 for TQE, 2013-10-08, end */

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstSmsCtx = AT_GetModemSmsCtxAddrFromModemId(enModemId);

        /* ��ȡME���Ŵ洢���ʵ�֧��״̬ */
        ulRet = NV_ReadEx(enModemId, en_NV_Item_SMS_Close_Path, &stClosePath, sizeof(stClosePath));
        if ( (NV_OK == ulRet)
           && (VOS_TRUE == stClosePath.ucNvimValid))
        {
            pstSmsCtx->ucLocalStoreFlg = stClosePath.ucSmsClosePathFlg;
        }
    }
}
/* modified by l65478 for DTS2013052304352 2013-05-23 end */

/* Added by x00182239 for DTS2014120310853, 2015-1-14, Begin */
/*****************************************************************************
 �� �� ��  : AT_ReadMtCustomizeInfo
 ��������  : ��ȡ���Ž�����Ӫ�̶�������
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��01��14��
    ��    ��   : x00182239
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadMtCustomizeInfo(VOS_VOID)
{
    VOS_UINT32                          ulRet;
    MN_MSG_MT_CUSTOMIZE_INFO_STRU       stMtCustomize;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstSmsCtx = AT_GetModemSmsCtxAddrFromModemId(enModemId);

        ulRet = NV_Read(en_NV_Item_SMS_MT_CUSTOMIZE_INFO,
                       &stMtCustomize,
                        sizeof(stMtCustomize));
        if (NV_OK != ulRet)
        {
            AT_INFO_LOG("AT_ReadMtCustomizeInfo: Fail to read ");
            return;
        }

        if (MN_MSG_NVIM_ITEM_ACTIVE == stMtCustomize.ucActFlag)
        {
            pstSmsCtx->stSmMeFullCustomize.ucActFlg      = VOS_TRUE;
            pstSmsCtx->stSmMeFullCustomize.enMtCustomize = stMtCustomize.enMtCustomize;
        }
    }
    return;
}
/* Added by x00182239 for DTS2014120310853, 2015-1-14, End */

/* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
#ifdef DMT
/*****************************************************************************
 �� �� ��  : AT_ReadPortBuffCfgNV
 ��������  : ��ȡ�������NV��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : HSUART PHASE III
*****************************************************************************/
VOS_VOID AT_ReadPortBuffCfgNV(VOS_VOID)
{
    AT_COMM_CTX_STRU                   *pstCommCtx = VOS_NULL_PTR;

    pstCommCtx = AT_GetCommCtxAddr();

    pstCommCtx->stPortBuffCfg.enSmsBuffCfg = AT_PORT_BUFF_DISABLE;

    return;
}

#else
/*****************************************************************************
 �� �� ��  : AT_ReadPortBuffCfgNV
 ��������  : ��ȡ�������NV��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : HSUART PHASE III

  2.��    ��   : 2014��2��14��
    ��    ��   : j00174725
    �޸�����   : TQE
*****************************************************************************/
VOS_VOID AT_ReadPortBuffCfgNV(VOS_VOID)
{
    AT_COMM_CTX_STRU                   *pstCommCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulRet;
    TAF_NV_PORT_BUFF_CFG_STRU           stSmsBuffCfg;

    pstCommCtx = AT_GetCommCtxAddr();

    /* Add by j00174725 for TQE, 2014-02-14, Begin */
    PS_MEM_SET(&stSmsBuffCfg, 0x0, sizeof(stSmsBuffCfg));
    /* Add by j00174725 for TQE, 2014-02-14, End */

    ulRet = NV_ReadEx(MODEM_ID_0, en_NV_Item_PORT_BUFF_CFG, &stSmsBuffCfg, sizeof(stSmsBuffCfg));

    if (NV_OK == ulRet)
    {
        if (stSmsBuffCfg.ucEnableFlg < AT_PORT_BUFF_BUTT)
        {
            pstCommCtx->stPortBuffCfg.enSmsBuffCfg = stSmsBuffCfg.ucEnableFlg;
        }
        else
        {
            pstCommCtx->stPortBuffCfg.enSmsBuffCfg = AT_PORT_BUFF_DISABLE;
        }
    }
    else
    {
        pstCommCtx->stPortBuffCfg.enSmsBuffCfg = AT_PORT_BUFF_DISABLE;
    }

    return;
}
#endif

/* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */

/*****************************************************************************
 �� �� ��  : AT_ReadSmsNV
 ��������  : ��ȡSMS��ص�NV��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_ReadSmsNV(VOS_VOID)
{
    AT_ReadSmsSendDomainNV();

    AT_ReadSmsMeStorageInfoNV();

    AT_ReadSmsClass0TailorNV();

    /* modified by l65478 for DTS2013052304352 2013-05-23 begin */
    AT_ReadSmsClosePathNV();
    /* modified by l65478 for DTS2013052304352 2013-05-23 end */

    /* Added by x00182239 for DTS2014120310853, 2015-1-14, Begin */
    AT_ReadMtCustomizeInfo();
    /* Added by x00182239 for DTS2014120310853, 2015-1-14, End */

    return;
}
/* Deleted by s00217060 for �����ϱ�AT�������������C��, 2013-4-1, begin */
/* Deleted by s00217060 for �����ϱ�AT�������������C��, 2013-4-1, end */

/*****************************************************************************
 �� �� ��  : AT_ReadRoamCapaNV
 ��������  : ��ȡ����������ص�NV��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��20��
    ��    ��   : m00217266
    �޸�����   : nv����
  3.��    ��   : 2013��10��08��
    ��    ��   : j00174725
    �޸�����   : TQE
*****************************************************************************/
VOS_VOID  AT_ReadRoamCapaNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-20, begin */
    NAS_NVIM_ROAM_CFG_INFO_STRU         stRoamCfgInfo;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    /* Modified by j00174725 for TQE, 2013-10-08, begin */
    ulLength = 0;
    PS_MEM_SET(&stRoamCfgInfo, 0x0, sizeof(stRoamCfgInfo));
    /* Modified by j00174725 for TQE, 2013-10-08, end */

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstNetCtx = AT_GetModemNetCtxAddrFromModemId(enModemId);

        NV_GetLength(en_NV_Item_Roam_Capa, &ulLength);
        if (NV_OK == NV_ReadEx(enModemId, en_NV_Item_Roam_Capa, &stRoamCfgInfo, ulLength))
        {
            if (stRoamCfgInfo.ucRoamFeatureFlg >1)
            {
                 pstNetCtx->ucRoamFeature = AT_ROAM_FEATURE_OFF;
            }
            else
            {
                pstNetCtx->ucRoamFeature = stRoamCfgInfo.ucRoamFeatureFlg;
            }
        }
        else
        {
            pstNetCtx->ucRoamFeature = AT_ROAM_FEATURE_OFF;
        }
    }
    /* modifyed by M00217266 for nv, 2013-05-20, end */
    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadSystemAppConfigNV
 ��������  : ��ȡ���Կ���NV�����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��17��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID  AT_ReadSystemAppConfigNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-17, begin */
    VOS_UINT8                          *pucSystemAppConfig;
    NAS_NVIM_SYSTEM_APP_CONFIG_STRU     stSysAppConfig;
    VOS_UINT32                          ulLength;

    stSysAppConfig.usSysAppConfigType = SYSTEM_APP_BUTT;

    /* ��ȡ�������Ե�NV�����ĵ�ַ */
    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();
    ulLength                            = 0;

    NV_GetLength(en_NV_Item_System_APP_Config, &ulLength);
    /*��ȡʧ�ܰ�Ĭ��ֵ���� */
    if (VOS_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_System_APP_Config,&stSysAppConfig,ulLength))
    {
        *pucSystemAppConfig  = SYSTEM_APP_MP;
        return;
    }

    if ( SYSTEM_APP_ANDROID < stSysAppConfig.usSysAppConfigType)
    {
        *pucSystemAppConfig  = SYSTEM_APP_MP;
    }
    else
    {
        *pucSystemAppConfig  = (VOS_UINT8)stSysAppConfig.usSysAppConfigType;
    }

    return;
    /* modifyed by M00217266 for nv, 2013-05-17, end */

}

/*****************************************************************************
 �� �� ��  : AT_ReadAtDislogPwdNV
 ��������  : ��ȡDISLOG����NV
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��17��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_ReadAtDislogPwdNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-17, begin */
    VOS_UINT8                          *pucSystemAppConfig;
    TAF_AT_NVIM_DISLOG_PWD_NEW_STRU     stDislogPwdNew;

    /* ��D25������  ������Ȩ�ޱ�־, ���ô�NV�ж�ȡȨ��, Ĭ����Ȩ�� */
    g_enATE5RightFlag = AT_E5_RIGHT_FLAG_NO;
    PS_MEM_SET(&stDislogPwdNew, 0x00, sizeof(stDislogPwdNew));

    /* ��ȡNV���е�ǰ��Ʒ��̬ */
    pucSystemAppConfig = AT_GetSystemAppConfigAddr();

    /* �¼�NV���DISLOG����(OPWORDʹ��) */
    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_AT_DISLOG_PWD_NEW,
                          &stDislogPwdNew,
                          AT_OPWORD_PWD_LEN))
    {
        PS_MEM_CPY((VOS_INT8*)g_acATOpwordPwd,
                   (VOS_INT8*)(&stDislogPwdNew),
                   AT_OPWORD_PWD_LEN);

        /* �����ַ�����β�� */
        g_acATOpwordPwd[AT_OPWORD_PWD_LEN] = 0;

        /* �������Ϊ�գ��ҷ�AP-Modem��̬����ȡ�����뱣�� */
        if ((0 == VOS_StrLen((VOS_CHAR *)g_acATOpwordPwd))
         && (SYSTEM_APP_ANDROID != *pucSystemAppConfig))
        {
            /* NV������Ϊ���������κ����붼�ܻ�ȡȨ�� */
            g_enATE5RightFlag = AT_E5_RIGHT_FLAG_YES;
        }
    }
    else
    {
        AT_WARN_LOG("AT_ReadAtDislogPwdNV:read en_NV_Item_AT_DISLOG_PWD_NEW failed");
    }

    return;
    /* modifyed by M00217266 for nv, 2013-05-17, end */
}

/*****************************************************************************
 �� �� ��  : AT_ReadAtRightPasswordNV
 ��������  : ��ȡ��ǰ�Ƿ���Ȩ�޲���AT�˿���Ϣ��NV
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
  3.��    ��   : 2014��5��30��
    ��    ��   : j00174725
    �޸�����   : TQE
*****************************************************************************/
VOS_VOID AT_ReadAtRightPasswordNV(VOS_VOID)
{
    TAF_AT_NVIM_RIGHT_OPEN_FLAG_STRU        stNvimRightOpenFlg;

    /* Add by j00174725 for TQE, 2014-05-30, Begin */
    PS_MEM_SET(&stNvimRightOpenFlg, 0x0, sizeof(stNvimRightOpenFlg));
    /* Add by j00174725 for TQE, 2014-05-30, End */

    /* ��NV�л�ȡ��ǰ����AT�����Ȩ�� */
    if (NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_AT_RIGHT_PASSWORD,
                           &stNvimRightOpenFlg,
                           sizeof(stNvimRightOpenFlg)))
    {
        /* ��ȡNVʧ��,����Ĭ������ */
        PS_MEM_SET(&g_stAtRightOpenFlg, 0x00, sizeof(g_stAtRightOpenFlg));
        AT_WARN_LOG("AT_ReadAtRightPasswordNV:read en_NV_Item_AT_RIGHT_PASSWORD failed");
    }

    PS_MEM_CPY(&g_stAtRightOpenFlg, &stNvimRightOpenFlg, sizeof(g_stAtRightOpenFlg));
    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadAtDissdPwdNV
 ��������  : ��ȡʹ�ܽ�ֹSD��ʱ��Ҫ������NV
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��17��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_ReadAtDissdPwdNV(VOS_VOID)
{
    /* modifyed by M00217266 for nv, 2013-05-17, begin */
    TAF_AT_NVIM_DISSD_PWD_STRU          stDissdPwd;

    PS_MEM_SET(&stDissdPwd, 0x00, AT_DISSD_PWD_LEN);

    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_AT_DISSD_PWD,
                           &stDissdPwd,
                           AT_DISSD_PWD_LEN))
    {
        PS_MEM_CPY((VOS_INT8*)g_acATE5DissdPwd,
                   (VOS_INT8*)(&stDissdPwd),
                   AT_DISSD_PWD_LEN);
        /* �����ַ�����β�� */
        g_acATE5DissdPwd[AT_DISSD_PWD_LEN] = 0;
    }
    else
    {
        AT_WARN_LOG("AT_ReadAtDissdPwdNV:read en_NV_Item_AT_DISSD_PWD failed");
    }

    return;
    /* modifyed by M00217266 for nv, 2013-05-17, end */
}

/*****************************************************************************
 �� �� ��  : AT_ReadNotSupportRetValueNV
 ��������  : ��NV�л�ȡ��֧������ķ���ֵ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
*****************************************************************************/
VOS_VOID AT_ReadNotSupportRetValueNV(VOS_VOID)
{
    VOS_UINT32                                  ulRetLen;
    VOS_CHAR                                    acRetVal[AT_NOTSUPPORT_STR_LEN+1];
    TAF_AT_NOT_SUPPORT_CMD_ERROR_TEXT_STRU      stErrorText;

    PS_MEM_SET(&stErrorText,  0x00, AT_NOTSUPPORT_STR_LEN);

     /* ��NV�л�ȡ��֧������ķ���ֵ����ΪE5��LCARD��DONGLEʱ��
        ��NV����Ϊ"ERROR",����ƽ̨����Ϊ"COMMAND NOT SUPPORT" */
    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_NOT_SUPPORT_RET_VALUE, stErrorText.acErrorText,
                           AT_NOTSUPPORT_STR_LEN))
    {
         PS_MEM_CPY(acRetVal,
                    stErrorText.acErrorText,
                    AT_NOTSUPPORT_STR_LEN);

         /* �����ַ�����β�� */
         acRetVal[AT_NOTSUPPORT_STR_LEN] = 0;
         ulRetLen                        = VOS_StrLen(acRetVal);
         if (ulRetLen > 0)
         {
             VOS_StrCpy((VOS_CHAR *)gaucAtCmdNotSupportStr, acRetVal);
         }
     }
     else
     {
         AT_WARN_LOG("AT_ReadNotSupportRetValueNV:read en_NV_Item_NOT_SUPPORT_RET_VALUE failed");
     }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadE5NV
 ��������  : ��ȡE5��صļ���NV��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadE5NV(VOS_VOID)
{
    AT_ReadAtDislogPwdNV();

    AT_ReadAtRightPasswordNV();

    AT_ReadAtDissdPwdNV();

    AT_ReadNotSupportRetValueNV();

    return;
}

/*****************************************************************************
 �� �� ��  : AT_IsAbortCmdCharValid
 ��������  : �жϴ��������ַ��Ƿ�Ϊ��Ч�ַ�.
 �������  : pucAbortCmdChar: ���������ַ���
             ulLen          : �ַ����ĳ���
 �������  : ��
 �� �� ֵ  : VOS_TRUE: ��Ч
             VOS_FALSE:��Ч
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��22��
    ��    ��   : lijun 00171473
    �޸�����   : �����ɺ��� for V7R1C50_At_Abort

*****************************************************************************/
VOS_UINT32 AT_IsAbortCmdCharValid(
    VOS_UINT8                          *pucAbortCmdChar,
    VOS_UINT32                          ulLen
)
{
    VOS_UINT32                          i;

    /* ������AT����Ĵ�����������, ���������ַ����и�ʽ������, ���»���˵����ɼ��ַ�
       (<0x20��ASCII�ַ�,�ո�, S3, S5)��.
       ���NV�����õ�ABORT�����ַ����а�����Щ�ַ��ᵼ�´������ƥ��ʧ��, �����ݴ�����,
       ���NV�����õ�ABORT�����ַ����а�����Щ�ַ�����Ϊ������Ч, ʹ��Ĭ��ֵ */

    /* ������������ϻظ�Ϊ��ָ�룬��Ч */
    if ( VOS_NULL_PTR == pucAbortCmdChar )
    {
        return VOS_FALSE;
    }

    /* ������������ϻظ�����Ϊ0����Ч */
    if ( 0 == ulLen )
    {
        return VOS_FALSE;
    }

    /* ������������ϻظ��в��ɼ��ַ�(<0x20��ASCII�ַ�,�ո�, S3, S5)�ȣ���Ч */
    for ( i = 0 ; i < ulLen; i++ )
    {
        if ( (pucAbortCmdChar[i] == ucAtS3)
          || (pucAbortCmdChar[i] == ucAtS5)
          || (pucAbortCmdChar[i] <= 0x20) )
        {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : AT_ReadAbortCmdParaNV
 ��������  : ��ȡAT��������NV��, �û������ô��������������ͷ���ֵ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��22��
    ��    ��   : lijun 00171473
    �޸�����   : �����ɺ��� for V7R1C50_At_Abort
  2.��    ��   : 2013��10��22��
    ��    ��   : w00242748
    �޸�����   : NETSCAN��Ŀ����ȡNV��ʱ������ȡ��ANY������Ϊ�����ַ����

*****************************************************************************/
VOS_VOID AT_ReadAbortCmdParaNV(VOS_VOID)
{
    AT_ABORT_CMD_PARA_STRU             *pstAbortCmdPara   = VOS_NULL_PTR;
    VOS_UINT8                           aucDfltAbortCmd[] = "AT";
    /* Modified by w00242748 for NETSCAN, 2013-10-20, begin */
    VOS_UINT8                           aucDfltAbortRsp[] = "OK";            /* ����Ƚ�ʱ�����ִ�Сд, �˴�Ĭ��ֵ����Ϊ��Сд���� */
    /* Modified by w00242748 for NETSCAN, 2013-10-20, end */
    AT_NVIM_ABORT_CMD_PARA_STRU         stNvAbortCmdPara;
    VOS_UINT32                          ulIsAbortCmdValid;
    VOS_UINT32                          ulIsAbortRspValid;
    VOS_UINT32                          ulRlst;
    /* Modified by w00242748 for NETSCAN, 2013-10-20, begin */
    VOS_UINT8                           aucAnyCharAbortCmd[] = "ANY";
    VOS_UINT32                          ulLen;
    /* Modified by w00242748 for NETSCAN, 2013-10-20, end */

    /* ��ʼ�� ����TQE�澯 */
    PS_MEM_SET(&stNvAbortCmdPara,  0x00, sizeof(stNvAbortCmdPara));

    /* ��������ʼ�� */
    pstAbortCmdPara = AT_GetAbortCmdPara();
    PS_MEM_SET(pstAbortCmdPara, 0x00, sizeof(AT_ABORT_CMD_PARA_STRU));

    /* ��ȡNV�������õĴ������ʹ�ϻظ� */
    ulRlst = NV_ReadEx(MODEM_ID_0, en_NV_Item_AT_ABORT_CMD_PARA,
                       &stNvAbortCmdPara,
                       sizeof(AT_NVIM_ABORT_CMD_PARA_STRU));

    /* NV��ȡʧ����ʹ��Ĭ��ֵ */
    if ( NV_OK != ulRlst )
    {
        /* ���ʹ�ܱ�־Ĭ��ֵΪTRUE */
        pstAbortCmdPara->ucAbortEnableFlg = VOS_TRUE;

        /* �������Ĭ��ֵ */
        VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtCmdStr),
                    (VOS_CHAR*)aucDfltAbortCmd,
                    AT_MAX_ABORT_CMD_STR_LEN);
        pstAbortCmdPara->aucAbortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';

        /* ��ϻظ�Ĭ��ֵ */
        VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtRspStr),
                    (VOS_CHAR *)aucDfltAbortRsp,
                    AT_MAX_ABORT_RSP_STR_LEN);
        pstAbortCmdPara->aucAbortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';

        AT_WARN_LOG("AT_ReadAbortCmdParaNV:read en_NV_Item_AT_ABORT_CMD_PARA failed");

        return;
    }

    /* �����Ϲ���δʹ��, ����Բ��ù�ע�������� */
    pstAbortCmdPara->ucAbortEnableFlg = stNvAbortCmdPara.ucAbortEnableFlg;
    if ( VOS_TRUE != pstAbortCmdPara->ucAbortEnableFlg )
    {
        return;
    }

    /* ��NV�д����������ݸ�ֵ��ȫ�ֱ�������ֵ����ΪAT_MAX_ABORT_CMD_STR_LEN��
       Ȼ�������AT_MAX_ABORT_CMD_STR_LEN+1���ַ���ֵΪ��������
       ��Ӧ�����±�ΪAT_MAX_ABORT_CMD_STR_LEN */
    VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtCmdStr),
                (VOS_CHAR *)(stNvAbortCmdPara.aucAbortAtCmdStr),
                AT_MAX_ABORT_CMD_STR_LEN);
    pstAbortCmdPara->aucAbortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';

    /* ��NV�д�ϻظ������ݸ�ֵ��ȫ�ֱ�������ֵ����ΪAT_MAX_ABORT_CMD_STR_LEN��
       Ȼ�������AT_MAX_ABORT_RSP_STR_LEN+1���ַ���ֵΪ��������
       ��Ӧ�����±�ΪAT_MAX_ABORT_RSP_STR_LEN */
    VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtRspStr),
                (VOS_CHAR *)(stNvAbortCmdPara.aucAbortAtRspStr),
                AT_MAX_ABORT_RSP_STR_LEN);
    pstAbortCmdPara->aucAbortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';


    /* �жϴ�������NV�������Ƿ�����Ч�ַ� */
    ulIsAbortCmdValid = AT_IsAbortCmdCharValid(pstAbortCmdPara->aucAbortAtCmdStr,
                                     VOS_StrLen((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtCmdStr)));

    ulIsAbortRspValid = AT_IsAbortCmdCharValid(pstAbortCmdPara->aucAbortAtRspStr,
                                     VOS_StrLen((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtRspStr)));

    /* ���NV�����к��в��ɼ��ַ�(�ո�, S3, S5, С��x020���ַ�), ��ʹ��Ĭ��ֵ */
    if ( VOS_TRUE != ulIsAbortCmdValid )
    {
        VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtCmdStr),
                    (VOS_CHAR*)aucDfltAbortCmd,
                    AT_MAX_ABORT_CMD_STR_LEN);
        pstAbortCmdPara->aucAbortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';

    }

    if ( VOS_TRUE != ulIsAbortRspValid )
    {
        VOS_StrNCpy((VOS_CHAR *)(pstAbortCmdPara->aucAbortAtRspStr),
                    (VOS_CHAR *)aucDfltAbortRsp,
                    AT_MAX_ABORT_RSP_STR_LEN);
        pstAbortCmdPara->aucAbortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';
    }

    /* Modified by w00242748 for NETSCAN, 2013-10-20, begin */
    ulLen = VOS_StrLen((VOS_CHAR *)pstAbortCmdPara->aucAbortAtCmdStr);

    if (VOS_StrLen((VOS_CHAR *)aucAnyCharAbortCmd) != ulLen)
    {
        return;
    }

    if ( (0 == VOS_StrNiCmp((VOS_CHAR *)aucAnyCharAbortCmd, (VOS_CHAR *)pstAbortCmdPara->aucAbortAtCmdStr, ulLen)) )
    {
        /* PS_MEM_SET��ʱ���Ѿ������ó�0����ΪĬ�ϲ�֧�������ַ���� */
        pstAbortCmdPara->ucAnyAbortFlg = VOS_TRUE;
    }
    /* Modified by w00242748 for NETSCAN, 2013-10-20, end */

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadSysNV
 ��������  : ��ȡϵͳ��ص�NV��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadSysNV(VOS_VOID)
{
    AT_ReadRoamCapaNV();

    AT_ReadSystemAppConfigNV();

    AT_ReadE5NV();

    AT_ReadAbortCmdParaNV();

    return;
}

/* Modified by z00161729 for cerssi and cnmr, 2012-11-21, begin */
/*****************************************************************************
 �� �� ��  : AT_ReadCellSignReportCfgNV
 ��������  : ��NV�ж�ȡ�ź������ϱ����������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��1��11��
    ��    ��   : t00212959
    �޸�����   : ֧��cerssi��������
  3.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
*****************************************************************************/
VOS_VOID AT_ReadCellSignReportCfgNV( VOS_VOID )
{
    AT_NV_CELL_SIGN_REPORT_CFG_STRU     stCellSignReportCfg;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    ulLength                            = 0;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstNetCtx = AT_GetModemNetCtxAddrFromModemId(enModemId);

        PS_MEM_SET(&stCellSignReportCfg, 0x0, sizeof(stCellSignReportCfg));

        NV_GetLength(en_NV_Item_CELL_SIGN_REPORT_CFG, &ulLength);
        if(NV_OK != NV_ReadEx(enModemId,
                              en_NV_Item_CELL_SIGN_REPORT_CFG,
                              &stCellSignReportCfg,
                              ulLength))
        {
             AT_WARN_LOG("AT_ReadCellSignReportCfgNV:read en_NV_Item_CELL_SIGN_REPORT_CFG failed");
             return;
        }

        pstNetCtx->ucCerssiReportType = stCellSignReportCfg.enSignThreshold;

        if (stCellSignReportCfg.enSignThreshold >= AT_CERSSI_REPORT_TYPE_BUTT)
        {
            pstNetCtx->ucCerssiReportType  = AT_CERSSI_REPORT_TYPE_NOT_REPORT;
        }

        /* Modified by t00212959 for DTS2013010809978, 2013-1-9, begin */
        pstNetCtx->ucCerssiMinTimerInterval = stCellSignReportCfg.ucMinRptTimerInterval;

        if (stCellSignReportCfg.ucMinRptTimerInterval > AT_CERSSI_MAX_TIMER_INTERVAL)
        {
            pstNetCtx->ucCerssiMinTimerInterval = 0;
        }
        /* Modified by t00212959 for DTS2013010809978, 2013-1-9, end */
        }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ValidateWasCategory
 ��������  : ���UE������Category��Ч��
 �������  : pstUeCapability - WAS��UE����NV��
 �������  : ��
 �� �� ֵ  : VOS_OK          - ��Ч
             VOS_ERR         - ��Ч
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_ValidateWasCategory(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    if (VOS_TRUE != pstUeCapability->ucHSDSCHSupport)
    {
        return VOS_ERR;
    }

    if ( (pstUeCapability->ucHSDSCHPhyCategory < 1)
      || (pstUeCapability->ucHSDSCHPhyCategory > 12) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : AT_ValidateWasCategoryExt
 ��������  : ���UE������CategoryExt��Ч��
 �������  : pstUeCapability - WAS��UE����NV��
 �������  : ��
 �� �� ֵ  : VOS_OK          - ��Ч
             VOS_ERR         - ��Ч
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_ValidateWasCategoryExt(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    if ( (VOS_TRUE != pstUeCapability->ucHSDSCHSupport)
      || (VOS_TRUE != pstUeCapability->ucMacEhsSupport) )
    {
        return VOS_ERR;
    }

    if ( (pstUeCapability->ucHSDSCHPhyCategoryExt < 13)
      || (pstUeCapability->ucHSDSCHPhyCategoryExt > 20) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : AT_ValidateWasCategoryExt2
 ��������  : ���UE������CategoryExt2��Ч��
 �������  : pstUeCapability - WAS��UE����NV��
 �������  : ��
 �� �� ֵ  : VOS_OK          - ��Ч
             VOS_ERR         - ��Ч
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_ValidateWasCategoryExt2(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    if ( (VOS_TRUE != pstUeCapability->ucHSDSCHSupport)
      || (VOS_TRUE != pstUeCapability->ucMacEhsSupport)
      || (VOS_TRUE != pstUeCapability->ucMultiCellSupport) )
    {
        return VOS_ERR;
    }

    if ( (pstUeCapability->ucHSDSCHPhyCategoryExt2 < 21)
      || (pstUeCapability->ucHSDSCHPhyCategoryExt2 > 24) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : AT_ValidateWasCategoryExt3
 ��������  : ���UE������CategoryExt3��Ч��
 �������  : pstUeCapability - WAS��UE����NV��
 �������  : ��
 �� �� ֵ  : VOS_OK          - ��Ч
             VOS_ERR         - ��Ч
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_ValidateWasCategoryExt3(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    if ( (VOS_TRUE != pstUeCapability->ucHSDSCHSupport)
      || (VOS_TRUE != pstUeCapability->ucMacEhsSupport)
      || (VOS_TRUE != pstUeCapability->ucMultiCellSupport)
      || (VOS_TRUE != pstUeCapability->ucDcMimoSupport) )
    {
        return VOS_ERR;
    }

    if ( (pstUeCapability->ucHSDSCHPhyCategoryExt3 < 25)
      || (pstUeCapability->ucHSDSCHPhyCategoryExt3 > 28) )
    {
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : AT_GetWasDefaultCategory
 ��������  :
 �������  : AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
 �������  : ��
 �� �� ֵ  : VOS_UINT8
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT8 AT_GetWasDefaultCategory(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    AT_RATE_WCDMA_CATEGORY_ENUM_UINT8   enRateCategory;

    switch (pstUeCapability->ucAsRelIndicator)
    {
        case PS_PTL_VER_R5:
        case PS_PTL_VER_R6:
            enRateCategory = AT_RATE_WCDMA_CATEGORY_10;
            break;

        case PS_PTL_VER_R7:
            enRateCategory = AT_RATE_WCDMA_CATEGORY_18;
            break;

        case PS_PTL_VER_R8:
            enRateCategory = AT_RATE_WCDMA_CATEGORY_28;
            break;

        default:
            enRateCategory = AT_RATE_WCDMA_CATEGORY_10;
            break;
    }

    return enRateCategory;
}

/*****************************************************************************
 �� �� ��  : AT_CalcWasCategory
 ��������  : ����WAS��UE����NV�����ݼ���WAS��CATEGORY
 �������  : pstUeCapability - WAS��UE����NV��
 �������  : ��
 �� �� ֵ  : VOS_UINT8
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��9��14��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT8 AT_CalcWasCategory(
    AT_NV_UE_CAPABILITY_STRU           *pstUeCapability
)
{
    AT_RATE_WCDMA_CATEGORY_ENUM_UINT8   enRateCategory;

    if (VOS_OK == AT_ValidateWasCategoryExt3(pstUeCapability))
    {
        return pstUeCapability->ucHSDSCHPhyCategoryExt3;
    }

    if (VOS_OK == AT_ValidateWasCategoryExt2(pstUeCapability))
    {
        return pstUeCapability->ucHSDSCHPhyCategoryExt2;
    }

    if (VOS_OK == AT_ValidateWasCategoryExt(pstUeCapability))
    {
        return pstUeCapability->ucHSDSCHPhyCategoryExt;
    }

    if (VOS_OK == AT_ValidateWasCategory(pstUeCapability))
    {
        return pstUeCapability->ucHSDSCHPhyCategory;
    }

    enRateCategory = AT_GetWasDefaultCategory(pstUeCapability);

    return enRateCategory;
}

/*****************************************************************************
 �� �� ��  : AT_ReadWasCapabilityNV
 ��������  : ��ȡWAS������ֵ: Э��汾��CATEGORY
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadWasCapabilityNV(VOS_VOID)
{
    AT_NV_UE_CAPABILITY_STRU            stNvUeCapability;
    VOS_UINT32                          ulResult;

    PS_MEM_SET(&stNvUeCapability, 0x00, sizeof(stNvUeCapability));

    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_WAS_RadioAccess_Capa_New,
                         &stNvUeCapability,
                         sizeof(AT_NV_UE_CAPABILITY_STRU));
    if (VOS_OK != ulResult)
    {
        /* NV��ȡʧ��, Э��汾Ĭ��ΪR99 */
        g_stAtDlRateCategory.cWasRelIndicator = PS_PTL_VER_R99;
        g_stAtDlRateCategory.ucWasCategory    = AT_RATE_WCDMA_R99;
    }
    else
    {
        g_stAtDlRateCategory.cWasRelIndicator = (VOS_INT8)stNvUeCapability.ucAsRelIndicator;
        g_stAtDlRateCategory.ucWasCategory    = AT_CalcWasCategory(&stNvUeCapability);
    }
}

/*****************************************************************************
 �� �� ��  : AT_CalcGasCategory
 ��������  : ����GAS��UE����NV�����ݼ���GAS��CATEGORY
 �������  : NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU         *pstHighMultislotclass
             VOS_UINT16                                  usGprsMultiSlotClass
             VOS_UINT16                                  usEgprsMultiSlotClass
             VOS_UINT16                                  usEgprsFlag
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��15��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_CalcGasCategory(
    NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU         *pstHighMultislotclass,
    VOS_UINT16                                  usGprsMultiSlotClass,
    VOS_UINT16                                  usEgprsMultiSlotClass,
    VOS_UINT16                                  usEgprsFlag
)
{
    /* Ĭ��ΪVOS_FALSE */
    g_stAtDlRateCategory.ucGasMultislotClass33Flg = VOS_FALSE;

    if((VOS_TRUE == pstHighMultislotclass->usHighMultislotClassFlg)
     &&(AT_GAS_HIGH_MULTISLOT_CLASS_0 == pstHighMultislotclass->usHighMultislotClass))
    {
        if ((VOS_FALSE  == usEgprsFlag)
          &&(AT_GAS_GRR_MULTISLOT_CLASS_MAX == usGprsMultiSlotClass))
        {
            g_stAtDlRateCategory.ucGasMultislotClass33Flg = VOS_TRUE;
        }

        if((VOS_TRUE == usEgprsFlag)
         &&(AT_GAS_GRR_MULTISLOT_CLASS_MAX == usEgprsMultiSlotClass))
        {
            g_stAtDlRateCategory.ucGasMultislotClass33Flg = VOS_TRUE;
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadGasCapabilityNV
 ��������  : ��ȡGAS������ֵ: Э��汾��CATEGORY
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadGasCapabilityNV(VOS_VOID)
{
    VOS_UINT32                                  ulResult;
    NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU          stHighMultislotclass;
    VOS_UINT16                                  usGprsMultiSlotClass;
    VOS_UINT16                                  usEgprsMultiSlotClass;
    VOS_UINT16                                  usEgprsFlag;

    stHighMultislotclass.usHighMultislotClassFlg = VOS_FALSE;
    stHighMultislotclass.usHighMultislotClass    = AT_GAS_HIGH_MULTISLOT_CLASS_0;

    usGprsMultiSlotClass  = 0;
    usEgprsMultiSlotClass = 0;
    usEgprsFlag           = 0;

    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_Egprs_Flag, &usEgprsFlag, sizeof(VOS_UINT16));
    if ( NV_OK != ulResult )
    {
        usEgprsFlag = 0x01;
    }

    ulResult = NV_ReadEx(MODEM_ID_0, en_Nv_Item_Gprs_Multi_Slot_Class, &usGprsMultiSlotClass, sizeof(VOS_UINT16));

    /* ��ȡʧ�ܣ�Ĭ�϶�ʱ϶�����ȼ�Ϊ12 */
    if ( NV_OK != ulResult )
    {
        usGprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    /* ��ʱ϶�����ȼ�ȡֵΪ1-12 */
    if (( 0 == usGprsMultiSlotClass )
      ||( AT_GAS_GRR_MULTISLOT_CLASS_MAX < usGprsMultiSlotClass ))
    {
        usGprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_Egprs_Multi_Slot_Class, &usEgprsMultiSlotClass, sizeof(VOS_UINT16));
    if ( NV_OK != ulResult )
    {
        usEgprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    /* ��ʱ϶�����ȼ�ȡֵΪ1-12 */
    if (( 0 == usEgprsMultiSlotClass )
      ||( AT_GAS_GRR_MULTISLOT_CLASS_MAX < usEgprsMultiSlotClass ))
    {
        usEgprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_GAS_High_Multislot_Class,&stHighMultislotclass, sizeof(NVIM_GAS_HIGH_MULTISLOT_CLASS_STRU));

    /* ��ȡʧ�ܣ�Ĭ��High Multislot Class ��Ч  */
    if ( NV_OK != ulResult )
    {
        return  ;
    }

    if((VOS_TRUE == stHighMultislotclass.usHighMultislotClassFlg)
     &&(AT_GAS_HIGH_MULTISLOT_CLASS_0 != stHighMultislotclass.usHighMultislotClass))
    {
        stHighMultislotclass.usHighMultislotClass = AT_GAS_HIGH_MULTISLOT_CLASS_0;
    }

    AT_CalcGasCategory(&stHighMultislotclass,
                       usGprsMultiSlotClass,
                       usEgprsMultiSlotClass,
                       usEgprsFlag);

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadPppDialErrCodeNV
 ��������  : ��NV�ж�ȡPPP���Ŵ������ϱ������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
*****************************************************************************/
VOS_VOID AT_ReadPppDialErrCodeNV( VOS_VOID )
{
    NAS_NV_PPP_DIAL_ERR_CODE_STRU       stPppErrRpt;
    VOS_UINT32                          ulResult;
    VOS_UINT32                          ulLength;

    stPppErrRpt.ucStatus         = VOS_FALSE;
    stPppErrRpt.ucErrCodeRpt     = PPP_DIAL_ERR_CODE_BUTT;
    ulLength                     = 0;

    NV_GetLength(en_NV_Item_PPP_DIAL_ERR_CODE, &ulLength);
    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_PPP_DIAL_ERR_CODE,
                         &stPppErrRpt,
                         ulLength);

    if ((ulResult == NV_OK)
     && (VOS_TRUE == stPppErrRpt.ucStatus)
     && (PPP_DIAL_ERR_CODE_ENABLE == stPppErrRpt.ucErrCodeRpt))

    {
        gucPppDialErrCodeRpt = PPP_DIAL_ERR_CODE_ENABLE;
    }
    else
    {
        gucPppDialErrCodeRpt = PPP_DIAL_ERR_CODE_DISABLE;
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadReportRegActFlgNV
 ��������  : ��NV�ж�ȡCREG/CGREG��ACT���������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��05��20��
    ��    ��   : m00217266
    �޸�����   : nv����
*****************************************************************************/
VOS_VOID AT_ReadReportRegActFlgNV( VOS_VOID )
{
    /* modifyed by M00217266 for nv, 2013-05-20, begin */
    VOS_UINT32                          ulResult;
    TAF_AT_NVIM_REPORT_REG_ACT_FLG_STRU stReportRegFlg;

    PS_MEM_SET(&stReportRegFlg, 0x00, sizeof(TAF_AT_NVIM_REPORT_REG_ACT_FLG_STRU));

    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_REPORT_REG_ACT_FLG,
                         &(stReportRegFlg.usReportRegActFlg),
                         sizeof(stReportRegFlg.usReportRegActFlg));
    if (NV_OK != ulResult)
    {
        g_usReportCregActParaFlg = VOS_FALSE;
    }
    else
    {
        g_usReportCregActParaFlg = stReportRegFlg.usReportRegActFlg;
    }

    return;
    /* modifyed by M00217266 for nv, 2013-05-20, end */
}

/*****************************************************************************
 �� �� ��  : AT_ReadCregAndCgregCiFourByteRptNV
 ��������  : ��NV�ж�ȡCREG/CGREG��CI�����Ƿ���4�ֽ��ϱ������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
*****************************************************************************/
VOS_VOID AT_ReadCregAndCgregCiFourByteRptNV( VOS_VOID )
{
    NAS_NV_CREG_CGREG_CI_FOUR_BYTE_RPT_STRU     stCiFourByteRpt;
    VOS_UINT32                                  ulResult;
    VOS_UINT32                                  ulLength;

    stCiFourByteRpt.ucStatus     = VOS_FALSE;
    stCiFourByteRpt.ucCiBytesRpt = 0;
    ulLength                     = 0;

    NV_GetLength(en_NV_Item_CREG_CGREG_CI_Four_Byte_Rpt, &ulLength);
    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_CREG_CGREG_CI_Four_Byte_Rpt,
                         &stCiFourByteRpt,
                         ulLength);
    if ((NV_OK == ulResult)
     && (VOS_TRUE == stCiFourByteRpt.ucStatus)
     && (CREG_CGREG_CI_RPT_FOUR_BYTE == stCiFourByteRpt.ucCiBytesRpt))
    {
        gucCiRptByte = CREG_CGREG_CI_RPT_FOUR_BYTE;
    }
    else
    {
        gucCiRptByte = CREG_CGREG_CI_RPT_TWO_BYTE;
    }

    return;
}

#if (FEATURE_ON == FEATURE_IPV6)
/*****************************************************************************
 �� �� ��  : AT_ReadIpv6CapabilityNV
 ��������  : ��NV�ж�ȡIPV6���������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_ReadIpv6CapabilityNV( VOS_VOID )
{
    AT_NV_IPV6_CAPABILITY_STRU          stNvIpv6Capability;
    VOS_UINT32                          ulRslt;
    AT_COMM_PS_CTX_STRU                *pstCommPsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulIpv6CapabilityValid;

    PS_MEM_SET(&stNvIpv6Capability, 0x00, sizeof(AT_NV_IPV6_CAPABILITY_STRU));

    pstCommPsCtx = AT_GetCommPsCtxAddr();

    ulRslt = NV_ReadEx(MODEM_ID_0,
                       en_NV_Item_IPV6_CAPABILITY,
                       &stNvIpv6Capability,
                       sizeof(AT_NV_IPV6_CAPABILITY_STRU));

    ulIpv6CapabilityValid = AT_PS_IsIpv6CapabilityValid(stNvIpv6Capability.ucIpv6Capablity);

    if ((NV_OK == ulRslt)
     && (VOS_TRUE == stNvIpv6Capability.ucStatus)
     && (VOS_TRUE == ulIpv6CapabilityValid))
    {
        pstCommPsCtx->ucIpv6Capability = stNvIpv6Capability.ucIpv6Capablity;
    }
    else
    {
        pstCommPsCtx->ucIpv6Capability = AT_IPV6_CAPABILITY_IPV4_ONLY;
    }

    return;
}

/* Added by l60609 for V9R1 IPv6&TAF/SM Project, 2013-4-24, begin */
/*****************************************************************************
 �� �� ��  : AT_ReadIpv6BackProcExtCauseNV
 ��������  : ��ȡNV���Ƶ�IPv4v6˫ջ���Ż��˴�����չԭ��ֵ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��17��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_ReadIpv6BackProcExtCauseNV(VOS_VOID)
{
    AT_PS_IPV6_BACKPROC_EXT_CAUSE_STRU *pstPsBackProcExtCause = VOS_NULL_PTR;
    TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU stNvBackProcExtCause;
    VOS_UINT32                          ulCauseNum;
    VOS_UINT32                          ulRslt;
    VOS_UINT32                          ulCnt;

    pstPsBackProcExtCause = &(AT_GetCommPsCtxAddr()->stIpv6BackProcExtCauseTbl);
    ulCauseNum            = 0;

    PS_MEM_SET(&stNvBackProcExtCause, 0x00, sizeof(TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU));

    ulRslt = NV_ReadEx(MODEM_ID_0,
                       en_NV_Item_IPV6_BACKPROC_EXT_CAUSE,
                       &stNvBackProcExtCause,
                       sizeof(TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU));

    if ( (NV_OK == ulRslt)
      && (VOS_TRUE == stNvBackProcExtCause.ulActiveFlag) )
    {
        for (ulCnt = 0 ; ulCnt < TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM; ulCnt++)
        {
            /* ���(E)SM Cause�Ƿ���Ч */
            if (!TAF_PS_IS_SM_CAUSE_VALID(stNvBackProcExtCause.aucSmCause[ulCnt]))
            {
                continue;
            }
            else
            {
                pstPsBackProcExtCause->aenPsCause[ulCauseNum] = TAF_PS_CONVERT_SM_CAUSE_TO_PS_CAUSE(stNvBackProcExtCause.aucSmCause[ulCnt]);
                ulCauseNum++;
            }
        }
    }

    pstPsBackProcExtCause->ulCauseNum = ulCauseNum;

    return;
}
#endif
/* Added by l60609 for V9R1 IPv6&TAF/SM Project, 2013-4-24, end */

/* Added by z00214637 for V3R3 Share-PDP Project, 2013-6-5, begin */
/*****************************************************************************
 �� �� ��  : AT_ReadSharePdpInfoNV
 ��������  : ��ȡNV���Ƶ�Share PDP����FLAG
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : z00214637
    �޸�����   : V3R3 Share-PDP��Ŀ����
*****************************************************************************/
VOS_VOID AT_ReadSharePdpInfoNV(VOS_VOID)
{
    TAF_NVIM_SHARE_PDP_INFO_STRU        stSharePdpInfo;
    AT_COMM_PS_CTX_STRU                *pstCommPsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulRslt;

    PS_MEM_SET(&stSharePdpInfo, 0x00, sizeof(TAF_NVIM_SHARE_PDP_INFO_STRU));

    pstCommPsCtx = AT_GetCommPsCtxAddr();

    ulRslt = NV_ReadEx(MODEM_ID_0,
                       en_NV_Item_SHARE_PDP_INFO,
                       &stSharePdpInfo,
                       sizeof(stSharePdpInfo));

    if (NV_OK == ulRslt)
    {
        pstCommPsCtx->ucSharePdpFlag = stSharePdpInfo.ucEnableFlag;
    }
    else
    {
        pstCommPsCtx->ucSharePdpFlag = VOS_FALSE;
    }

    return;
}
/* Added by z00214637 for V3R3 Share-PDP Project, 2013-6-5, end */

/*****************************************************************************
 �� �� ��  : AT_ReadDialConnectDisplayRateNV
 ��������  : ��NV�ж�ȡ����ϵͳ������ʾ���������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
  3.��    ��   : 2013��10��08��
    ��    ��   : j00174725
    �޸�����   : TQE
*****************************************************************************/
VOS_VOID AT_ReadDialConnectDisplayRateNV(VOS_VOID)
{
    VOS_UINT32                                              ulResult;
    AT_NVIM_DIAL_CONNECT_DISPLAY_RATE_STRU                  stDialConnectDisplayRate;

    /* Modified by j00174725 for TQE, 2013-10-08, begin */
    PS_MEM_SET(&stDialConnectDisplayRate, 0x0, sizeof(stDialConnectDisplayRate));
    /* Modified by j00174725 for TQE, 2013-10-08, end */


    ulResult = NV_ReadEx(MODEM_ID_0, en_NV_Item_DIAL_CONNECT_DISPLAY_RATE,
                         &stDialConnectDisplayRate,
                         sizeof(AT_NVIM_DIAL_CONNECT_DISPLAY_RATE_STRU));
    if (NV_OK != ulResult)
    {
        VOS_MemSet(&g_stDialConnectDisplayRate, 0, sizeof(g_stDialConnectDisplayRate));
		return;
    }

    g_stDialConnectDisplayRate.ucDpaConnectRate             = stDialConnectDisplayRate.ucDpaConnectRate;
    g_stDialConnectDisplayRate.ucEdgeConnectRate            = stDialConnectDisplayRate.ucEdgeConnectRate;
    g_stDialConnectDisplayRate.ucGprsConnectRate            = stDialConnectDisplayRate.ucGprsConnectRate;
    g_stDialConnectDisplayRate.ucGsmConnectRate             = stDialConnectDisplayRate.ucGsmConnectRate;
    g_stDialConnectDisplayRate.ucReserve                    = stDialConnectDisplayRate.ucReserve;
    g_stDialConnectDisplayRate.ucWcdmaConnectRate           = stDialConnectDisplayRate.ucWcdmaConnectRate;

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadPsNV
 ��������  : ��ȡPS����ص�NV��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
    �޸�����   : DSDA PHASE III
  2.��    ��   : 2013��6��4��
    ��    ��   : z00214637
    �޸�����   : V3R3 Share-PDP��Ŀ�޸�
*****************************************************************************/
VOS_VOID  AT_ReadPsNV(VOS_VOID)
{
    /* ��ȡ���Ŵ������ϱ�NV */
    AT_ReadPppDialErrCodeNV();

    /* ��ȡ�Ƿ��ϱ�CREG/CGREG��ACT��������NV */
    AT_ReadReportRegActFlgNV();

    /* ��ȡCREG/CGREG��CI�����Ƿ���4�ֽ��ϱ��Ŀ���NV(Vodafone����) */
    AT_ReadCregAndCgregCiFourByteRptNV();

#if (FEATURE_ON == FEATURE_IPV6)
    /* ��ȡIPV6������NV, ĿǰIPV6����ֻ��֧�ֵ�IPV4V6_OVER_ONE_PDP */
    AT_ReadIpv6CapabilityNV();

    /* Added by l60609 for V9R1 IPv6&TAF/SM Project, 2013-4-24, begin */
    /* ��ȡIPv6���˴�����չԭ��ֵ */
    AT_ReadIpv6BackProcExtCauseNV();
    /* Added by l60609 for V9R1 IPv6&TAF/SM Project, 2013-4-24, end */
#endif

    /* ��ȡ����ϵͳ������ʾ���ʶ���NV */
    AT_ReadDialConnectDisplayRateNV();

    /* ��ȡWCDMA����ȼ�NV, ����PS�򲦺������ϱ� */
    AT_ReadWasCapabilityNV();

    /* ��ȡGAS�Ƿ�֧��CLASS33 NV��Ŀ */
    AT_ReadGasCapabilityNV();

    AT_ReadCellSignReportCfgNV();

    /* Added by z00214637 for V3R3 Share-PDP Project, 2013-6-5, begin */
    AT_ReadSharePdpInfoNV();
    /* Added by z00214637 for V3R3 Share-PDP Project, 2013-6-5, end */
    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadCsNV
 ��������  : cs�����NV��Ķ�ȡ (����ss��CC��ص�NV��)
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��01��24��
    ��    ��   : f62575
    �޸�����   : DTS2013012408620, ֧��SS��ѯ�����������SS-STATUS����
  3.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : NV������Ŀ, ��NV�������ýṹ������
*****************************************************************************/
VOS_VOID  AT_ReadCsNV(VOS_VOID)
{
    TAF_USSD_NVIM_TRANS_MODE_STRU       stUssdTranMode;
    TAF_CCA_TELE_PARA_STRU              stTimeInfo;
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_CC_CTX_STRU               *pstCcCtx = VOS_NULL_PTR;
    AT_MODEM_SS_CTX_STRU               *pstSsCtx = VOS_NULL_PTR;
    VOS_UINT32                          ulLength;

    ulLength = 0;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        pstCcCtx = AT_GetModemCcCtxAddrFromModemId(enModemId);
        pstSsCtx = AT_GetModemSsCtxAddrFromModemId(enModemId);

        PS_MEM_SET(&stTimeInfo, 0x00, sizeof(stTimeInfo));

        /*��ȡ�����Զ�Ӧ�� */
        if(NV_OK == NV_ReadEx(enModemId,
                              en_NV_Item_CCA_TelePara,
                              &stTimeInfo,
                              sizeof(TAF_CCA_TELE_PARA_STRU)))
        {
            pstCcCtx->stS0TimeInfo.ucS0TimerLen = stTimeInfo.ucS0TimerLen;
        }

        PS_MEM_SET(&stUssdTranMode, 0x00, sizeof(stUssdTranMode));

        NV_GetLength(en_NV_Item_CUST_USSD_MODE, &ulLength);
        /*��ȡUSSD�Ƿ�͸������ */
        if (NV_OK == NV_ReadEx(MODEM_ID_0,
                               en_NV_Item_CUST_USSD_MODE,
                               &stUssdTranMode,
                               ulLength))
        {
            if (VOS_TRUE == stUssdTranMode.ucStatus)
            {
                pstSsCtx->usUssdTransMode = stUssdTranMode.ucUssdTransMode;
            }
        }

    }

    /* Added by f62575 for DTS2013012408620, 2013-01-24, begin */
    AT_ReadSsNV();
    /* Added by f62575 for DTS2013012408620, 2013-01-24, end */

    return;
}

/*DTS2012042501159 w00182550 start in 2012-04-25*/
#if (FEATURE_ON == FEATURE_LTE)
/*****************************************************************************
 �� �� ��  : AT_ReadLTENV
 ��������  : ��ȡLTE��ص�NV��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��4��28��
    ��    ��   : w00182550
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��07��222��
    ��    ��   : j00177245
    �޸�����   : ��������warning
*****************************************************************************/
/*lint -e813*/
VOS_VOID AT_ReadLTENV(VOS_VOID)
{
    VOS_UINT32 ulResult = NV_OK;
    LRRC_NV_UE_EUTRA_CAP_STRU   *pstUECapability = VOS_NULL_PTR;

    /* Added by j00174725 for clean warning, 2013-07-22, Begin */
    PS_MEM_SET(&g_stRsrpCfg, 0, sizeof(g_stRsrpCfg));
    PS_MEM_SET(&g_stRscpCfg, 0, sizeof(g_stRscpCfg));
    PS_MEM_SET(&g_stEcioCfg, 0, sizeof(g_stEcioCfg));
    /* Added by j00174725 for clean warning, 2013-07-22, End */

    /* ��NV�л�ȡ����ֵ */
    ulResult = NVM_Read(EN_NV_ID_RSRP_CFG, &g_stRsrpCfg, sizeof(NVIM_RSRP_CFG_STRU));

    if(NV_OK != ulResult)
    {
        vos_printf("read RSRP NV fail!\n");
        return ;
    }

    /* ��NV�л�ȡ����ֵ */
    ulResult = NVM_Read(EN_NV_ID_RSCP_CFG, &g_stRscpCfg, sizeof(NVIM_RSCP_CFG_STRU));

    if(NV_OK != ulResult)
    {
        vos_printf("read RSCP NV fail!\n");
        return ;
    }

    /* ��NV�л�ȡ����ֵ */
    ulResult = NVM_Read(EN_NV_ID_ECIO_CFG, &g_stEcioCfg, sizeof(NVIM_ECIO_CFG_STRU));

    if(NV_OK != ulResult)
    {
        vos_printf("read ECIO NV fail!\n");
        return ;
    }

    /*DTS2012051201659 w00182550 start in 2012-05-09*/
    /* ����warning�����ڽṹ��̫���޸�Ϊ�ڴ���䷽ʽ j00174725*/
    pstUECapability =(LRRC_NV_UE_EUTRA_CAP_STRU*)PS_MEM_ALLOC(WUEPS_PID_AT,
                            sizeof(LRRC_NV_UE_EUTRA_CAP_STRU));

    if(VOS_NULL_PTR == pstUECapability)
    {
        vos_printf("AT_ReadLTENV: alloc mem fail!\n");
        return ;
    }

    PS_MEM_SET(pstUECapability, 0, sizeof(LRRC_NV_UE_EUTRA_CAP_STRU));
    /*DTS2012051201659 w00182550 end in 2012-05-09*/

    /*DTS2012051201659 w00182550 start in 2012-05-09*/
    /* ��NV�л�ȡUE ����ֵ */
    ulResult = NVM_Read(EN_NV_ID_UE_CAPABILITY, pstUECapability, sizeof(LRRC_NV_UE_EUTRA_CAP_STRU));

    if(NV_OK != ulResult)
    {
        vos_printf("read UE_CAPABILITY NV fail!\n");
        PS_MEM_FREE(WUEPS_PID_AT, pstUECapability);
        return ;
    }
    else
    {
        g_stAtDlRateCategory.ucLteUeCategory = pstUECapability->enUeCatg;
    };
    /*DTS2012051201659 w00182550 end in 2012-05-09*/

    PS_MEM_FREE(WUEPS_PID_AT, pstUECapability);
}
/*lint +e813*/
/* DTS2012042501159 w00182550 end in 2012-04-25 */
#endif

/*****************************************************************************
 �� �� ��  : AT_ReadNV
 ��������  : AT��ʼ��ʱ��ȡNV
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

  2.��    ��   : 2013��09��21��
    ��    ��   : j00174725
    �޸�����   : UART-MODEM: ����UART����NV��ȡ
*****************************************************************************/
VOS_VOID  AT_ReadNV(VOS_VOID)
{
    /* ƽ̨���뼼��NV��ȡ */
    AT_ReadPlatformNV();

    /* client NV��ȡ */
    AT_ReadClientConfigNV();

    /* Deleted by s00217060 for �����ϱ�AT�������������C��, 2013-4-3, begin */
    /* Deleted by s00217060 for �����ϱ�AT�������������C��, 2013-4-3, end */

    /* ��ȡϵͳ��ص�NV�� */
    AT_ReadSysNV();

    /* ��ȡPS����ص�NV�� */
    AT_ReadPsNV();

    /* ��ȡcs��CC��SS��ص�NV) */
    AT_ReadCsNV();

    /* ��ȡSMS��ص�NV */
    AT_ReadSmsNV();

#if (FEATURE_ON == FEATURE_LTE)
    /* ��ȡLTE NV�� */
    AT_ReadLTENV();
#endif

#if (FEATURE_ON == FEATURE_AT_HSUART)
    /* Added by j00174725 for UART-MODEM Project, 2013-09-21, begin */
    /* ��ȡuart������NV�� */
    AT_ReadUartCfgNV();
    /* Added by j00174725 for UART-MODEM Project, 2013-09-21, end */
#endif

    /* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
    AT_ReadPortBuffCfgNV();
    /* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */

    return;
}

/*****************************************************************************
 �� �� ��  : AT_ReadPhyNV
 ��������  : ��ȡ��������ص�NV
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��4��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32  AT_ReadPhyNV(VOS_VOID)
{
    VOS_BOOL                            bImeiIsNull;
    MODEM_ID_ENUM_UINT16                enModemId;

    for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
    {
        if (AT_OK != AT_PhyNumIsNull(enModemId, AT_PHYNUM_TYPE_IMEI, &bImeiIsNull))
        {
            return VOS_ERR;
        }

        if (VOS_TRUE != bImeiIsNull)
        {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : AT_InitDeviceCmd
 ��������  : װ����س�ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��3��21��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_InitDeviceCmd(VOS_VOID)
{
    if (VOS_OK != AT_ReadPhyNV())
    {
        return;
    }

    /* IMEIΪȫ��ʱ����ΪDATALOCK����״̬ */
    g_bAtDataLocked = VOS_FALSE;

    return;
}

/*****************************************************************************
 �� �� ��  : AT_InitStk
 ��������  : STK��ATģ��ĳ�ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_InitStk(VOS_VOID)
{
    USIMM_STK_CFG_STRU                  stProfile;

    PS_MEM_SET(&stProfile, 0x00, sizeof(stProfile));

    if(NV_OK != NV_ReadEx(MODEM_ID_0, en_NV_Item_TerminalProfile_Set, &stProfile, sizeof(USIMM_STK_CFG_STRU)))
    {
        AT_ERR_LOG("AT_StkInit: read en_NV_Item_TerminalProfile_Set fail.");

        g_ulSTKFunctionFlag = VOS_FALSE;
    }
    else
    {
        g_ulSTKFunctionFlag = stProfile.ucFuncEnable;
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_InitPara
 ��������  : ATģ������ĳ�ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID AT_InitPara(VOS_VOID)
{
    VOS_UINT8                          *pucSystemAppConfig;
    VOS_UINT8                           ucAtCmdNotSupportStr1[]   = "ERROR";
    VOS_UINT8                           ucAtCmdNotSupportStr2[]   = "COMMAND NOT SUPPORT";
    MODEM_ID_ENUM_UINT16                enModemId;
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    pucSystemAppConfig                  = AT_GetSystemAppConfigAddr();

    /* ���ݲ�Ʒ���ͷֱ���д��� */
    switch(*pucSystemAppConfig)
    {
        /* E5 */
        case SYSTEM_APP_WEBUI:

            pstNetCtx = AT_GetModemNetCtxAddrFromModemId(MODEM_ID_0);

            /*E5�汾�Բ�֧�������ERROR*/
            PS_MEM_CPY((VOS_CHAR*)gaucAtCmdNotSupportStr,
                       (VOS_CHAR*)ucAtCmdNotSupportStr1,
                       sizeof(ucAtCmdNotSupportStr1));

            /* E5��Ĭ�ϸ�ʽ��Ϊ���ָ�ʽ */
            pstNetCtx->ucCopsFormatType = AT_COPS_NUMERIC_TYPE;

            /* E5������Ĭ�ϴ����� */
            gucAtCmeeType   = AT_CMEE_ERROR_CODE;

            break;

         /* STICK*/
        case SYSTEM_APP_MP:

            pstNetCtx = AT_GetModemNetCtxAddrFromModemId(MODEM_ID_0);

            /*���֧����ʾ�ִ�*/
            PS_MEM_CPY((VOS_CHAR*)gaucAtCmdNotSupportStr,
                       (VOS_CHAR*)ucAtCmdNotSupportStr2,
                       sizeof(ucAtCmdNotSupportStr2));

            pstNetCtx->ucCopsFormatType = AT_COPS_LONG_ALPH_TYPE;

            /* ָʾ��������������� */
            gucAtCmeeType   = AT_CMEE_ERROR_CONTENT;


            break;

        /* PAD,Ŀǰû�У�Ϊ������ԭ����һ���ԣ��ݰ�stick����*/
        case SYSTEM_APP_ANDROID:

            /*���֧����ʾ�ִ�*/
            PS_MEM_CPY((VOS_CHAR*)gaucAtCmdNotSupportStr,
                       (VOS_CHAR*)ucAtCmdNotSupportStr2,
                       sizeof(ucAtCmdNotSupportStr2));

            for (enModemId = 0; enModemId < MODEM_ID_BUTT; enModemId++)
            {
                pstNetCtx = AT_GetModemNetCtxAddrFromModemId(enModemId);
                pstNetCtx->ucCopsFormatType = AT_COPS_LONG_ALPH_TYPE;
            }

            /* ָʾ��������������� */
            gucAtCmeeType   = AT_CMEE_ERROR_CONTENT;

            break;

        default:
            break;
    }

    return;
}

/*****************************************************************************
 �� �� ��  : AT_UsbSwitchGwMode
 ��������  : �����翨�л�������ģʽ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��1��14��
    ��    ��   : A00165503
    �޸�����   : DTS2012042104301: PCUI�˿ڷ���NDIS/APP���������Ż�
  3.��    ��   : 2013��4��13��
    ��    ��   : y00213812
    �޸�����   : ����DTS2012042104301���޸�ClientId
  4.��    ��   : 2013��07��08��
    ��    ��   : Y00213812
    �޸�����   : VoLTE_PhaseI ��Ŀ������PID
*****************************************************************************/
VOS_VOID AT_UsbSwitchGwMode(VOS_VOID)
{
    AT_PDP_ENTITY_STRU                 *pstAppPdpEntity;
    VOS_UINT32                          ulResult;
    VOS_UINT8                           aucCid[TAF_MAX_CID+1];

    pstAppPdpEntity                     = AT_APP_GetPdpEntInfoAddr();

    g_enHiLinkMode                      = AT_HILINK_GATEWAY_MODE;

    /* ��ʼ��CID�б� */
    PS_MEM_SET(aucCid, 0x00, sizeof(aucCid));

    /* ������е�PDP������IDLE, ����; �����һ������DEACTING, ���� */
    ulResult = AT_AppCheckPdpIdleState();
    if (VOS_TRUE == ulResult)
    {
        AT_NORM_LOG("AT_UsbSwitchGwMode: The state is already IDLE.");
        return;
    }

    /* Modified by A00165503 for DTS2012042104301, 2013-01-14, Begin */
    if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4v6State)
      || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4v6State) )
    {
        if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                     gastAtClientTab[AT_CLIENT_TAB_APP_INDEX].usClientId,
                                     0,
                                     g_stAtAppPdpEntity.ucIpv4v6Cid))
        {
            AT_WARN_LOG("AT_UsbSwitchGwMode(): Disconnect IPv4v6 fail");
        }

        AT_AppSetPdpState(TAF_PDP_IPV4V6, AT_PDP_STATE_DEACTING);
    }
    else
    {
        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv4State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv4State) )
        {
            if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                         gastAtClientTab[AT_CLIENT_TAB_APP_INDEX].usClientId,
                                         0,
                                         pstAppPdpEntity->ucIpv4Cid))
            {
                AT_WARN_LOG("AT_UsbSwitchGwMode():TAF_PS_CallEnd fail");
            }

            AT_AppSetPdpState(TAF_PDP_IPV4, AT_PDP_STATE_DEACTING);
        }

        if ( (AT_PDP_STATE_ACTING == pstAppPdpEntity->enIpv6State)
          || (AT_PDP_STATE_ACTED  == pstAppPdpEntity->enIpv6State) )
        {
            if (VOS_OK != TAF_PS_CallEnd(WUEPS_PID_AT,
                                         gastAtClientTab[AT_CLIENT_TAB_APP_INDEX].usClientId,
                                         0,
                                         pstAppPdpEntity->ucIpv6Cid))
            {
                AT_WARN_LOG("AT_UsbSwitchGwMode():TAF_PS_CallEnd fail");
            }

            AT_AppSetPdpState(TAF_PDP_IPV6, AT_PDP_STATE_DEACTING);
        }
    }
    /* Modified by A00165503 for DTS2012042104301, 2013-01-14, End */

    return;
}

/* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
/*****************************************************************************
 �� �� ��  : AT_UsbEnableCB
 ��������  : ע��USB��ض˿�ʹ�ܻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_UsbEnableCB(VOS_VOID)
{
    /* USB MODEM����·�Ľ��� */
    AT_UsbModemInit();

    /* ע���NCM�Ľӿ�,NCM�˿ڴ򿪺͹ر���DRV����Э��ջ�ӿڸ���򿪻�ر� */
    AT_OpenUsbNdis();
}

/*****************************************************************************
 �� �� ��  : AT_UsbDisableCB
 ��������  : ע��USB��ض˿�ȥʹ�ܻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_UsbDisableCB(VOS_VOID)
{
    vos_printf("AT_UsbDisableCB\r\n");
    /* USB MODEM����·�Ľ��� */
    AT_UsbModemClose();

    /* ע��ر�NCM�Ľӿ�,NCM�˿ڴ򿪺͹ر���DRV����Э��ջ�ӿڸ���򿪻�ر� */
    AT_CloseUsbNdis();

    /* �Ӽ�¼����ɾ��USB��ص�client ID */
    AT_RmUsedClientIdFromTab(AT_CLIENT_ID_PCUI);
    AT_RmUsedClientIdFromTab(AT_CLIENT_ID_CTRL);
    AT_RmUsedClientIdFromTab(AT_CLIENT_ID_NDIS);
    AT_RmUsedClientIdFromTab(AT_CLIENT_ID_MODEM);
}

#if (FEATURE_ON == FEATURE_AT_HSIC)
/*****************************************************************************
 �� �� ��  : AT_HsicEnableCB
 ��������  : ע��Hsic��ض˿�ʹ�ܻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_HsicEnableCB(VOS_VOID)
{
    AT_HsicInit();

    AT_HsicModemInit();
}

/*****************************************************************************
 �� �� ��  : AT_HsicDisableCB
 ��������  : ע��HSIC��ض˿�ȥʹ�ܻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID AT_HsicDisableCB(VOS_VOID)
{
    VOS_UINT8                           ucLoop;

    AT_HsicModemClose();

    /* �Ӽ�¼����ɾ��HSIC��ص�client ID */
    for (ucLoop = 0; ucLoop < AT_HSIC_AT_CHANNEL_MAX; ucLoop++)
    {
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_HSIC1 + ucLoop);
    }

    /* �Ӽ�¼����ɾ��MUX��ص�client ID */
    for (ucLoop = 0; ucLoop < AT_MUX_AT_CHANNEL_MAX; ucLoop++)
    {
        AT_RmUsedClientIdFromTab(AT_CLIENT_ID_MUX1 + ucLoop);
    }

    /* �Ӽ�¼����ɾ��HSIC_MODEM��ص�client ID */
    AT_RmUsedClientIdFromTab(AT_CLIENT_ID_HSIC_MODEM);
}
/* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */

#endif

/*****************************************************************************
 �� �� ��  : AT_InitPort
 ��������  : AT�˿ڳ�ʼ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��1��22��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2013��05��22��
    ��    ��   : f00179208
    �޸�����   : V3R3 PPP PROJECT

  3.��    ��   : 2013��9��21��
    ��    ��   : j00174725
    �޸�����   : UART-MODEM: ����HSUART�˿�
*****************************************************************************/
VOS_VOID AT_InitPort(VOS_VOID)
{
    /* Added by j00174725 for UART-MODEM Project, 2013-09-21, begin */
    VOS_UINT8                           i;

    for (i = 0; i < AT_CLIENT_BUTT; i++)
    {
        g_alAtUdiHandle[i] = UDI_INVALID_HANDLE;
    }
    /* Added by j00174725 for UART-MODEM Project, 2013-09-21, end */

    /* Ϊ�˱�֤PC�طŹ��̺� NAS GTR PC������SDT USBCOM AT�˿ڵ�ע��client ID
       ��ͬ��������USB COM�ڵ�һ��ע�� */
    /* USB PCUI����·�Ľ��� */
    At_UsbPcuiEst(AT_USB_COM_PORT_NO);

    /* USB Control����·�Ľ��� */
    At_UsbCtrEst(AT_CTR_PORT_NO);

    /* UART����·�Ľ��� */
    /* Modified by j00174725 for UART-MODEM Project, 2013-09-21, begin */
    AT_UART_InitPort();

#if (FEATURE_ON == FEATURE_AT_HSUART)
    AT_HSUART_InitPort();
#endif
    /* Modified by j00174725 for UART-MODEM Project, 2013-09-21, end */

    /* NDIS MODEM����·�Ľ��� */
    AT_UsbNdisEst();

    /* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
    mdrv_usb_reg_enablecb((USB_UDI_ENABLE_CB_T)AT_UsbEnableCB);
    mdrv_usb_reg_disablecb((USB_UDI_ENABLE_CB_T)AT_UsbDisableCB);
    /* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */

    /* APP ����ͨ�� */
    AT_AppComEst();

    /* ע��DRV�ص�����ָ�� */
    if (VOS_ERROR == DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC((USB_NET_DEV_SWITCH_GATEWAY)AT_UsbSwitchGwMode))
    {
        AT_ERR_LOG( "AT_InitPort: DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC Failed!" );
    }

    AT_SockComEst(AT_SOCK_PORT_NO);

    AT_AppSockComEst(AT_APP_SOCK_PORT_NO);

#if (FEATURE_ON == FEATURE_AT_HSIC)
    /* ���HSICͨ���Ѿ�ö�ٳɹ�������Э��ջִ�г�ʼ�����������򽫳�ʼ������ע����������
        �ɵ�����HSICö�ٳɹ�������Խ��г�ʼ�� */
    if (VOS_TRUE == DRV_GET_HSIC_ENUM_STATUS())
    {
        if ( AT_SUCCESS != AT_HsicInit() )
        {
            AT_ERR_LOG("AT_InitPort: HSIC AT Establishment Failed");
        }

        /* Added by f00179208 for V3R3 PPP RPOJECT 2013-05-25, Begin */
        AT_HsicModemInit();
        /* Added by f00179208 for V3R3 PPP RPOJECT 2013-05-25, End */
    }
    else
    {

#if (VOS_OS_VER == VOS_WIN32)
        AT_HsicInit();
#endif
        /* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
        DRV_HSIC_REGUDI_ENABLECB((HSIC_UDI_ENABLE_CB_T)AT_HsicEnableCB);
        DRV_HSIC_REGUDI_DISABLECB((HSIC_UDI_ENABLE_CB_T)AT_HsicDisableCB);
        /* Modify by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */
    }

    AT_MuxInit();
#endif

}
/*****************************************************************************
 �� �� ��  : At_PidInit
 ��������  : AT PID��ʼ������
 �������  : enum VOS_INIT_PHASE_DEFINE enPhase
 �������  : ��
 �� �� ֵ  : VOS_UINT32
             VOS_OK
             VOS_ERR
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��03��06��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

  2.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : HSUART PHASE III
*****************************************************************************/
VOS_UINT32  At_PidInit(enum VOS_INIT_PHASE_DEFINE enPhase)
{
    switch ( enPhase )
    {
        case VOS_IP_INITIAL:

            /* ��ʼ��AT�������� */
            AT_InitCtx();

#if (FEATURE_ON == FEATURE_AT_HSUART)
            /* Added by z00189113 for UART-MODEM Project, 2013-9-22 Begin */
            /* ��ʼ��UART��ص������� */
            AT_InitUartCtx();
            /* Added by z00189113 for UART-MODEM Project, 2013-9-22 End */
#endif

            /* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, Begin */
            AT_InitPortBuffCfg();
            /* Add by j00174725 for V3R3 HSUART PHASEIII, 2013-12-28, End */

            /* Add by j00174725 for V3R3 Cut Out Memory, 2013-11-06, Begin */
            AT_InitTraceMsgTab();
            /* Add by j00174725 for V3R3 Cut Out Memory, 2013-11-06, End */

            /* Added by f00179208 for CCPU RESET, 2013-04-15, Begin */
            /* ��ʼ����λ��ص������� */
            AT_InitResetCtx();
            /* Added by f00179208 for CCPU RESET, 2013-04-15, End */

            /*��ȡNV��*/
            AT_ReadNV();

            /*AT ��������ʼ��*/
            At_ParseInit();

            /*ע��AT�����*/
            At_RegisterBasicCmdTable();
            At_RegisterExCmdTable();
            At_RegisterExPrivateCmdTable();
            At_RegisterDeviceCmdTable();
#if (FEATURE_LTE == FEATURE_ON)
            At_RegisterDeviceCmdTLTable();
            At_RegisterTLCmdTable();                  /* ADDED by c64416 for V9R1/V7R1 AT, 2013/09/18 */
#endif
            /* װ����ʼ�� */
            AT_InitDeviceCmd();

            /* STK��ATģ��ĳ�ʼ�� */
            AT_InitStk();

            /* ATģ������ĳ�ʼ�� */
            AT_InitPara();

            /* �˿ڳ�ʼ�� */
            AT_InitPort();

            /* ��ʼ��g_stFcIdMaptoFcPri */
            AT_InitFcMap();

            /* ������ģ��ע�ᶯ̬����NCM/ECM��������Ļص��ӿ� */
            FC_ACORE_RegDrvAssemFunc((FC_ACORE_DRV_ASSEMBLE_PARA_FUNC)AT_UsbEthDeviceAccumTuneCB,
                                     (FC_ACORE_DRV_ASSEMBLE_PARA_FUNC)AT_UsbEthHostAccumTuneCB);


            /* Added by f00179208 for CCPU RESET, 2013-04-10, Begin */
            /* ������ע��ص�����������C�˵�����λ�Ĵ��� */
            mdrv_sysboot_register_reset_notify(NAS_AT_FUNC_PROC_NAME,
                                               AT_CCpuResetCallback,
                                               0,
                                               ACPU_RESET_PRIORITY_AT);

            /* ������ע��ص�����������HIFI������λ�Ĵ��� */
            hifireset_regcbfunc(NAS_AT_FUNC_PROC_NAME,
                                    AT_HifiResetCallback,
                                    0,
                                    ACPU_RESET_PRIORITY_AT);
            /* Added by f00179208 for CCPU RESET, 2013-04-10, End */

            /* Added by g00261581 for AT mntn, 2014-09-13, Begin */
            AT_InitMntnCtx();
            /* Added by g00261581 for AT mntn, 2014-09-13, end */

            break;

        default:
            break;
    }

    return VOS_OK;
}

/* Added by f62575 for DTS2013012403505, 2013-01-24, begin */
/*****************************************************************************
 �� �� ��  : AT_ReadSsNV
 ��������  : ��NV�ж�ȡSS��ض��Ʋ���
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��01��24��
    ��    ��   : f62575
    �޸�����   : DTS2013012408620, ֧��SS��ѯ�����������SS-STATUS����

*****************************************************************************/
VOS_VOID AT_ReadSsNV( VOS_VOID )
{
    PS_MEM_SET(&g_stAtSsCustomizePara, 0, sizeof(g_stAtSsCustomizePara));

    /* ��ȡSS���Ƶ�NV�ȫ�ֱ��� */
    if(NV_OK != NV_ReadEx(MODEM_ID_0,
                          en_NV_Item_SS_CUSTOMIZE_PARA,
                          &g_stAtSsCustomizePara,
                          sizeof(g_stAtSsCustomizePara)))
    {
        g_stAtSsCustomizePara.ucStatus = VOS_FALSE;
    }

    return;
}
/* Added by f62575 for DTS2013012403505, 2013-01-24, end */

#if (FEATURE_ON == FEATURE_AT_HSUART)
/* Added by j00174725 for UART-MODEM Project, 2013-09-21, begin */
/*****************************************************************************
 �� �� ��  : AT_ReadUartCfgNV
 ��������  : ��ȡUART��ص�NV��
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��09��18��
    ��    ��   : j00174725
    �޸�����   : �����ɺ���

  2.��    ��   : 2014��02��12��
    ��    ��   : j00174725
    �޸�����   : �޸�֡��ʽ��У��֤���жϷ�ʽ
*****************************************************************************/
VOS_VOID AT_ReadUartCfgNV(VOS_VOID)
{
    AT_UART_CTX_STRU                   *pstUartCtx          = VOS_NULL_PTR;
    /* Deleted by w00176964 for V3R3C60_eCall��Ŀ, 2014-6-6, begin */
    
    /* Deleted by w00176964 for V3R3C60_eCall��Ŀ, 2014-6-6, end */
    TAF_NV_UART_CFG_STRU                stUartNVCfg;
    VOS_UINT32                          ulRet;
    VOS_UINT32                          ulParityRet;
    VOS_UINT32                          ulFormatRet;

    PS_MEM_SET(&stUartNVCfg, 0x00, sizeof(TAF_NV_UART_CFG_STRU));

    pstUartCtx = AT_GetUartCtxAddr();

    /* ��ȡNV�� */
    ulRet = NV_ReadEx(MODEM_ID_0,
                      en_NV_Item_UART_CFG,
                      &stUartNVCfg,
                      sizeof(TAF_NV_UART_CFG_STRU));

    if (VOS_OK == ulRet)
    {
         /* ���NV�����õĲ������Ƿ���֧�ֵķ�Χ�� */
        ulRet = AT_HSUART_IsBaudRateValid(stUartNVCfg.ulBaudRate);
        if (VOS_TRUE == ulRet)
        {
            /* ��NV�е�ֵ����������ȫ�ֱ��� */
            pstUartCtx->stPhyConfig.enBaudRate = stUartNVCfg.ulBaudRate;
        }
        else
        {
            AT_HSUART_DBG_NV_BAUDRATE_ERR(1);
            pstUartCtx->stPhyConfig.enBaudRate = AT_UART_DEFAULT_BAUDRATE;
        }

        /* Modify by j00174725 for DTS2014012900848, 2014-02-12, Begin */
        /* ���NV�����õ�֡��ʽ�Ƿ���֧�ֵķ�Χ�� */
        ulFormatRet = AT_HSUART_IsFormatValid(stUartNVCfg.stFrame.ucFormat);

        /* ���NV�����õ�У���Ƿ���֧�ֵķ�Χ�� */
        ulParityRet = AT_HSUART_IsParityValid(stUartNVCfg.stFrame.ucParity);
        if ((VOS_TRUE == ulFormatRet) && (VOS_TRUE == ulParityRet))
        {
            pstUartCtx->stPhyConfig.stFrame.enFormat = stUartNVCfg.stFrame.ucFormat;
            pstUartCtx->stPhyConfig.stFrame.enParity = stUartNVCfg.stFrame.ucParity;
        }
        else
        {
            AT_HSUART_DBG_NV_FORMAT_ERR(1);
            pstUartCtx->stPhyConfig.stFrame.enFormat = AT_UART_DEFAULT_FORMAT;
            pstUartCtx->stPhyConfig.stFrame.enParity = AT_UART_DEFAULT_PARITY;
        }
        /* Modify by j00174725 for DTS2014012900848, 2014-02-12, End */

        pstUartCtx->stRiConfig.ulSmsRiOnInterval    = stUartNVCfg.stRiConfig.ulSmsRiOnInterval;
        pstUartCtx->stRiConfig.ulSmsRiOffInterval   = stUartNVCfg.stRiConfig.ulSmsRiOffInterval;
        pstUartCtx->stRiConfig.ulVoiceRiOnInterval  = stUartNVCfg.stRiConfig.ulVoiceRiOnInterval;
        pstUartCtx->stRiConfig.ulVoiceRiOffInterval = stUartNVCfg.stRiConfig.ulVoiceRiOffInterval;
        pstUartCtx->stRiConfig.ucVoiceRiCycleTimes  = stUartNVCfg.stRiConfig.ucVoiceRiCycleTimes;
    }
    else
    {
        AT_HSUART_DBG_NV_READ_ERR(1);
    }

    return;
}
/* Added by j00174725 for UART-MODEM Project, 2013-09-21, end */
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
