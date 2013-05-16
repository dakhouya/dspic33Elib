#ifndef __GLOBALDEF_H_
#define __GLOBALDEF_H_

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Thoses types are in stdint and others

//typedef unsigned char      bool;
//typedef unsigned char      uint8_t;
//typedef char               int8_t;
//typedef unsigned int       uint16_t;
//typedef int                int16_t;
//typedef unsigned long      uint32_t;
//typedef long               int32_t;
//typedef unsigned long long uint64_t;
//typedef long long          int64_t;

#ifndef NULL
#define NULL    0
#endif

#define TRUE    1
#define FALSE   0
#define ENABLE  1
#define DISABLE 0
#define ON      1
#define OFF     0

#endif // __GLOBALDEF_H_
