

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*
 * 1 Header File Including
 */

#define INI_KO_MODULE
//#define HISI_NVRAM_SUPPORT

#ifdef INI_KO_MODULE
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/time.h>
#include <linux/fs.h>
#include <linux/of.h>
#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif
#ifdef HISI_NVRAM_SUPPORT
#include <linux/mtd/hisi_nve_interface.h>
#endif
#include "hisi_ini.h"


#define CUST_COMP_NODE "hi1102,customize"
/* mutex for open ini file */
struct mutex        file_mutex;
int8 g_ini_file_name[INI_FILE_PATH_LEN];
INI_BOARD_VERSION_STRU g_board_version;
EXPORT_SYMBOL(g_board_version);

INI_PARAM_VERSION_STRU g_param_version;
EXPORT_SYMBOL(g_param_version);

#else

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    "hisi_ini.h"

pthread_mutex_t     file_mutex;
static int8 init_mutex_flag = 0;

int8 g_ini_file_path[INI_FILE_PATH_LEN] = {0};
#endif

#define INI_FILE_CFG_PATH       "/sys/hi110x_ps/ini_file_name"

#ifdef INI_KO_MODULE
#define INI_FILE_PATH           (g_ini_file_name)
#else
#define INI_FILE_PATH           (get_ini_file_path())
#endif

#define INI_VAR_LOG_CFG_STATUS  ("log_cfg_status")
#define INI_VAR_LOG_CFG_FILE    ("log_cfg_file")
#define INI_VAR_FILE_LEN        (40)
#define INI_VAR_CFG_ENABLE      (1)

/*
 * 2 Global Variable Definition
 */
int8  gac_cfg_file[INI_VAR_FILE_LEN] = {0};
int8  gac_file[INI_VAR_FILE_LEN] = {0};
/*
 * 3 Function Definition
 */
#ifndef INI_KO_MODULE
int8 * get_ini_file_path(void)
{
    FILE *          fp;
    int32           ret = 0;
    static int32    flag_got_path = 0;

    if(!flag_got_path)
    {
        fp = fopen(INI_FILE_CFG_PATH, "r");
        if (NULL == fp)
        {
            INI_ERROR("open %s failed:%s", INI_FILE_CFG_PATH, strerror(errno));
            return NULL;
        }
        ret = fread( g_ini_file_path, INI_FILE_PATH_LEN, 1, fp);
        if (0 > ret)
        {
            INI_WARNING("read %s failed:%s", INI_FILE_CFG_PATH, strerror(errno));
            fclose(fp);
            return NULL;
        }
        fclose(fp);
        flag_got_path = 1;
    }
    return g_ini_file_path;
}
#endif

#ifdef INI_KO_MODULE


static int32 ko_read_line(INI_FILE *fp, char *addr)
{
	int32 l_ret;
	int8 auc_tmp[MAX_READ_LINE_NUM];
	int32 cnt = 0;

	l_ret = kernel_read(fp, fp->f_pos, auc_tmp, MAX_READ_LINE_NUM);
	if (0 > l_ret)
	{
		INI_ERROR("kernel_line read l_ret < 0");
		return INI_FAILED;
	}
	else if (0 == l_ret)
	{
		/*end of file*/
		return 0;
	}

	cnt = 0;
	while (('\n' != auc_tmp[cnt]) && (cnt < MAX_READ_LINE_NUM - 1))
	{
		*addr++ = auc_tmp[cnt];
		cnt++;
	}

    if ((MAX_READ_LINE_NUM - 1) >= cnt)
    {
        *addr = '\n';
	}
	else
	{
		INI_ERROR("ko read_line is unexpected");
		return INI_FAILED;
	}

    /* change file pos to next line */
	fp->f_pos += (cnt + 1);

	return l_ret;
}
#endif


static INI_FILE * ini_file_open(int8 * filename, int8 * para)
{
    INI_FILE * fp;

#ifdef INI_KO_MODULE
    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR_OR_NULL(fp))
#else
    fp = fopen(filename, para);
    if (0 == fp)
#endif
    {
        fp = NULL;
    }
    return fp;
}


static int32 ini_file_close(INI_FILE * fp)
{
#ifdef INI_KO_MODULE
    filp_close(fp, NULL);
#else
    fclose(fp);
#endif
    return INI_SUCC;
}


static int32 ini_file_seek(INI_FILE *fp, long fp_pos)
{
#ifdef INI_KO_MODULE
    fp->f_pos += fp_pos;
    return INI_SUCC;
#else
    if (INI_FAILED == fseek(fp, fp_pos, SEEK_CUR))
    {
        INI_ERROR("fseek failed, fp_pos is %ld; errno is %s", fp_pos, strerror(errno));
        return INI_FAILED;
    }
    else
    {
        return INI_SUCC;
    }
#endif
}


static int32 ini_readline_func(INI_FILE *fp, int8 * rd_buf)
{
	int8 auc_tmp[MAX_READ_LINE_NUM];
    int32 l_ret;

    memset(auc_tmp, 0, MAX_READ_LINE_NUM);
#ifdef INI_KO_MODULE
    l_ret = ko_read_line(fp, auc_tmp);

    if (INI_FAILED == l_ret)
    {
        INI_ERROR("ko_read_line failed!!!");
        return INI_FAILED;
    }
    else if (0 == l_ret)
    {
        INI_ERROR("end of .ini file!!!");
        return INI_FAILED;
    }
#else
    int8* pau_ret;

    if (feof(fp))
    {
        INI_ERROR("have end of .ini file!!!");
        return INI_FAILED;
    }
    pau_ret = NULL;
    pau_ret = fgets(auc_tmp, MAX_READ_LINE_NUM, fp);
    if (NULL == pau_ret)
    {
        INI_ERROR("have end of .ini file!!!");
        return INI_FAILED;
    }
#endif
    strncpy(rd_buf, auc_tmp, MAX_READ_LINE_NUM);
    return INI_SUCC;
}


int32 ini_check_str(INI_FILE *fp, int8 * auc_tmp, int8 * puc_var)
{
    uint16 auc_len;
    uint16 curr_var_len;
    uint16 search_var_len;

    if ((NULL == fp)||(NULL == puc_var)||('\0' == puc_var[0]))
    {
        INI_ERROR("check if puc_var is NULL or blank");
        return INI_FAILED;
    }

    do{
        auc_len = strlen(auc_tmp);
        curr_var_len = 0;

        while ((curr_var_len < MAX_READ_LINE_NUM) && (auc_tmp[curr_var_len] != '\r') && (auc_tmp[curr_var_len] != '\n') &&
                (auc_tmp[curr_var_len] != 0))
        {
            curr_var_len++;
        }

        if (('#' == auc_tmp[0]) || (' ' == auc_tmp[0]) || ('\n' == auc_tmp[0]) || ('\r' == auc_tmp[0]))
        {
            break;
        }
        search_var_len = strlen(puc_var);
        if (search_var_len > curr_var_len)
        {
            search_var_len = curr_var_len;
        }
        if (0 == strncmp(auc_tmp, puc_var, search_var_len))
        {
            return INI_SUCC;
        }
        else
        {
            break;
        }
    }while(0);

    if (INI_FAILED == ini_file_seek(fp, -auc_len))
    {
        INI_ERROR("file seek failed!!!");
        return INI_FAILED;
    }
    if (INI_FAILED == ini_file_seek(fp, curr_var_len+1))
    {
        INI_ERROR("file seek failed!!!");
        return INI_FAILED;
    }
    if (auc_tmp[curr_var_len + 1] == '\n')
    {
        if (INI_FAILED == ini_file_seek(fp, 1))
        {
            INI_ERROR("file seek failed!!!");
            return INI_FAILED;
        }
    }
    return INI_FAILED;
}


static int32 ini_check_value(int8 *puc_value)
{
	int32 l_len;

	l_len = strlen(puc_value);
	if (' ' == puc_value[0] || ' ' == puc_value[l_len - 1]
							|| '\n' == puc_value[0])
	{
		puc_value[0] = '\0';
		INI_ERROR("::%s has blank space or is blank::", puc_value);
		return INI_FAILED;
	}

    /* check \n of line */
	if ('\n' == puc_value[l_len - 1])
    {
        puc_value[l_len - 1] = '\0';
    }

    /* check \r of line */
    if ('\r' == puc_value[l_len - 2])
    {
        puc_value[l_len - 2] = '\0';
    }
	return INI_SUCC;
}


static int32 ini_find_mode(INI_FILE *fp, int32 modu, int8 *puc_var, int8 *puc_value)
{
    int32 ret;
	int8 auc_tmp[MAX_READ_LINE_NUM];
	int8 auc_mode_var[INI_STR_MODU_LEN];
	int8 *puc_val = NULL;

	switch (modu)
	{
		case INI_MODU_WIFI:
			strncpy(auc_mode_var, INI_MODE_VAR_WIFI, INI_STR_MODU_LEN);
			break;
		case INI_MODU_GNSS:
			strncpy(auc_mode_var, INI_MODE_VAR_GNSS, INI_STR_MODU_LEN);
			break;
		case INI_MODU_BT:
			strncpy(auc_mode_var, INI_MODE_VAR_BT, INI_STR_MODU_LEN);
			break;
		case INI_MODU_FM:
			strncpy(auc_mode_var, INI_MODE_VAR_FM, INI_STR_MODU_LEN);
			break;
		case INI_MODU_WIFI_PLAT:
			strncpy(auc_mode_var, INI_MODE_VAR_WIFI_PLAT, INI_STR_MODU_LEN);
			break;
		case INI_MODU_BFG_PLAT:
			strncpy(auc_mode_var, INI_MODE_VAR_BFG_PLAT, INI_STR_MODU_LEN);
			break;
		case INI_MODU_PLAT:
		case INI_MODU_HOST_VERSION:
		case INI_MODU_WIFI_MAC:
		case INI_MODU_COEXIST:
			return INI_SUCC;
			break;
		default :
			INI_ERROR("not suport modu type!!!");
			break;
	}

	while(1)
	{
        ret = ini_readline_func(fp, auc_tmp);
        if (INI_FAILED == ret)
        {
            INI_ERROR("have end of .ini file!!!");
            return INI_FAILED;
        }

		if (NULL != strstr(auc_tmp, INI_STR_DEVICE_BFG_PLAT))
		{
			INI_ERROR("not find %s!!!", auc_mode_var);
			return INI_FAILED;
		}

        ret = ini_check_str(fp, auc_tmp, auc_mode_var);
        if (INI_SUCC == ret)
        {
            INI_DEBUG("have found %s", auc_mode_var);
            break;
        }
        else
        {
            continue;
        }
	}

	puc_val = strstr(auc_tmp, "=");
	if (NULL == puc_val)
	{
		INI_ERROR("has not find = in %s", auc_tmp);
		return INI_FAILED;
	}

	strcpy(puc_value, puc_val + 1);
	if (0 == strcmp(auc_mode_var, puc_var))
	{
		return INI_SUCC_MODE_VAR;			
	}
	return INI_SUCC;
}


static int32 ini_find_modu(INI_FILE *fp, int32 modu, int8 * puc_var, int8 *puc_value)
{
	int8 auc_tmp[MAX_READ_LINE_NUM];
	int8 auc_modu[INI_STR_MODU_LEN];
	int32 l_mode_value;
	int32 ret;

	/* check the mode of moduler */
	if (INI_FAILED == ini_check_value(puc_value))
	{
		return INI_FAILED;
	}

	/* INI_MODU_PLAT has no mode*/
	if (INI_MODU_PLAT != modu)
	{
		l_mode_value = puc_value[0] - '0';
		if (10 < l_mode_value || 0 > l_mode_value)
		{
			INI_ERROR("not support value %s", puc_value);
			return INI_FAILED;
		}
	}

	switch (modu)
	{
		case INI_MODU_WIFI:
			/*find mode var*/
			if (INI_MODE_NORMAL	== l_mode_value)
			{
				strncpy(auc_modu, INI_STR_WIFI_NORMAL, INI_STR_MODU_LEN);
			}
			else if (INI_MODE_PERFORMANCE == l_mode_value)
			{
				strncpy(auc_modu, INI_STR_WIFI_PERFORMANCE, INI_STR_MODU_LEN);
			}
			else if (INI_MODE_CERTIFY == l_mode_value)
			{
				strncpy(auc_modu, INI_STR_WIFI_CERTIFY, INI_STR_MODU_LEN);
			}
            else if (INI_MODE_CERTIFY_CE == l_mode_value)
			{
				strncpy(auc_modu, INI_STR_WIFI_CERTIFY_CE, INI_STR_MODU_LEN);
			}

			else
			{
				INI_ERROR("not support mode!!!");
				return INI_FAILED;
			}
			break;
		case INI_MODU_GNSS:
			if (INI_MODE_GPSGLONASS	== l_mode_value)
			{
				strncpy(auc_modu, INI_STR_GNSS_GPSGLONASS, INI_STR_MODU_LEN);
			}
			else if (INI_MODE_BDGPS == l_mode_value)
			{
				strncpy(auc_modu, INI_STR_GNSS_BDGPS, INI_STR_MODU_LEN);
			}
			else
			{
				INI_ERROR("not support mode!!!");
				return INI_FAILED;
			}
			break;
		case INI_MODU_BT:
			strncpy(auc_modu, INI_STR_BT_NORMAL, INI_STR_MODU_LEN);
			break;
		case INI_MODU_FM:
			strncpy(auc_modu, INI_STR_FM_NORMAL, INI_STR_MODU_LEN);
			break;
		case INI_MODU_WIFI_PLAT:
			strncpy(auc_modu, INI_STR_WIFI_PLAT_NORMAL, INI_STR_MODU_LEN);
			break;
		case INI_MODU_BFG_PLAT:
			strncpy(auc_modu, INI_STR_BFG_PLAT_NORMAL, INI_STR_MODU_LEN);
			break;
		case INI_MODU_PLAT:
			strncpy(auc_modu, INI_STR_PLAT, INI_STR_MODU_LEN);
			break;
		case INI_MODU_HOST_VERSION:
		    strncpy(auc_modu, INT_STR_HOST_VERSION, INI_STR_MODU_LEN);
			break;
		case INI_MODU_WIFI_MAC:
		    strncpy(auc_modu, INI_STR_WIFI_MAC, INI_STR_MODU_LEN);
			break;
		case INI_MODU_COEXIST:
		    strncpy(auc_modu, INI_STR_COEXIST, INI_STR_MODU_LEN);
			break;
		default :
			INI_ERROR("not suport modu type!!!");
			break;
	}

	/* find the value of mode var, such as ini_wifi_mode 
	 * every mode except PLAT mode has only one mode var */
	while(1)
	{
        ret = ini_readline_func(fp, auc_tmp);
        if (INI_FAILED == ret)
        {
            INI_ERROR("have end of .ini file!!!");
            return INI_FAILED;
        }

		if (NULL != strstr(auc_tmp, INI_STR_DEVICE_BFG_PLAT))
		{
			INI_ERROR("not find %s!!!", auc_modu);
			return INI_FAILED;
		}

        ret = ini_check_str(fp, auc_tmp, auc_modu);
        if (INI_SUCC == ret)
        {
			INI_DEBUG("have found %s", auc_modu);
			break;
        }
        else
        {
			continue;
        }
	}

	return INI_SUCC;
}


static int32 ini_find_var(INI_FILE *fp, int32 modu, int8 * puc_var, int8 *puc_value)
{
	int32 ret;
	int8 auc_tmp[MAX_READ_LINE_NUM];
    size_t search_var_len;

	/* find the modu of var, such as [HOST_WIFI_NORMAL] of wifi moduler*/
    ret = ini_find_modu(fp, modu, puc_var, puc_value);
	if (INI_FAILED == ret)
	{
		return INI_FAILED;
	}

	/* find the var in modu, such as [HOST_WIFI_NORMAL] of wifi moduler*/
	while(1)
	{
        ret = ini_readline_func(fp, auc_tmp);
        if (INI_FAILED == ret)
        {
            INI_ERROR("have end of .ini file!!!");
            return INI_FAILED;
        }

		if ('[' == auc_tmp[0])
		{
			INI_ERROR("not find %s!!!, check if var in correct mode", puc_var);
			return INI_FAILED;
		}

        search_var_len = strlen(puc_var);
        ret = ini_check_str(fp, auc_tmp, puc_var);
        if (INI_SUCC == ret)
        {
            //INI_DEBUG("have found %s", puc_var);
            strcpy(puc_value, &auc_tmp[search_var_len+1]);
            break;
        }
        else
        {
            continue;
        }
	}
	return INI_SUCC;
}

#ifndef HISI_NVRAM_SUPPORT

int32 read_conf_from_file(int8 * name, int8* pc_arr)
{
    INI_FILE * fp;
    int32 len;

#ifdef INI_KO_MODULE
    fp = filp_open(NVRAM_CUST_FILE, O_RDONLY, 0660);
    if (IS_ERR(fp))
#else
        fp = fopen(NVRAM_CUST_FILE, "r");
    if (NULL == fp)
#endif
    {
#ifdef INI_KO_MODULE
        INI_DEBUG("open %s failed", NVRAM_CUST_FILE);
#else
        INI_DEBUG("open %s failed, errno is %s", NVRAM_CUST_FILE, strerror(errno));
#endif
        return INI_FAILED;
    }

    INI_DEBUG("open %s succ", NVRAM_CUST_FILE);

#ifdef INI_KO_MODULE
    len = kernel_read(fp, 0, pc_arr, NUM_OF_NV_PARAMS);
#else
    len = fread(pc_arr, 1, HISI_CUST_NVRAM_LEN,  fp);
#endif
    if (len < 0)
    {
        ini_file_close(fp);
#ifdef INI_KO_MODULE
        INI_ERROR("read %s failed, len = %d", NVRAM_CUST_FILE, len);
#else
        INI_ERROR("read %s failed, len = %d, errno is %s", NVRAM_CUST_FILE, len, strerror(errno));
#endif
        return INI_FAILED;
    }
    INI_DEBUG("read %s succ", NVRAM_CUST_FILE);
    ini_file_close(fp);

    return INI_SUCC;
}

int32 write_conf_to_file(int8* name, int8 * pc_arr)
{
    INI_FILE * fp;
    int32 len;

#ifdef INI_KO_MODULE
    fp = filp_open(NVRAM_CUST_FILE, O_CREAT|O_RDWR, 0664);
	if (IS_ERR(fp))
    {
        INI_DEBUG("open %s failed", NVRAM_CUST_FILE);
        return INI_FAILED;
    }
#else
	fp = fopen(NVRAM_CUST_FILE, "w+");
	if (NULL == fp)
    {
        INI_DEBUG("open %s failed!:%s ", NVRAM_CUST_FILE, strerror(errno));
        return INI_FAILED;
    }
#endif

    INI_DEBUG("open %s succ", NVRAM_CUST_FILE);

#ifdef INI_KO_MODULE
    len = kernel_write(fp, pc_arr, HISI_CUST_NVRAM_LEN, 0);
#else
	len = fwrite(pc_arr, 1, HISI_CUST_NVRAM_LEN, fp);
#endif
    INI_DEBUG("write len:%d", len);

    ini_file_close(fp);

    return INI_SUCC;
}
#else

int32 read_conf_from_nvram(int8 * name, int8 * pc_out)
{
#ifdef	INI_KO_MODULE
    struct hisi_nve_info_user  info;
#endif
    int8 buff[HISI_CUST_NVRAM_LEN] = {0};
    int32 ret = -1;

#ifndef	INI_KO_MODULE
    ret = hweiOperaNVWR(HISI_CUST_NVRAM_READ, HISI_CUST_NVRAM_NUM, HISI_CUST_NVRAM_NAME, HISI_CUST_NVRAM_LEN, buff);
    if (ret < -1) {
        INI_ERROR("read nvm failed");
        return INI_FAILED;
    }
#else
    memset(&info, 0, sizeof(info));
    strncpy(info.nv_name, HISI_CUST_NVRAM_NAME, strlen(HISI_CUST_NVRAM_NAME) - 1);
    info.nv_name[strlen(HISI_CUST_NVRAM_NAME) - 1] = '\0';
    info.nv_number = HISI_CUST_NVRAM_NUM;
    info.valid_size = HISI_CUST_NVRAM_LEN;
    info.nv_operation = HISI_CUST_NVRAM_READ;

    ret = hisi_nve_direct_access( &info );
    if (ret < -1) {
        INI_ERROR("read nvm failed");
        return INI_FAILED;
    }
    else
    {
        memcpy(pc_out, info.nv_data, NUM_OF_NV_PARAMS);
    }
#endif
    return INI_SUCC;
}

int32 write_conf_to_nvram(int8 * name, int8 * pc_arr)
{
#ifdef	INI_KO_MODULE
    struct hisi_nve_info_user  info;
#endif
    int8 buff[HISI_CUST_NVRAM_LEN] = {0};
    int8 save_buff[HISI_CUST_NVRAM_LEN] = {0};
    int32 ret = -1;

#ifndef	INI_KO_MODULE
    ret = hweiOperaNVWR(HISI_CUST_NVRAM_WRITE, HISI_CUST_NVRAM_NUM, HISI_CUST_NVRAM_NAME, HISI_CUST_NVRAM_LEN, pc_arr);
    if (ret < -1) {
        INI_ERROR("write nvm failed");
        return INI_FAILED;
    }
#else
    memset(&info, 0, sizeof(info));
    strncpy(info.nv_name, HISI_CUST_NVRAM_NAME, strlen(HISI_CUST_NVRAM_NAME) - 1);
    info.nv_name[strlen(HISI_CUST_NVRAM_NAME) - 1] = '\0';
    info.nv_number = HISI_CUST_NVRAM_NUM;
    info.valid_size = HISI_CUST_NVRAM_LEN;
    info.nv_operation = HISI_CUST_NVRAM_WRITE;
    memcpy(info.data, pc_arr, HISI_CUST_NVRAM_LEN);

    ret = hisi_nve_direct_access( &info );
    if (ret < -1) {
        INI_ERROR("write nvm failed");
        return INI_FAILED;
    }
#endif

    return INI_SUCC;
}
#endif


#ifndef INI_KO_MODULE
 int32 ini_config_init(int32 modu)
 {
     int8  log_status[10] = {0};
     int32 log_value;
	 int32 l_ret = INI_FAILED;
     INI_FILE *fp = NULL;
	/* init only once */

	if (INI_MODU_PLAT != modu)
    {
        if (NULL == INI_FILE_PATH)
        {
            INI_ERROR("didn't get ini file path!!!");
            return INI_FAILED;
        }
		strncpy(gac_file, INI_FILE_PATH, INI_VAR_FILE_LEN);
		gac_file[INI_VAR_FILE_LEN] = '\0';
		return INI_SUCC;
	}

	INI_DEBUG("open ini file:%s", gac_file);
	fp = ini_file_open(gac_file, "rt");
	if (0 == fp)
	{
		INI_ERROR("open %s failed!!!", gac_file);
        return INI_FAILED;
	}
    INI_DEBUG("open %s succ!", gac_file);

	l_ret = ini_find_var(fp, INI_MODU_PLAT, INI_VAR_LOG_CFG_STATUS, log_status);
	if (INI_FAILED == l_ret)
	{
		ini_file_close(fp);
		return INI_FAILED;
	}

	if (INI_FAILED == ini_check_value(log_status))
	{
		ini_file_close(fp);
		return INI_FAILED;
	}
	sscanf(log_status, "%d", &log_value);
	if (0 > log_value)
	{
		INI_ERROR("log_status value=%d not support!!!", log_value);
		ini_file_close(fp);
		return INI_FAILED;
	}
	INI_INFO("::log_status value=%d::", log_value);
	ini_file_close(fp);
#if 0
	if (INI_VAR_CFG_ENABLE == log_value)
	{
		fp = ini_file_open(INI_FILE_PATH, "rt");
		if (0 == fp)
		{
			INI_ERROR("open %s failed!!!", INI_FILE_PATH);
			return INI_FAILED;
		}

		l_ret = ini_find_var(fp, INI_MODU_PLAT, INI_VAR_LOG_CFG_FILE, gac_cfg_file);
		if (INI_FAILED == l_ret)
		{
			ini_file_close(fp);
			return INI_FAILED;
		}
		if (INI_FAILED == ini_check_value(gac_cfg_file))
		{
			ini_file_close(fp);
			return INI_FAILED;
		}
		//INI_DEBUG("::gac_cfg_file = %s::", gac_cfg_file);
		strncpy(gac_file, gac_cfg_file, INI_VAR_FILE_LEN);
		gac_file[INI_VAR_FILE_LEN] = '\0';
		if (INI_FAILED == access(gac_file, 0))
		{
			INI_ERROR("%s is not exist, init log control system", gac_file);
			l_ret = system("/system/bin/set_log_hisi -S init");
			if (INI_SUCC!= l_ret)
			{
				INI_ERROR("system() run err");
				ini_file_close(fp);
				return INI_FAILED;
			}
		}
	}
	ini_file_close(fp);
#endif

	return INI_SUCC;
 }
#endif

int32 get_cust_conf_string(int32 modu, int8 * puc_var, int8* puc_value)
{
    int32 ret = 0;

#ifdef INI_KO_MODULE
    struct device_node *np;
    int32  len;
    int8   out_str[HISI_CUST_NVRAM_LEN] = {0};
#endif

    switch (modu)
    {
#ifdef INI_KO_MODULE
        case CUST_MODU_DTS:
        {
            np = of_find_compatible_node(NULL, NULL, CUST_COMP_NODE);
            if (NULL == np)
            {
                INI_ERROR("no compatible node found");
                return INI_FAILED;
            }

            //ret = of_property_read_string(np, puc_var, (const char **)&out_str);
            len = of_property_count_u8_elems(np, puc_var);
            if (len < 0)
            {
                INI_ERROR("can't get len of value(%s)", puc_var);
                return INI_FAILED;
            }
            len = INI_MIN(len, NUM_OF_NV_PARAMS);
            INI_DEBUG("get len of value(%s), read len is %d", puc_var, len);
            ret = of_property_read_u8_array(np, puc_var, out_str, len);
            if (0 > ret)
            {
                INI_ERROR("conf str:%s property is not found", puc_var);
                return INI_FAILED;
            }
            //strncpy(puc_value, out_str, INI_FILE_PATH_LEN);
            memcpy(puc_value, out_str, len);
            INI_DEBUG("conf string:%s get value:%s", puc_var, puc_value);
            return INI_SUCC;
        }
#endif
        case CUST_MODU_NVRAM:
            #ifdef HISI_NVRAM_SUPPORT
            ret = read_conf_from_nvram(puc_var, puc_value);
            #else
            ret = read_conf_from_file(puc_var, puc_value);
            #endif
            if (ret < 0)
            {
                INI_ERROR("read nv_conf failed, ret(%d)", ret);
                return INI_FAILED;
            }
            return INI_SUCC;

        default:
        {
            return ini_find_var_value(modu, puc_var, puc_value);
        }
    }
    return INI_SUCC;
}
#ifdef INI_KO_MODULE
EXPORT_SYMBOL(get_cust_conf_string);
#endif

int32 set_cust_conf_string(int32 modu, int8 * name, int8 * var)
{
    int32 ret = -1;
    if (modu != CUST_MODU_NVRAM)
    {
        INI_ERROR("NOT SUPPORT MODU TO WRITE");
        return INI_FAILED;
    }

    #ifdef HISI_NVRAM_SUPPORT
    ret = write_conf_to_nvram(name, var);
    #else
    ret = write_conf_to_file(name, var);
    #endif

    return ret;
}

int32 get_cust_conf_int32(int32 modu, int8 * puc_var, int32* puc_value)
{
    int32  ret = 0;
#ifdef INI_KO_MODULE
    struct device_node * np;
#endif

	switch (modu)
	{
#ifdef INI_KO_MODULE
        case CUST_MODU_DTS:
        {
            np = of_find_compatible_node(NULL, NULL, CUST_COMP_NODE);
            if (NULL == np)
            {
                INI_ERROR("no compatible node found");
                return INI_FAILED;
            }

            ret = of_property_read_u32(np, puc_var, puc_value);
            if (ret < 0)
            {
                /* driver has log, delete this log */
                INI_DEBUG("conf %s is not exist, ret = %d", puc_var, ret);
                return INI_FAILED;
            }

            INI_DEBUG("conf %s get vale:%d", puc_var, *puc_value);
            return INI_SUCC;
        }
#endif
        default:
        {
            int8  out_str[INI_READ_VALUE_LEN] = {0};

            ret = ini_find_var_value(modu, puc_var, out_str);
            if (ret < 0)
            {
                /* ini_find_var_value has error log, delete this log */
                INI_DEBUG("cust modu didn't get var of %s.", puc_var);
                return INI_FAILED;
            }
            if ( !strncmp(out_str, "0x", strlen("0x")) || !strncmp(out_str, "0X", strlen("0X")))
            {
                INI_DEBUG("get hex of:%s.", puc_var);
                ret = sscanf(out_str, "%x", puc_value);
            }
            else
            {
                ret = sscanf(out_str, "%d", puc_value);
            }

	        if(ret < 0)
            {
	            INI_ERROR("%s trans to int failed", puc_var);
                return INI_FAILED;
            }
            INI_DEBUG("conf %s get vale:%d", puc_var, *puc_value);

            return INI_SUCC;
        }
    }
    return INI_SUCC;
}
#ifdef INI_KO_MODULE
EXPORT_SYMBOL(get_cust_conf_int32);
#endif

#ifdef INI_KO_MODULE

void print_device_version(INI_FILE *fp)
{
    int8  version_buff[INI_VERSION_STR_LEN] = {0};
    int32 l_ret;

    l_ret = ini_find_var(fp, INI_MODU_PLAT, INI_VAR_PLAT_BOARD, version_buff);
    if (INI_FAILED == l_ret)
    {
        version_buff[0] = '\0';
        fp->f_pos = 0;
        return ;
    }
    if (INI_FAILED == ini_check_value(version_buff))
    {
        fp->f_pos = 0;
        return ;
    }
    strncpy(g_board_version.board_version, version_buff, INI_VERSION_STR_LEN);
    INI_INFO("::g_board_version.board_version = %s::", g_board_version.board_version);
    fp->f_pos = 0;

    l_ret = ini_find_var(fp, INI_MODU_PLAT, INI_VAR_PLAT_PARAM, version_buff);
    if (INI_FAILED == l_ret)
    {
        version_buff[0] = '\0';
        fp->f_pos = 0;
        return ;
    }
    if (INI_FAILED == ini_check_value(version_buff))
    {
        fp->f_pos = 0;
        return ;
    }
    strncpy(g_param_version.param_version, version_buff, INI_VERSION_STR_LEN);
    INI_INFO("::g_param_version.param_version = %s::", g_param_version.param_version);
    fp->f_pos = 0;
    return ;
}
#endif

int32 ini_find_var_value(int32 modu, int8 * puc_var, int8* puc_value)
{
	INI_FILE *fp = NULL;

#ifdef INI_KO_MODULE
	static int32 sl_once = 1;
#endif
#ifdef INI_TIME_TEST
	struct timeval tv[2];
#endif

	int32 l_ret;

	if (NULL == puc_var || '\0' == puc_var[0] || NULL == puc_value)
	{
		INI_ERROR("check if puc_var and puc_value is NULL or blank");
		return INI_FAILED;
	}

#ifdef INI_TIME_TEST
	do_gettimeofday(&tv[0]);
#endif

#ifndef INI_KO_MODULE
    if (0 == init_mutex_flag)
    {
        INI_INIT_MUTEX(&file_mutex, NULL);
        init_mutex_flag = 1;
    }
#endif

    INI_MUTEX_LOCK(&file_mutex);

#ifndef INI_KO_MODULE
    l_ret = ini_config_init(modu);
    if (INI_FAILED == l_ret)
    {
        INI_MUTEX_UNLOCK(&file_mutex);
        return INI_FAILED;
    }
#endif

	fp = ini_file_open(INI_FILE_PATH, "rt");
	if (0 == fp)
	{
		fp = NULL;
		INI_ERROR("open %s failed!!!", INI_FILE_PATH);
        INI_MUTEX_UNLOCK(&file_mutex);
		return INI_FAILED;
	}
#ifdef INI_KO_MODULE
	/* init g_board_version.board_version and g_param_version.param_version
	 * just once while call ini_find_var_value */
	if (sl_once)
	{
        print_device_version(fp);
        sl_once = 0;
	}
#endif

	l_ret = ini_find_mode(fp, modu, puc_var, puc_value);
	if (INI_FAILED == l_ret)
	{
        ini_file_close(fp);
        INI_MUTEX_UNLOCK(&file_mutex);
		return INI_FAILED;
	}
	else if (INI_SUCC_MODE_VAR == l_ret)
	{
		INI_DEBUG("::return %s:%s::", puc_var, puc_value);
        ini_file_close(fp);
        INI_MUTEX_UNLOCK(&file_mutex);
		return  ini_check_value(puc_value);
	}
	
	/* find puc_var in .ini return puc_value */
	l_ret = ini_find_var(fp, modu, puc_var, puc_value);
	if (INI_FAILED == l_ret)
	{
		puc_value[0] = '\0';
        ini_file_close(fp);
        INI_MUTEX_UNLOCK(&file_mutex);
		return INI_FAILED;
	}

#ifdef INI_TIME_TEST
	do_gettimeofday(&tv[1]);
	INI_DEBUG("time take = %ld", (tv[1].tv_sec - tv[0].tv_sec) * 1000 + (tv[1].tv_usec - tv[0].tv_usec) / 1000);
#endif

    ini_file_close(fp);
    INI_MUTEX_UNLOCK(&file_mutex);

	/* check blank space of puc_value */
	if (INI_SUCC == ini_check_value(puc_value))
	{
		INI_DEBUG("::return %s:%s::", puc_var, puc_value);
		return INI_SUCC;
	}
	return INI_FAILED;
}

#ifdef INI_KO_MODULE
EXPORT_SYMBOL(ini_find_var_value);
#endif

#ifdef INI_KO_MODULE
int ini_cfg_init(void)
{

//	int8 auc_value[20];
    int32 ret;

#ifdef CONFIG_HWCONNECTIVITY
    if (!isMyConnectivityChip(CHIP_TYPE_HI110X)) {
        INI_ERROR("cfg ini chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        INI_INFO("cfg init type is matched with hi110x, continue");
    }
#endif

	INI_DEBUG("k3v2 .ini config search init!\n");

    ret = get_cust_conf_string(CUST_MODU_DTS, "ini_file_name", g_ini_file_name);
    if ( 0 > ret )
    {
        INI_DEBUG("can't find ini_file_name node");
    }

    INI_DEBUG("ini_file_name@%s ", g_ini_file_name);
    /*init mutex*/
    INI_INIT_MUTEX(&file_mutex);

#ifdef INI_TEST
#if 1
	ini_find_var_value(INI_MODU_WIFI, "wifi_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_GNSS, "gnss_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_BT, "bt_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_FM, "fm_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_WIFI_PLAT, "wifi_plat_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_BFG_PLAT, "bfg_plat_ini_mode", auc_value);

	ini_find_var_value(INI_MODU_WIFI, "loglevel", auc_value);
	ini_find_var_value(INI_MODU_WIFI, "data_rate_down", auc_value);
	ini_find_var_value(INI_MODU_WIFI, "data_rate_up", auc_value);

	ini_find_var_value(INI_MODU_GNSS, "gnss_gpsglonass", auc_value);
	ini_find_var_value(INI_MODU_GNSS, "gnss_bdgps", auc_value);

	ini_find_var_value(INI_MODU_BT, "bt_normal", auc_value);

	ini_find_var_value(INI_MODU_FM, "fm_normal", auc_value);

	ini_find_var_value(INI_MODU_WIFI_PLAT, "wifi_plat_normal", auc_value);
	
	ini_find_var_value(INI_MODU_BFG_PLAT, "bfg_plat_normal", auc_value);

#endif 
#endif 

	return INI_SUCC;
}
EXPORT_SYMBOL(ini_cfg_init);

void ini_cfg_exit(void)
{

	INI_DEBUG("k3v2 .ini config search exit!\n");
}
EXPORT_SYMBOL(ini_cfg_exit);

//module_init(ini_cfg_init);
//module_exit(ini_cfg_exit);
//MODULE_AUTHOR("Hisilicon .ini config search Driver Group");
//MODULE_DESCRIPTION("K3v2 .ini config search platform driver");
//MODULE_LICENSE("GPL");
#else
#ifdef INI_TEST
int32 main(int32 argc, int8** argv)
{
    int8 auc_value[50];

	INI_DEBUG("k3v2 .ini config search init!\n");

#if 1
	ini_write_var_value(INI_MODU_WIFI, "wifi_ini_mode", "2");
	ini_write_var_value(INI_MODU_WIFI, "wifi_ini_mode", "1");
	ini_write_var_value(INI_MODU_WIFI, "wifi_ini_mode", "0");
	ini_write_var_value(INI_MODU_GNSS, "gnss_ini_mode", "1");
#endif

#if 1
	ini_write_var_value(INI_MODU_BT, "bt_ini_mode", "0");
	ini_write_var_value(INI_MODU_FM, "fm_ini_mode", "0");
	ini_write_var_value(INI_MODU_WIFI_PLAT, "wifi_plat_ini_mode", "0");
	ini_write_var_value(INI_MODU_BFG_PLAT, "bfg_plat_ini_mode", "0");
	ini_find_var_value(INI_MODU_WIFI, "wifi_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_GNSS, "gnss_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_BT, "bt_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_FM, "fm_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_WIFI_PLAT, "wifi_plat_ini_mode", auc_value);
	ini_find_var_value(INI_MODU_BFG_PLAT, "bfg_plat_ini_mode", auc_value);

	ini_find_var_value(INI_MODU_WIFI, "loglevel", auc_value);
	ini_find_var_value(INI_MODU_WIFI, "data_rate_down", auc_value);
	ini_find_var_value(INI_MODU_WIFI, "data_rate_up", auc_value);
	ini_find_var_value(INI_MODU_WIFI, "hcc_rx_lo_queue", auc_value);

	ini_find_var_value(INI_MODU_GNSS, "gnss_gpsglonass", auc_value);
	ini_find_var_value(INI_MODU_GNSS, "gnss_bdgps", auc_value);

	ini_find_var_value(INI_MODU_BT, "bt_normal", auc_value);

	ini_find_var_value(INI_MODU_FM, "fm_normal", auc_value);

	ini_find_var_value(INI_MODU_WIFI_PLAT, "wifi_plat_normal", auc_value);

	ini_find_var_value(INI_MODU_BFG_PLAT, "bfg_plat_normal", auc_value);
#endif

	ini_find_var_value(INI_MODU_PLAT, "g_param_version.param_version", auc_value);

    INI_DEBUG("k3v2 .ini config over!\n");
	return 0;
}
#endif 
#endif
