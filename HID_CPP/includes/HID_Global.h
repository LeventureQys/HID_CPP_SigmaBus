#pragma once

#ifdef HID_CPP_EXPORTS
#define HID_API __declspec(dllexport)
#else
#define HID_API __declspec(dllimport)
#endif