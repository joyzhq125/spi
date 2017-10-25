#ifndef DS_ANIDATA_H
#define DS_ANIDATA_H
/*
    05/15/2003   linda wang       P000289     remove useless code.
*/

#include "OPUS_typedef.h"
#include "ds.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*==================================
**     CS_M210 Icon Images         **
**================================*/
extern DS_COLOR const check_off_image[];
extern DS_COLOR const check_on_image[];
extern DS_COLOR const icon_up_arrow[];
extern DS_COLOR const icon_down_arrow[];
extern DS_COLOR const icon_left_arrow[];
extern DS_COLOR const icon_right_arrow[];
extern DS_COLOR const icon_plus[];
extern DS_COLOR const icon_minus[];
extern DS_COLOR const icon_mul[];
extern DS_COLOR const icon_div[];
extern DS_COLOR const icon_question[];

#ifdef JANE

#else  // ifdef JANE
extern DS_COLOR const titlebar_blue[];
extern DS_COLOR const titlebar_violet[];
extern DS_COLOR const titlebar_green[];
extern DS_COLOR const titlebar_org[];
#endif //ifdef JANE
/*------------------------**
** Main Screen ICON Data  **
**------------------------*/
extern const OP_UINT16 ICON_m_rssi0[];
extern const OP_UINT16 ICON_m_rssi1[];
extern const OP_UINT16 ICON_m_rssi2[];
extern const OP_UINT16 ICON_m_rssi3[];
extern const OP_UINT16 ICON_m_rssi4[];
extern const OP_UINT16 ICON_m_rssi5[];
extern const OP_UINT16 ICON_m_rssi6[];

extern const OP_UINT16 ICON_m_batt0[];
extern const OP_UINT16 ICON_m_batt1[];
extern const OP_UINT16 ICON_m_batt2[];
extern const OP_UINT16 ICON_m_batt3[];
extern const OP_UINT16 ICON_m_vib[];
extern const OP_UINT16 ICON_m_use[];
extern const OP_UINT16 ICON_m_divert[];
extern const OP_UINT16 ICON_m_roam[];
extern const OP_UINT16 ICON_m_msg[];
extern const OP_UINT16 ICON_m_vmail[];
extern const OP_UINT16 ICON_m_alarm[];
extern const OP_UINT16 ICON_m_ds[];
extern const OP_UINT16 ICON_m_gprs[];
extern const OP_UINT16 ICON_m_etiquette[];
extern const OP_UINT16 ICON_m_ssl[];



//kks rcl icon extern types
/*---------------------------------------**
** 검색메뉴 아이콘 (30*22, 14*14,100*70) **
**---------------------------------------*/ 
extern const OP_UINT16 ICON_r_pb1[];
extern const OP_UINT16 ICON_r_pb2[];
extern const OP_UINT16 ICON_r_pb3[];
extern const OP_UINT16 ICON_r_pb4[];
extern const OP_UINT16 ICON_r_pb5[];
extern const OP_UINT16 ICON_r_pb6[];

//phonebook small icon
extern const OP_UINT16 ICON_home[];
extern const OP_UINT16 ICON_off[];
extern const OP_UINT16 ICON_handy[];
extern const OP_UINT16 ICON_etc[];
extern const OP_UINT16 ICON_haddr[];
extern const OP_UINT16 ICON_oaddr[];
extern const OP_UINT16 ICON_mail[];
extern const OP_UINT16 ICON_group[];
extern const OP_UINT16 ICON_voice[];
extern const OP_UINT16 ddd_map[];



/*-----------------------------------**
**   소프트키  아이콘 (18x12)(24x12) **
**-----------------------------------*/


extern DS_COLOR const skey_scroll[];

extern DS_COLOR const icon_num_1_blue[];
extern DS_COLOR const icon_num_2_blue[];
extern DS_COLOR const icon_num_3_blue[];
extern DS_COLOR const icon_num_4_blue[];
extern DS_COLOR const icon_num_5_blue[];
extern DS_COLOR const icon_num_6_blue[];
extern DS_COLOR const icon_num_7_blue[];
extern DS_COLOR const icon_num_8_blue[];
extern DS_COLOR const icon_num_9_blue[];
extern DS_COLOR const icon_num_10_blue[];
extern DS_COLOR const icon_num_11_blue[];
extern DS_COLOR const icon_num_12_blue[];
extern DS_COLOR const icon_num_13_blue[];
extern DS_COLOR const icon_num_14_blue[];
extern DS_COLOR const icon_num_15_blue[];
extern DS_COLOR const icon_num_16_blue[];
extern DS_COLOR const icon_num_17_blue[];
extern DS_COLOR const icon_num_18_blue[];
extern DS_COLOR const icon_num_19_blue[];
extern DS_COLOR const icon_num_20_blue[];

extern DS_COLOR const icon_num_1_violet[];
extern DS_COLOR const icon_num_2_violet[];
extern DS_COLOR const icon_num_3_violet[];
extern DS_COLOR const icon_num_4_violet[];
extern DS_COLOR const icon_num_5_violet[];
extern DS_COLOR const icon_num_6_violet[];
extern DS_COLOR const icon_num_7_violet[];
extern DS_COLOR const icon_num_8_violet[];
extern DS_COLOR const icon_num_9_violet[];
extern DS_COLOR const icon_num_10_violet[];
extern DS_COLOR const icon_num_11_violet[];
extern DS_COLOR const icon_num_12_violet[];
extern DS_COLOR const icon_num_13_violet[];
extern DS_COLOR const icon_num_14_violet[];
extern DS_COLOR const icon_num_15_violet[];
extern DS_COLOR const icon_num_16_violet[];
extern DS_COLOR const icon_num_17_violet[];
extern DS_COLOR const icon_num_18_violet[];
extern DS_COLOR const icon_num_19_violet[];
extern DS_COLOR const icon_num_20_violet[];

extern DS_COLOR const icon_num_1_green[];
extern DS_COLOR const icon_num_2_green[];
extern DS_COLOR const icon_num_3_green[];
extern DS_COLOR const icon_num_4_green[];
extern DS_COLOR const icon_num_5_green[];
extern DS_COLOR const icon_num_6_green[];
extern DS_COLOR const icon_num_7_green[];
extern DS_COLOR const icon_num_8_green[];
extern DS_COLOR const icon_num_9_green[];
extern DS_COLOR const icon_num_10_green[];
extern DS_COLOR const icon_num_11_green[];
extern DS_COLOR const icon_num_12_green[];
extern DS_COLOR const icon_num_13_green[];
extern DS_COLOR const icon_num_14_green[];
extern DS_COLOR const icon_num_15_green[];
extern DS_COLOR const icon_num_16_green[];
extern DS_COLOR const icon_num_17_green[];
extern DS_COLOR const icon_num_18_green[];
extern DS_COLOR const icon_num_19_green[];
extern DS_COLOR const icon_num_20_green[];

extern DS_COLOR const icon_num_1_org[];
extern DS_COLOR const icon_num_2_org[];
extern DS_COLOR const icon_num_3_org[];
extern DS_COLOR const icon_num_4_org[];
extern DS_COLOR const icon_num_5_org[];
extern DS_COLOR const icon_num_6_org[];
extern DS_COLOR const icon_num_7_org[];
extern DS_COLOR const icon_num_8_org[];
extern DS_COLOR const icon_num_9_org[];
extern DS_COLOR const icon_num_10_org[];
extern DS_COLOR const icon_num_11_org[];
extern DS_COLOR const icon_num_12_org[];
extern DS_COLOR const icon_num_13_org[];
extern DS_COLOR const icon_num_14_org[];
extern DS_COLOR const icon_num_15_org[];
extern DS_COLOR const icon_num_16_org[];
extern DS_COLOR const icon_num_17_org[];
extern DS_COLOR const icon_num_18_org[];
extern DS_COLOR const icon_num_19_org[];
extern DS_COLOR const icon_num_20_org[];


extern RM_ICON_T const icon_num_1[];
extern RM_ICON_T const icon_num_2[];
extern RM_ICON_T const icon_num_3[];
extern RM_ICON_T const icon_num_4[];
extern RM_ICON_T const icon_num_5[];
extern RM_ICON_T const icon_num_6[];
extern RM_ICON_T const icon_num_7[];
extern RM_ICON_T const icon_num_8[];
extern RM_ICON_T const icon_num_9[];

#ifdef JANE

#else //ifdef JANE
/*-----------------------------------**
** 타이틀바 배경 **
**-----------------------------------*/

#endif //ifdef JANE
/*-------------------**
** Radio button 표시 **      
**-------------------*/
extern DS_COLOR const radio_off[];
extern DS_COLOR const radio_on[];

/*-------------------**
**  Check Box  표시  **      
**-------------------*/

/*-------------------**
**  Arrow Key  표시  **      
**-------------------*/

/*-------------------**
**  계산기Icon 표시  **      
**-------------------*/

/*-------------------**
** 전자수첩 공용Icon **      
**-------------------*/
//e.s.choi 20010921;to add msg icons
//e.s.choi 20010921;end

/*--------------------------**
**  Images for Idle Screen  **      
**--------------------------*/
// 배경모드 대기화면 - idle screen image
#define IDLEIMAGES     5

#define MAXIDLEANI     2 //change by Alex_0322
//extern NORMALANIDATA const aniIdle[];

// 시계모드 대기화면 - analog clock background frame image
#define ACLOCKIMAGES    3


#define DIGITCHARS   16   // 각 폰트별 디지트 문자 수



/* end of "anidata.h" */

#endif //ifdef JANE
