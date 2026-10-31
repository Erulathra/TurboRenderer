#pragma once

// ##################################################################################
//                                       Common
// ##################################################################################

#define arrayLen(array) (sizeof(array) / sizeof(array[0]))
#define invalidDefaultCase default: verify(false); break;

// ##################################################################################
//                                   Math constants
// ##################################################################################

#define SMALL_NUMBER 1e-4
#define VERY_SMALL_NUMBER 1e-8

#define TE_PI (3.14159265358979323846264)
#define TE_TAU (6.28318530717958647692528)

#define TE_DEG_TO_RAD (0.01745329251994329576923)
#define TE_RAD_TO_DEG (57.2957795130823208767981)

#define TE_MILI  (1e-3)
#define TE_MICRO (1e-6)
#define TE_NANO  (1e-9)

#define TE_INV_MILI  (1e3)
#define TE_INV_MICRO (1e6)
#define TE_INV_NANO  (1e9)
