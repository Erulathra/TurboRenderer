#pragma once

#include <float.h>
#include <stdint.h>

// ##################################################################################
//                                      TypeDefs
// ##################################################################################

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

using iPtr = uintptr_t;

using fp32 = float;
using fp64 = double;

using LiteralString = const char*;
using CString = const char*;

/* Numeric ranges */
#define I16_MIN INT16_MIN
#define I16_MAX INT16_MAX
#define U16_MAX UINT16_MAX

#define I32_MIN INT32_MIN
#define I32_MAX INT32_MAX
#define U32_MAX UINT32_MAX

#define I64_MIN INT64_MIN
#define I64_MAX INT64_MAX
#define U64_MAX UINT64_MAX

#define FP32_MIN FLT_MIN
#define FP32_MAX FLT_MAX

#define FP64_MIN DBL_MIN
#define FP64_MAX DBL_MAX

/* Other */
#define INDEX_NONE (-1)
