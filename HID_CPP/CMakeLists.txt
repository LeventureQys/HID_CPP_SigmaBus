cmake_minimum_required(VERSION 3.0.0)
project(HID_CPP VERSION 0.1.0 LANGUAGES C CXX)

 set(Headers 

 ./includes/CRC.h
 ./includes/HID_DeviceManager.h
  ./includes/IHID.h
 ./includes/HID_Device.h
  ./includes/HID_Params.h
  ./includes/HID_DeviceManager.h
  ./includes/HID_Global.h
  ./hidapi/hidapi.h
)

 set(SRCS
 ./src/CRC.cpp
 ./src/HID_Device.cpp
 ./src/HID_DeviceManager.cpp
 ./hidapi/hidapi_windows.c)

add_library(HID_CPP SHARED ${Headers} ${SRCS})
target_compile_definitions(HID_CPP PRIVATE -DHID_CPP_EXPORTS)
target_include_directories(HID_CPP PUBLIC ./includes ./hidapi)
target_link_libraries(HID_CPP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/setupapi.lib)
message(STATUS "HID_CPP lbrary created" ${CMAKE_CURRENT_SOURCE_DIR}/lib/setupapi.lib)