#pragma once

#define arrayLen(array) (sizeof(array) / sizeof(array[0]))

#define invalidDefaultCase default: verify(false); break;
