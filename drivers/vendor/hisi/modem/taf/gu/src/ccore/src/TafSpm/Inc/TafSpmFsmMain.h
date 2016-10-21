/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : TafSpmFsmMain.h
  �� �� ��   : ����
  ��    ��   : w00176964
  ��������   : 2013��5��8��
  ����޸�   :
  ��������   : TafSpmFsmMain.C ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��5��8��
    ��    ��   : w00176964
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef _TAF_SPM_FSM_MAIN_H
#define _TAF_SPM_FSM_MAIN_H

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
/* Added by s00217060 for VoLTE_PhaseII  ��Ŀ, 2013-09-18, begin */
#if (FEATURE_IMS == FEATURE_ON)
#include "SpmImsaInterface.h"
#endif
/* Added by s00217060 for VoLTE_PhaseII  ��Ŀ, 2013-09-18, end */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

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
  10 ��������
*****************************************************************************/
/* Modified by y00245242 for VoLTE_PhaseI  ��Ŀ, 2013-7-30, begin */
VOS_UINT32 TAF_SPM_RcvAppReqMsg_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);
/* Modified by y00245242 for VoLTE_PhaseI  ��Ŀ, 2013-7-30, end */

VOS_UINT32 TAF_SPM_RcvSpmServiceCtrlResultInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);
/* Added by s00217060 for VoLTE_PhaseII  ��Ŀ, 2013-09-24, end */

VOS_UINT32 TAF_SPM_RcvPbFdnInd_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg
);

/* Deleted by s00217060 for VoLTE_PhaseIII  ��Ŀ, 2013-12-14, begin */
/* Deleted by s00217060 for VoLTE_PhaseIII  ��Ŀ, 2013-12-14, end */

/* Added by s00217060 for VoLTE_PhaseIII  ��Ŀ, 2013-12-30, begin */
VOS_VOID TAF_SPM_SendServiceRequetFail_Main(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    VOS_UINT32                          ulCause
);
/* Added by s00217060 for VoLTE_PhaseIII  ��Ŀ, 2013-12-30, end */




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

#endif /* end of TAF_SPM_FSM_MAIN.h */

