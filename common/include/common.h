#pragma once

#include <windows.h>

#define COMMON __declspec(dllexport)

#define ARRAY_SIZE(array) ((sizeof array) / sizeof *(array))
