/******************************************************************************

                  ��Ȩ���� (C), 2004-2014, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : RnicInterface.h
  �� �� ��   : ����
  ��    ��   : ����
  ��������   : 2014��07��31��
  ����޸�   :
  ��������   : RNIC����ӿ�
  �����б�   :

  �޸���ʷ   :
  1.��    ��   : 2014��07��31��
    ��    ��   : ����
    �޸�����   : �����ļ�
******************************************************************************/
#ifndef __RNIC_INTERFACE_H__
#define __RNIC_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"

#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
/*================================================*/
/* ��ֵ�궨�� */
/*================================================*/

/*================================================*/
/* ���ܺ����궨�� */
/*================================================*/

/*******************************************************************************
  3 ö�ٶ���
*******************************************************************************/
/*********************************************
 ö����   : RNIC_RM_NET_ID_ENUM
 ö��˵�� : RNIC������ID
 1.��    ��   : 2012��11��24��
   ��    ��   : f00179208
   �޸�����   : �½�
*********************************************/
enum RNIC_RM_NET_ID_ENUM
{
    RNIC_RM_NET_ID_0,                                                           /* ����0 */
    RNIC_RM_NET_ID_1,                                                           /* ����1 */
    RNIC_RM_NET_ID_2,                                                           /* ����2 */
#if (FEATURE_ON == FEATURE_MULTI_MODEM)
    RNIC_RM_NET_ID_3,                                                           /* ����3 */
    RNIC_RM_NET_ID_4,                                                           /* ����4 */
#endif
    RNIC_RM_NET_ID_VT,                                                          /* VT�󶨵����� */
    RNIC_RM_NET_ID_BUTT
};
typedef VOS_UINT8 RNIC_RM_NET_ID_ENUM_UINT8;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/

/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/

/*****************************************************************************
  7 STRUCT����
*****************************************************************************/

/*****************************************************************************
  8 UNION����
*****************************************************************************/

/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

/*****************************************************************************
  H2ASN������Ϣ�ṹ����
*****************************************************************************/

/*****************************************************************************
  10 ��������
*****************************************************************************/



#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* RnicInterface.h */
