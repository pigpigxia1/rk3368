LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=gpio.c
LOCAL_MODULE := gpio  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
