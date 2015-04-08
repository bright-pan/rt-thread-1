/*********************************************************************
 * Filename:			fprint.h
 *
 * Description:
 *
 * Author:				Bright Pan
 * Email:				bright_pan@yuettak.com
 * Date:				2015-04-08
 *
 * Modify:
 *
 * Copyright (C) 2015 Yuettak Co.,Ltd
 ********************************************************************/

#ifndef _FPRINT_H_
#define _FPRINT_H_

#include <rthw.h>
#include <rtthread.h>
#include <stm32f10x.h>

#define RT_DEVICE_CTRL_SEND_CMD 0x21


//cmd define
#define FRAME_CMD_VRYPWD 0x13
#define FRAME_CMD_EMPTY 0x0d
#define FRAME_CMD_GET_ECHO 0x53
#define FRAME_CMD_SETSYSPARA 0x0e
#define FRAME_CMD_SETADDR 0x15
#define FRAME_CMD_STORE 0x06
#define FRAME_CMD_LOADCHAR 0x07
#define FRAME_CMD_UPCHAR 0x08
#define FRAME_CMD_DOWNCHAR 0x09
#define	FRAME_CMD_DELETECHAR 0x0c
#define FRAME_CMD_GET_STATUS 0x46
#define FRAME_CMD_GENIMAGE 0x01
#define FRAME_CMD_IMG2TZ 0x02
#define FRAME_CMD_REGMODEL 0x05
#define FRAME_CMD_SEARCH 0x04
#define FRAME_CMD_GETECHO 0x53
#define FRAME_CMD_AUTOLOGIN 0x54
#define FRAME_CMD_AUTOSEARCH 0x55

//result define
#define FRAME_ERR_OK 0x00 //指令执行完毕或 OK；
#define FRAME_ERR_DATAERROR 0x01 //数据包接收错误；
#define FRAME_ERR_NOFINGER 0x02 //传感器上没有手指；
#define FRAME_ERR_GENIMAGE 0x03 //录入指纹图像失败；
#define FRAME_ERR_BAD_IMG 0x06 //指纹图像太乱而生不成特征；
#define FRAME_ERR_TZ 0x07 //指纹图像正常，但特征点太少（或面积太小）而生不成特征；
#define FRAME_ERR_MATCH 0x08 //指纹不匹配；
#define FRAME_ERR_SEARCH 0x09 //没搜索到指纹；
#define FRAME_ERR_REGMODEL 0x0a //特征合并失败；
#define FRAME_ERR_ID 0x0b //访问指纹库时地址序号超出指纹库范围；
#define FRAME_ERR_INVALIDTZ 0x0c //从指纹库读模板出错或无效；
#define FRAME_ERR_UPCHAR 0x0d //上传特征失败；
#define FRAME_ERR_RECEIVE 0x0e //模块不能接受后续数据包；
#define FRAME_ERR_UPIMG 0x0f //上传图像失败；
#define FRAME_ERR_DELETECHAR 0x10 //删除模板失败；
#define FRAME_ERR_EMPTY 0x11 //清空指纹库失败；
#define FRAME_ERR_PWD 0x13 //口令不正确；
#define FRAME_ERR_NOT_GENIMG 0x15 //缓冲区内没有有效原始图而生不成图像；
#define FRAME_ERR_FLASH 0x18 //读写 FLASH 出错；
#define FRAME_ERR_IVALIDREG 0x1a //无效寄存器号；
#define FRAME_ERR_ADDR 0x20 //地址码错误
#define FRAME_ERR_VERIFYPWD 0x21 //必须验证口令；
#define FRAME_ERR_ERROR 0xff //系统保留。

// fprint frame head define
typedef struct{

    uint8_t start[2];
	uint8_t addr[4];
	uint8_t pid;
	uint8_t len[2];
}FRAME_HEAD_TYPEDEF;

// fprint frame data define
typedef struct {
    uint8_t pwd[4];
}FRAME_REQ_VRYPWD_TYPEDEF;

typedef struct {
    uint8_t type;
    uint8_t data;
}FRAME_REQ_SETSYSPARA_TYPEDEF;
typedef struct {
    uint8_t addr[4];
}FRAME_REQ_SETADDR_TYPEDEF;

typedef struct {
    uint8_t buf_id;
}FRAME_REQ_IMG2TZ_TYPEDEF;

typedef struct {

    uint8_t buf_id;

}FRAME_REQ_UPCHAR_TYPEDEF;
typedef struct {

    uint8_t buf_id;

}FRAME_REQ_DOWNCHAR_TYPEDEF;
typedef struct {

    uint8_t buf_id;
    uint8_t model_id[2];

}FRAME_REQ_LOADCHAR_TYPEDEF;

typedef struct {

    uint8_t buf_id;
    uint8_t model_id[2];

}FRAME_REQ_STORECHAR_TYPEDEF;

typedef struct {

    uint8_t model_id[2];
    uint8_t number[2];

}FRAME_REQ_DELETECHAR_TYPEDEF;

typedef struct {

    uint8_t buf_id;
    uint8_t model_id[2];
    uint8_t number[2];

}FRAME_REQ_SEARCH_TYPEDEF;

typedef struct {

    uint8_t push_timeout;
    uint8_t push_times;
    uint8_t model_id[2];
    uint8_t rep_flag;

}FRAME_REQ_AUTOLOGIN_TYPEDEF;

typedef struct {

    uint8_t push_timeout;
    uint8_t start_model_id[2];
    uint8_t model_number[2];

}FRAME_REQ_AUTOSEARCH_TYPEDEF;
/*
typedef struct {

}FRAME_REQ_GET_IMAGE_TYPEDEF;
typedef struct {

}FRAME_REQ_MERGE_TYPEDEF;

typedef struct {

}FRAME_REQ_GET_STATUS_TYPEDEF;
typedef struct {

}FRAME_REQ_GET_ECHO_TYPEDEF;
typedef struct {

}FRAME_REQ_EMPTY_TYPEDEF;
*/
typedef union {
    
    FRAME_REQ_VRYPWD_TYPEDEF vrypwd;
    FRAME_REQ_SETSYSPARA_TYPEDEF setsyspara;
    FRAME_REQ_SETADDR_TYPEDEF setaddr;
    FRAME_REQ_IMG2TZ_TYPEDEF img2tz;
    FRAME_REQ_UPCHAR_TYPEDEF upchar;
    FRAME_REQ_DOWNCHAR_TYPEDEF downchar;
    FRAME_REQ_LOADCHAR_TYPEDEF loadchar;
    FRAME_REQ_STORECHAR_TYPEDEF storechar;
    FRAME_REQ_DELETECHAR_TYPEDEF deletechar;
    FRAME_REQ_SEARCH_TYPEDEF search;
    FRAME_REQ_AUTOLOGIN_TYPEDEF autologin;
    FRAME_REQ_AUTOSEARCH_TYPEDEF autosearch;

}FRAME_REQ_DATA_TYPEDEF;

typedef struct {
    
    uint8_t cmd;
    FRAME_REQ_DATA_TYPEDEF data;
    uint8_t cs[2];
    
}FRAME_REQ_TYPEDEF;

// fprint reponse data define
typedef struct {

    uint8_t model_id[2];
    uint8_t score[2];

}FRAME_REP_SEARCH_TYPEDEF;
typedef struct {

    uint8_t model_id[2];
    uint8_t score[2];

}FRAME_REP_AUTOSEARCH_TYPEDEF;
/*
typedef struct {

}FRAME_REP_MERGE_TYPEDEF;
typedef struct {

}FRAME_REP_GET_IMAGE_TYPEDEF;

typedef struct {
    
}FRAME_REP_GENERATE_TYPEDEF;
typedef struct {
    
}FRAME_REP_STORE_CHAR_TYPEDEF;
typedef struct {
    
}FRAME_REP_LOAD_CHAR_TYPEDEF;
typedef struct {
    
}FRAME_REP_UP_CHAR_TYPEDEF;
typedef struct {
    
}FRAME_REP_DOWN_CHAR_TYPEDEF;
typedef struct {

}FRAME_REP_DEL_CHAR_TYPEDEF;
typedef struct {

}FRAME_REP_EMPTY_TYPEDEF;
typedef struct {

}FRAME_REP_SET_PARAM_TYPEDEF;
typedef struct {

}FRAME_REP_VERIFY_TYPEDEF;
typedef struct {

}FRAME_REP_SET_ADDR_TYPEDEF;
typedef struct {

}FRAME_REP_GET_STATUS_TYPEDEF;
typedef struct {

}FRAME_REP_GET_ECHO_TYPEDEF;
*/

typedef union {
    FRAME_REP_SEARCH_TYPEDEF search;
    FRAME_REP_AUTOSEARCH_TYPEDEF autosearch;
} FRAME_REP_DATA_TYPEDEF;

typedef struct {
    
    uint8_t result;
    FRAME_REP_DATA_TYPEDEF data;
    uint8_t cs[2];

}FRAME_REP_TYPEDEF;

struct fprint_args {

    FRAME_REQ_TYPEDEF *req;
    FRAME_REP_TYPEDEF *rep;
    void *buf;
    uint16_t *size;
};

#endif /* _FPRINT_H_ */
