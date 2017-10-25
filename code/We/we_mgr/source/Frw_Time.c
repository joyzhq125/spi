/*
 * Copyright (C) Techfaith 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */










#include "Frw_Time.h"
#include "Frw_Sig.h"
#include "Frw.h"
#include "Frw_Int.h"

#include "We_Core.h"
#include "We_Log.h"
#include "We_Mem.h"
#include "We_Cfg.h"
#include "We_File.h"





#define WE_MAX_UINT32                0xFFFFFFFF

#define FRW_FILE_OP_WRITE             1
#define FRW_FILE_OP_READ              2

#define FRW_TIMERS_PATHNAME           "/frw"
#define FRW_TIMERS_FILENAME           "/frw/timers.dat"

#define FRW_TIMER_FILE_BLOCK_SIZE     14
#define FRW_TIMER_FILE_CHECKSUM_SIZE  4
#define FRW_TIMER_FILE_READ_SIZE      504  

#define FRW_TIMER_BLOCK_USED           1
#define FRW_TIMER_BLOCK_NOT_USED       2
#define FRW_TIMER_BLOCK_ERROR          3






typedef struct frw_timer_st {
  struct frw_timer_st *next;
  WE_UINT8            modId;
  WE_UINT32           timerID;
  WE_UINT8            expire_overflow; 
  WE_UINT32           expire_tick;
  WE_UINT16           expire_mtick;    
} frw_timer_t;

typedef struct frw_timer_persistent_st {
  struct frw_timer_persistent_st *next;
  WE_UINT8                       modId;
  WE_UINT32                      timerID;
  WE_UINT32                      expire_sec;
  WE_INT32                       file_pos;   

} frw_timer_persistent_t;

typedef struct  frw_timer_file_queue_st{
  int                             file;
  int                             op;
  WE_INT32                       pos;
  WE_INT32                       nb;
  unsigned char                  *data;
  WE_INT32                       data_len;
  int                             resp_dst; 
  int                             resp_sig;
  struct frw_timer_file_queue_st *next;
} frw_timer_file_queue_t;

typedef struct {
  unsigned char   *data;
  WE_INT32        data_len;
} frw_timer_file_read_resp_t;






static frw_timer_t            *frw_timer_list;
static frw_timer_persistent_t *frw_timer_persitent_list;

static WE_UINT32              frw_timer_last_ticks;

static frw_timer_file_queue_t *frw_timer_file_queue_first;
static frw_timer_file_queue_t *frw_timer_file_queue_last;
static long                    frw_timer_bytes_read;

static int                     frw_timer_terminating;





static void
frw_timer_main (frw_signal_t *sig);

static void 
frw_timer_read_resp (frw_timer_file_read_resp_t *p);

static void 
frw_timer_init_persistent_timers ();

static void
frw_timer_file_init (void);

static void
frw_timer_file_execute (void);

static void
frw_timer_file_write (WE_INT32            pos, 
                      const unsigned char *data, 
                      WE_INT32            data_len);

static void
frw_timer_file_read (WE_INT32  pos,  
                     WE_INT32  data_len,
                     WE_UINT32 resp_dst, 
                     WE_UINT32 resp_sig);

static int
frw_timer_file_terminate ();





void
frw_timer_init (void)
{
  frw_signal_register_dst (FRW_SIG_DST_FRW_TIMER, frw_timer_main);
  frw_timer_file_init ();
  frw_timer_list = NULL;
  frw_timer_last_ticks = 0;
  frw_timer_terminating = FALSE;
  frw_timer_init_persistent_timers ();
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW: TIMER - initialized\n"));
}

void
frw_timer_terminate_continue (void)
{
  frw_signal_deregister (FRW_SIG_DST_FRW_TIMER);
  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW: TIMER - terminated\n"));

}

void
frw_timer_terminate (void)
{
  FRWa_resetTimer ();

  while (frw_timer_list) {
    frw_timer_t *t = frw_timer_list;

    frw_timer_list = t->next;
    FRW_FREE (t);
  }
  while (frw_timer_persitent_list) {
    frw_timer_persistent_t *t = frw_timer_persitent_list;

    frw_timer_persitent_list = t->next;
    FRW_FREE (t);
  }

  if(frw_timer_file_terminate() == TRUE){
    
    frw_timer_terminate_continue ();
  }
  else {
    
    frw_sub_module_terminate_delay ();
  }
}

static void
frw_timer_main (frw_signal_t *sig)
{
  switch(sig->type) {
  case FRW_TIMER_SIG_SET:
    frw_timer_set ((WE_UINT8)(sig->i_param), sig->u_param1, sig->u_param2);
    break;
  case FRW_TIMER_SIG_RESET:
    frw_timer_reset ((WE_UINT8)(sig->i_param), sig->u_param1);
    break;
  case FRW_TIMER_SIG_EXPIRED:
    frw_timer_expired (FALSE);
    break;
  case FRW_TIMER_SIG_SET_PERSISTENT:
    frw_timer_set_persistent ((WE_UINT8)(sig->i_param), sig->u_param1, sig->u_param2);
    break;
  case FRW_TIMER_SIG_RESET_PERSISTENT:
    frw_timer_reset_persistent ((WE_UINT8)(sig->i_param), sig->u_param1);
    break;
  case FRW_TIMER_SIG_PERSISTENT_EXPIRED:
    frw_timer_persistent_expired ();
    break;
  case FRW_TIMER_SIG_FILE_EXECUTE:
    frw_timer_file_execute ();
    break;
  case FRW_TIMER_SIG_TIMER_READ_RESP:
    frw_timer_read_resp (sig->p_param);
    break;

  }
  frw_signal_delete (sig);
}





static void
frw_timer_send_expire (WE_UINT8 modId, WE_UINT32 timerID)
{
  we_timer_expired_t timer;
  we_dcvt_t          cvt_obj;
  void                *buffer, *user_data;
  WE_UINT16          length;

  timer.timerID = timerID;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_EXPIRED, WE_MODID_FRW, modId, 4);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_timer_expired (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}

static void
frw_timer_ms2tick(WE_UINT32 now, WE_UINT32 ms, frw_timer_t* t)
{
  

  WE_UINT32 tick_interval;
  WE_UINT32 sec;
  WE_UINT32 left;

  



  sec = ms / 1000;
  ms %= 1000;
  tick_interval = sec * WE_TICKS_PER_SECOND;
  tick_interval += (ms * WE_TICKS_PER_SECOND) / 1000;

  
  left = WE_MAX_UINT32 - now;
  if (tick_interval <= left) {
    t->expire_overflow = 0;
    t->expire_tick = now + tick_interval;
  }
  else {
    t->expire_overflow = 1; 
    t->expire_tick = tick_interval - left - 1;
  }
  t->expire_mtick = (WE_UINT16) ((ms * WE_TICKS_PER_SECOND) % 1000);
}

static WE_UINT32
frw_timer_tick2ms(WE_UINT32 now, frw_timer_t* t)
{
  

  WE_UINT32 tick_interval;
  WE_UINT32 sec;
  WE_UINT32 ms;
  WE_UINT32 left;

  





  if (t->expire_overflow) {
    left = WE_MAX_UINT32 - now;
    


    tick_interval =  left + 1 + t->expire_tick;
  }
  else {
    tick_interval = t->expire_tick >= now ? t->expire_tick - now : 0;
  }
    
  sec = tick_interval / WE_TICKS_PER_SECOND;
  tick_interval %= WE_TICKS_PER_SECOND; 
  ms = (1000*tick_interval + t->expire_mtick) / WE_TICKS_PER_SECOND;

  
  return 1000*sec + ms;
}

static void
frw_timer_new_period(WE_UINT32 new_expire_tick)
{
  frw_timer_t *t = frw_timer_list;

  



  while (t) {
    if (t->expire_overflow == 0) {
      t->expire_tick = new_expire_tick;
      t->expire_mtick = 0;
    }
    t->expire_overflow = 0;
    t = t->next;
  }
}

void
frw_timer_expired (int do_reset)
{
  frw_timer_t *t = frw_timer_list;
  WE_UINT32   now;
  WE_UINT32   ms;

  if (do_reset)
    FRWa_resetTimer ();

  if (!t)
    return;

  now = TPIa_timeGetTicks ();

  
  if (now < frw_timer_last_ticks) {
    frw_timer_new_period(0);
  }
  frw_timer_last_ticks = now;

  






  while (t && (t->expire_overflow == 0) &&
         ((t->expire_tick < now) ||
          ((t->expire_tick == now) && (t->expire_mtick == 0)))) {

    frw_timer_send_expire (t->modId, t->timerID);

    
    frw_timer_list = t->next;
    FRW_FREE (t);
    t = frw_timer_list;
  }

  if (frw_timer_list) {
    ms = frw_timer_tick2ms (now, frw_timer_list);
    if (ms == 0) {
      


 
      ms = 1;
    }
    FRWa_setTimer (ms);
  }
}

void
frw_timer_set (WE_UINT8 modId, WE_UINT32 timerID, WE_UINT32 msInterval)
{
  frw_timer_t     *prev = NULL;
  frw_timer_t     *t = frw_timer_list;
  frw_timer_t     *timer;
  int            has_reset = 0;
  WE_UINT32     now = TPIa_timeGetTicks ();

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_TIMER: set timer (%d, %d, %d)\n", modId, timerID, msInterval));

  
  if (now < frw_timer_last_ticks) {
    frw_timer_new_period(now);
  }
  frw_timer_last_ticks = now;

  
  while (t && ((t->modId != modId) || (t->timerID != timerID))) {
    prev = t;
    t = t->next;
  }
  if (t) {
    if (prev) 
      prev->next = t->next;
    else { 
      frw_timer_list = t->next;
      FRWa_resetTimer ();
      has_reset = 1;
    }
    timer = t;
  }
  else {
    timer = FRW_ALLOCTYPE (frw_timer_t);
    timer->modId = modId;
    timer->timerID = timerID;
  }

  frw_timer_ms2tick(now, msInterval, timer);

  

  for (prev = NULL, t = frw_timer_list; t; prev = t, t = t->next) {
    if (timer->expire_overflow < t->expire_overflow)
        break;
      if (timer->expire_overflow == t->expire_overflow) {
        if (timer->expire_tick < t->expire_tick)
          break;
        if ((timer->expire_tick == t->expire_tick) &&
            (timer->expire_mtick < t->expire_mtick))
          break;
      }
  }

  
  timer->next = t;

  if (prev) { 
    prev->next = timer;
  }
  else {
    
    if (t) {
      if (!has_reset) {
        FRWa_resetTimer ();
        has_reset = TRUE;
      }
    }
    frw_timer_list = timer;
  }
  if (has_reset || (frw_timer_list->next == NULL)) {
    FRWa_setTimer (frw_timer_tick2ms(now, frw_timer_list));
  }
}

void
frw_timer_reset (WE_UINT8 modId, WE_UINT32 timerID)
{
  frw_timer_t *prev = NULL;
  frw_timer_t *t = frw_timer_list;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_TIMER: reset timer (%d, %d)\n", modId, timerID));
  
  while (t && ((t->modId != modId) || (t->timerID != timerID))) {
    prev = t;
    t = t->next;
  }
  if (!t)
    return;

  if (prev) { 
    prev->next = t->next;
  }
  else { 
    frw_timer_list = t->next;
    frw_timer_expired (TRUE);
  }
  FRW_FREE (t);
}

void
frw_timer_reset_all (WE_UINT8 modId)
{
  frw_timer_t *prev = NULL;
  frw_timer_t *t = frw_timer_list;
  int        first_timer_is_removed = FALSE;;

  
  while (t) {
    if (t->modId == modId) {
      if (prev) { 
        prev->next = t->next;
        FRW_FREE (t);
        t = prev->next;
      }
      else { 
        first_timer_is_removed = TRUE;
        frw_timer_list = t->next;
        FRW_FREE (t);
        t = frw_timer_list;
      }
    }
    else {
      prev = t;
      t = t->next;
    }
  }

  if (first_timer_is_removed) {
    frw_timer_expired (TRUE);
  }
}





#define FRW_TIMER_CLEAR_BIT(a, b) a ^= (0x01 << ((b) - 1)); 

WE_UINT32
frw_crc (const WE_UINT8 *p, WE_UINT32 len){
  WE_UINT32  i,n;
  WE_UINT32  r;
  unsigned long  poly = 0x04C11DB7L;
  unsigned long  ch;

  r = 0;

  for(i=0;i<len;i++){
    ch = p[i];
    r = r ^ (ch << 24);
    for(n=0;n<8;n++){
      if (r & 0x80000000){
        r = (r << 1) ^ poly;
      }
      else{
        r = (r << 1);
      }
    }
  }
  return r;
}
   
static long
frw_timer_find_free_pos (void)
{
  frw_timer_persistent_t *t;
  WE_UINT32              r;
  WE_INT32              max = 0;
  WE_INT32              min = 0;
  int                     i   = 1;
  

  for (;;){
    t = frw_timer_persitent_list;
    min = max;
    max = (FRW_TIMER_FILE_BLOCK_SIZE * i * 32);
    
    r = 0xFFFFFFFF;
    while (t != NULL){
      if ((t->file_pos >= min) && (t->file_pos < max)) {
        FRW_TIMER_CLEAR_BIT(r, ((t->file_pos - min) / FRW_TIMER_FILE_BLOCK_SIZE) + 1);
      }
      t = t->next;
    }
    
    if (r == 0){
      i++;
      continue;
    }
    
    
    r =r^(r-1);
    r -=  (r>>1) & 0x55555555;
    r  = ((r>>2) & 0x33333333) + (r & 0x33333333);
    r  = ((r>>4) + r) & 0x0f0f0f0f;
    r *= 0x01010101;
    r  =  r>>24;
    
    return ((r-1) * FRW_TIMER_FILE_BLOCK_SIZE + (FRW_TIMER_FILE_BLOCK_SIZE * (i - 1) * 32));
  }
}

static void
frw_timer_encode_timer (WE_UINT8 modid, WE_UINT32 timer_id, 
                        WE_UINT32 time, unsigned char *data)
{
  we_dcvt_t     cvt_obj;
  unsigned char  used = 0xFF;
  WE_UINT32     checksum;

  we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, data, FRW_TIMER_FILE_BLOCK_SIZE, WE_MODID_FRW);
  we_dcvt_uint8 (&cvt_obj, &used);
  we_dcvt_uint8 (&cvt_obj, &modid);
  we_dcvt_uint32(&cvt_obj, &timer_id);
  we_dcvt_uint32(&cvt_obj, &time);

  checksum = frw_crc (data, FRW_TIMER_FILE_BLOCK_SIZE - FRW_TIMER_FILE_CHECKSUM_SIZE);
  we_dcvt_uint32(&cvt_obj, &checksum);
}

static int
frw_timer_decode_timer (unsigned char *data, WE_INT32 data_len, 
                        WE_UINT8 *modid, WE_UINT32 *timer_id, WE_UINT32 *time)
{
  we_dcvt_t    cvt_obj;
  WE_UINT32    checksum;
  unsigned char used;
     
  we_dcvt_init (&cvt_obj, WE_DCVT_DECODE, data, data_len, WE_MODID_FRW);

  if (!we_dcvt_uint8 (&cvt_obj, &used)    ||
      !we_dcvt_uint8 (&cvt_obj, modid)    ||
      !we_dcvt_uint32(&cvt_obj, timer_id) ||
      !we_dcvt_uint32(&cvt_obj, time)     ||
      !we_dcvt_uint32(&cvt_obj, &checksum)){
    return FRW_TIMER_BLOCK_ERROR;
  }
  
  if (checksum != frw_crc (data, FRW_TIMER_FILE_BLOCK_SIZE - FRW_TIMER_FILE_CHECKSUM_SIZE)){
    
    return FRW_TIMER_BLOCK_ERROR;
  }

  if ((used == 0xFF))
    return FRW_TIMER_BLOCK_USED;

  if ((used == 0x00))
    return FRW_TIMER_BLOCK_NOT_USED;

  return FRW_TIMER_BLOCK_ERROR;
}

void
frw_timer_remove_timer_from_file (WE_INT32 file_pos)
{
  unsigned char  data[FRW_TIMER_FILE_BLOCK_SIZE];

  
  memset (data, 0, FRW_TIMER_FILE_BLOCK_SIZE);
  frw_timer_file_write (file_pos, data, FRW_TIMER_FILE_BLOCK_SIZE);
}

static void 
frw_timer_read_resp (frw_timer_file_read_resp_t *p)
{
  unsigned char *data;
  WE_INT32      data_len;
  int            i = 0;

  data_len = p->data_len;
  data = p->data;

  while (data_len > 0){
    frw_timer_persistent_t *timer;
    frw_timer_persistent_t *prev = NULL;
    frw_timer_persistent_t *t = frw_timer_persitent_list;
    WE_UINT8               modid;
    WE_UINT32              timer_id;
    WE_UINT32              time;
    int                     r;
    
    r = frw_timer_decode_timer (data, data_len, &modid, &timer_id, &time);
    if ( r == FRW_TIMER_BLOCK_USED){    
      timer             = FRW_ALLOCTYPE (frw_timer_persistent_t);
      timer->modId      = modid;
      timer->timerID    = timer_id;
      timer->expire_sec = time;
      timer->file_pos   = frw_timer_bytes_read  + (i * FRW_TIMER_FILE_BLOCK_SIZE);
      
      
      

      for (prev = NULL, t = frw_timer_persitent_list; t; prev = t, t = t->next) {
        if (timer->expire_sec <= t->expire_sec)
          break;
      }
      
      
      timer->next = t;
      
      if (prev) { 
        prev->next = timer;
      }
      else {
        
        frw_timer_persitent_list = timer;
      }
    }
    else if (r == FRW_TIMER_BLOCK_ERROR){
      



      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, 
            "FRW_TIMER: ERROR. Invalid timer found.\n"));
      frw_timer_remove_timer_from_file(frw_timer_bytes_read + i * FRW_TIMER_FILE_BLOCK_SIZE);
    }
    
    
    data     += FRW_TIMER_FILE_BLOCK_SIZE;
    data_len -= FRW_TIMER_FILE_BLOCK_SIZE;
    i++;
  }
    
  if (p->data_len == FRW_TIMER_FILE_READ_SIZE){
    
    frw_timer_bytes_read += p->data_len;
    frw_timer_file_read (frw_timer_bytes_read, FRW_TIMER_FILE_READ_SIZE, FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_TIMER_READ_RESP);
  }
  else {
    
    FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_SUB_MOD_INITIALISED);
    if (frw_timer_persitent_list != NULL)
      FRWa_setPersistentTimer (frw_timer_persitent_list->modId, frw_timer_persitent_list->expire_sec);
  }
  FRW_FREE (p->data);
  FRW_FREE (p);
}


static void
frw_timer_init_persistent_timers (void)
{
  frw_timer_bytes_read = 0;
  frw_timer_persitent_list = NULL;
  
  frw_sub_module_init_delay ();
  frw_timer_file_read (frw_timer_bytes_read, FRW_TIMER_FILE_READ_SIZE, FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_TIMER_READ_RESP);
}


static void
frw_timer_send_persistent_expire (WE_UINT8 modId, WE_UINT32 timerID)
{
  we_timer_expired_t timer;
  we_dcvt_t          cvt_obj;
  void                *buffer, *user_data;
  WE_UINT16          length;

  timer.timerID = timerID;
  buffer = WE_SIGNAL_CREATE (WE_SIG_TIMER_PERSISTENT_EXPIRED, WE_MODID_FRW, modId, 4);
  if (buffer != NULL){
    user_data = WE_SIGNAL_GET_USER_DATA (buffer, &length);
    we_dcvt_init (&cvt_obj, WE_DCVT_ENCODE, user_data, length, WE_MODID_FRW);
    we_cvt_timer_expired (&cvt_obj, &timer);
    WE_SIGNAL_SEND (buffer);
  }
}

void
frw_timer_set_persistent (WE_UINT8 modId, WE_UINT32 timerID, WE_UINT32 date_time)
{
  frw_timer_persistent_t     *prev = NULL;
  frw_timer_persistent_t     *t = frw_timer_persitent_list;
  frw_timer_persistent_t     *timer;
  int            has_reset = 0;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_TIMER: set persistent timer (%d, %d, %d)\n", modId, timerID, date_time));
  
  while (t && ((t->modId != modId) || (t->timerID != timerID))) {
    prev = t;
    t = t->next;
  }
  if (t) {
    if (prev) 
      prev->next = t->next;
    else { 
      frw_timer_persitent_list = t->next;
      FRWa_resetPersistentTimer (); 
      has_reset = 1;
    }
    timer = t;
  }
  else {
    timer = FRW_ALLOCTYPE (frw_timer_persistent_t);
    timer->modId = modId;
    timer->timerID = timerID;
    timer->file_pos = frw_timer_find_free_pos ();
  }
  timer->expire_sec = date_time;

  

  for (prev = NULL, t = frw_timer_persitent_list; t; prev = t, t = t->next) {
    if (timer->expire_sec <= t->expire_sec)
      break;
  }

  
  timer->next = t;

  if (prev) { 
    prev->next = timer;
  }
  else {
    
    if (t) {
      if (!has_reset) {
        FRWa_resetPersistentTimer (); 
        has_reset = TRUE;
      }
    }
    frw_timer_persitent_list = timer;
  }

  {
    unsigned char data[FRW_TIMER_FILE_BLOCK_SIZE];

    
    frw_timer_encode_timer (modId, timerID, timer->expire_sec, data);
    frw_timer_file_write (timer->file_pos, data, FRW_TIMER_FILE_BLOCK_SIZE);
  }

  if (has_reset || (frw_timer_persitent_list->next == NULL)) {
    FRWa_setPersistentTimer (frw_timer_persitent_list->modId, frw_timer_persitent_list->expire_sec);
  }
}

void
frw_timer_reset_persistent (WE_UINT8 modId, WE_UINT32 timerID)
{
  frw_timer_persistent_t *prev = NULL;
  frw_timer_persistent_t *t    = frw_timer_persitent_list;

  WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, "FRW_TIMER: reset persistent timer (%d, %d)\n", modId, timerID));
  
  while (t && ((t->modId != modId) || (t->timerID != timerID))) {
    prev = t;
    t = t->next;
  }
  if (!t)
    return;

  if (prev) { 
    prev->next = t->next;
  }
  else { 
    frw_timer_persitent_list = t->next;
    frw_timer_expired (TRUE);
  }

  
  frw_timer_remove_timer_from_file(t->file_pos);

  FRW_FREE (t);
}

void
frw_timer_persistent_expired (void)
{
  frw_timer_persistent_t *t   = frw_timer_persitent_list;
  WE_UINT32              now = TPIa_timeGetCurrent ();

  if (!t)
    return;

  


  while (t && (t->expire_sec <= now)) {

    
    if(frw_get_module_status(t->modId)!= MODULE_STATUS_ACTIVE){
      frw_start_module_internal (t->modId, 
                                 FRW_SIG_DST_FRW_TIMER,
                                 FRW_TIMER_SIG_PERSISTENT_EXPIRED);
      return;
    }

    frw_timer_send_persistent_expire (t->modId, t->timerID);

    
    frw_timer_remove_timer_from_file(t->file_pos);

    
    frw_timer_persitent_list = t->next;
    FRW_FREE (t);
    t = frw_timer_persitent_list;
  }

  if (frw_timer_persitent_list) {
    t = frw_timer_persitent_list;
    FRWa_setPersistentTimer (t->modId, t->expire_sec);
  }
}





static void
frw_timer_file_init (void)
{
  frw_timer_file_queue_first = NULL;
  frw_timer_file_queue_last  = NULL;
  WE_FILE_MKDIR (FRW_TIMERS_PATHNAME);
}

static void
frw_timer_file_execute ()
{
  frw_timer_file_queue_t *p = frw_timer_file_queue_first;
  long                    r = 0;
  long                    (*fp)();
  int                     mode;
  int                     evt;

  if (p == NULL)
    return;

  if (p->op == FRW_FILE_OP_WRITE){
    fp = WE_FILE_WRITE;
    mode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
    evt = TPI_FILE_EVENT_WRITE;
  }
  else {
    fp = WE_FILE_READ;
    mode = WE_FILE_SET_RDONLY | WE_FILE_SET_CREATE;
    evt = TPI_FILE_EVENT_READ;
  }

  if (p->file < 0){
    p->file = WE_FILE_OPEN (WE_MODID_FRW, FRW_TIMERS_FILENAME, mode, 0);
    if (p->file < 0){
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, 
        "FRW_TIMER: ERROR Failed to open file.\n"));
      return;
    }
    frw_register_file_handle (p->file, FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_FILE_EXECUTE);
  }
    
  if (p->pos != WE_FILE_SEEK (p->file, p->pos, TPI_FILE_SEEK_SET)){
    WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, 
      "FRW_TIMER: ERROR Failed set position in file.\n"));
    return;
  }
  
  while (p->data_len > 0 ){
    r = fp (p->file, &p->data[p->nb], p->data_len);
    
    if (r < 0){
      if (r == WE_FILE_ERROR_DELAYED){
        WE_FILE_SELECT ( p->file, evt);
        return;
      }
      else if (r == WE_FILE_ERROR_EOF){
        break;
      }
      WE_LOG_MSG ((WE_LOG_DETAIL_HIGH, WE_MODID_FRW, 
        "FRW_TIMER: ERROR Failed to write/read file. Error %i\n", r));
    }
    p->pos      += r;
    p->data_len -= r;
    p->nb       += r;
  }

  WE_FILE_CLOSE (p->file);
  frw_deregister_file_handle (p->file);

  
  frw_timer_file_queue_first = p->next;
  if (frw_timer_file_queue_first == NULL)
    frw_timer_file_queue_last = NULL;

  if ((p->resp_dst >= 0) && (p->resp_sig >= 0)){
    frw_timer_file_read_resp_t *resp;
    resp           = FRW_ALLOCTYPE (frw_timer_file_read_resp_t);
    resp->data     = p->data;
    resp->data_len = p->nb;
    FRW_SIGNAL_SENDTO_P (p->resp_dst, p->resp_sig, resp);
    p->data = NULL;
  }
    
  FRW_FREE (p->data);
  FRW_FREE (p);

  if (frw_timer_file_queue_first != NULL){
    FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_FILE_EXECUTE);
  }
  else {
    if (frw_timer_terminating == TRUE){
      FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_MAIN, FRW_MAIN_SIG_SUB_MOD_TERMINATED);
      frw_timer_terminate_continue ();
    }
  }
}

static void
frw_timer_file_write (WE_INT32            pos, 
                      const unsigned char *data, 
                      WE_INT32            data_len)
{
  frw_timer_file_queue_t *p;

  p = FRW_ALLOCTYPE (frw_timer_file_queue_t);

  p->file     = -1;
  p->op       = FRW_FILE_OP_WRITE;
  p->pos      = pos;
  p->nb       = 0;
  p->data     = memcpy(FRW_ALLOC (data_len), data, data_len);
  p->data_len = data_len;
  p->resp_dst = -1;
  p->resp_sig = -1;
  p->next     = NULL;
  
  if (frw_timer_file_queue_last == NULL) {
    frw_timer_file_queue_last  = p;
    frw_timer_file_queue_first = p;
  }
  else {
    frw_timer_file_queue_last->next = p;
    frw_timer_file_queue_last = p;
  }
  
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_FILE_EXECUTE);
}

static void
frw_timer_file_read (WE_INT32  pos,  
                     WE_INT32  data_len,
                     WE_UINT32 resp_dst, 
                     WE_UINT32 resp_sig)
{
    frw_timer_file_queue_t *p;

  p = FRW_ALLOCTYPE (frw_timer_file_queue_t);

  p->file     = -1;
  p->op       = FRW_FILE_OP_READ;
  p->pos      = pos;
  p->nb       = 0;
  p->data     = FRW_ALLOC (data_len);
  p->data_len = data_len;
  p->resp_dst = resp_dst;
  p->resp_sig = resp_sig;
  p->next     = NULL;
  
  if (frw_timer_file_queue_last == NULL) {
    frw_timer_file_queue_last  = p;
    frw_timer_file_queue_first = p;
  }
  else {
    frw_timer_file_queue_last->next = p;
    frw_timer_file_queue_last = p;
  }
  
  FRW_SIGNAL_SENDTO (FRW_SIG_DST_FRW_TIMER, FRW_TIMER_SIG_FILE_EXECUTE);

}

static int
frw_timer_file_terminate ()
{
  if (frw_timer_file_queue_first == NULL){
    
    return TRUE;
  }

  
  frw_timer_terminating = TRUE;

  return FALSE;
}


