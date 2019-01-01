LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=uart.c  
LOCAL_MODULE := uart  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
