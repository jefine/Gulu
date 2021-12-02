#include "jx_uart_send.h"
#include "stdio.h"
#include "usart.h"
// 串口通信消息头
const unsigned char g_uart_send_head[] = {
  0xaa, 0x55
};

// 串口通信消息尾
const unsigned char g_uart_send_foot[] = {
  0x55, 0xaa
};

// 串口发送函数实现
void _uart_send_impl(unsigned char* buff, int len) {
  
  int i = 0;
  unsigned char c;
  for (i = 0; i < len; i++) {
    c = buff[i];
  //in here .stm32 need to use %c to send hex("%02X" is incorrect).
  printf("%c", c);
  
  }
  printf("\n");
  
}

// 十六位整数转32位整数
void _int16_to_int32(uart_param_t* param) {
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = (value >> 15) & 1;
  unsigned long v = value;
  if (sign)
    v = 0xFFFF0000 | value;
  uart_param_t p;
  p.d_long = v;
  param->d_ucs[0] = p.d_ucs[0];
  param->d_ucs[1] = p.d_ucs[1];
  param->d_ucs[2] = p.d_ucs[2];
  param->d_ucs[3] = p.d_ucs[3];
}

// 浮点数转双精度 
void _float_to_double(uart_param_t* param) {
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = value >> 31;
  unsigned long M = value & 0x007FFFFF;
  unsigned long e =  ((value >> 23 ) & 0xFF) - 127 + 1023;
  uart_param_t p0, p1;  
  p1.d_long = ((sign & 1) << 31) | ((e & 0x7FF) << 20) | (M >> 3);
  p0.d_long = (M & 0x7) << 29;
  param->d_ucs[0] = p0.d_ucs[0];
  param->d_ucs[1] = p0.d_ucs[1];
  param->d_ucs[2] = p0.d_ucs[2];
  param->d_ucs[3] = p0.d_ucs[3];
  param->d_ucs[4] = p1.d_ucs[0];
  param->d_ucs[5] = p1.d_ucs[1];
  param->d_ucs[6] = p1.d_ucs[2];
  param->d_ucs[7] = p1.d_ucs[3];
}

// action: Weather
void _uart_Weather(int TempLocalVal, int TempWebVal) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_Weather;
  param.d_int = TempLocalVal;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  param.d_int = TempWebVal;
  _int16_to_int32(&param);
  buff[7] = param.d_ucs[0];
  buff[8] = param.d_ucs[1];
  buff[9] = param.d_ucs[2];
  buff[10] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

