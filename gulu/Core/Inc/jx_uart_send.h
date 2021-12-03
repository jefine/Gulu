#ifndef JX_UART_SEND_H_
#define JX_UART_SEND_H_

#ifdef __cplusplus
extern "C" {
#endif

// 串口发送消息最大长度
#define UART_SEND_MAX      32
#define UART_MSG_HEAD_LEN  2
#define UART_MSG_FOOT_LEN  2

// 串口发送消息号
#define U_MSG_Time      2
#define U_MSG_Weather      1

// 串口消息参数类型
typedef union {
  double d_double;
  int d_int;
  unsigned char d_ucs[8];
  char d_char;
  unsigned char d_uchar;
  unsigned long d_long;
  short d_short;
  float d_float;
}uart_param_t;

void _int16_to_int32(uart_param_t* param);
void _float_to_double(uart_param_t* param);

// action: Weather
void _uart_Weather(int TempLocalVal, int TempWebVal);
void _uart_Time(int HH, int MM);
#ifdef __cplusplus
}
#endif

#endif /*JX_UART_SEND_H_*/

