

#include <osl_types.h>
#include "mdrv_chg.h"

/*lint --e{715,818}*/

/*************************CHGģ��START********************************/

/*****************************************************************************
 �� �� ��  : mdrv_misc_get_cbc_state
 ��������  : ���ص��״̬�͵���
 �������  :pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
                          pucBcl  0:���������û����������
 �������  : pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
                          pucBcl  0:���������û����������
 ����ֵ��   0 �����ɹ�
                         -1����ʧ��

*****************************************************************************/
int mdrv_misc_get_cbc_state(unsigned char *pusBcs,unsigned char *pucBcl)
{
    return -1;
}


/*****************************************************************************
 �� �� ��  : mdrv_misc_get_battery_state
 ��������  :��ȡ�ײ���״̬��Ϣ
 �������  :battery_state ������Ϣ
 �������  :battery_state ������Ϣ
 ����ֵ��   0 �����ɹ�
                         -1����ʧ��

*****************************************************************************/
int mdrv_misc_get_battery_state(BATT_STATE_S *battery_state)
{
    return -1;
}

/*****************************************************************************
 �� �� ��  : mdrv_misc_get_charging_status
 ��������  :��ѯĿǰ�Ƿ����ڳ����
 �������  :��
 �������  :��
 ����ֵ��   0 δ���
                          1 �����
*****************************************************************************/
int mdrv_misc_get_charging_status(void)
{
    return -1;
}

/*****************************************************************************
 �� �� ��  : mdrv_misc_set_charge_state
 ��������  :ʹ�ܻ��߽�ֹ���
 �������  :ulState      0:��ֹ���
 						1:ʹ�ܳ��
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
void mdrv_misc_set_charge_state(unsigned long ulState)
{

}

/*****************************************************************************
 �� �� ��  : mdrv_misc_get_charge_state
 ��������  :��ѯ�Ƿ���Ҫ�ϵ�
 �������  :
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
int mdrv_misc_get_charge_state(void)
{
    return -1;
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_Sply
 ��������  :��ѯ
 �������  :
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
int mdrv_misc_sply_battery(void)
{
    return -1;
}

/*************************CHGģ�� END*********************************/

