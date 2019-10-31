#ifndef LOG_H
#define LOG_H

#include "SEGGER_RTT.h"

#define LOGE(tag,msg,...) { \
      char _out[256]; \
      sprintf(_out,"[E] " msg, ##__VA_ARGS__); \
      SEGGER_RTT_WriteString(0, _out); \
    }

#define LOGI(tag,msg,...) { \
    char _out[256]; \
    sprintf(_out,"[I] " msg, ##__VA_ARGS__); \
    SEGGER_RTT_WriteString(0, _out); \
  }

//#define TRACE_LOG
#ifdef TRACE_LOG
#define LOGT(tag,msg,...) { \
    char _out[256]; \
    sprintf(_out,"[T] " msg, ##__VA_ARGS__); \
    SEGGER_RTT_WriteString(0, _out); \
  }
#else
#define LOGT(tag,msg,...)
#endif

#define ERROR(tag,msg) { \
    LOGE(tag,msg)    \
    while(1) {} \
}

#endif // LOG_H
