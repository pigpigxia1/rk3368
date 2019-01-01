LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=key.c
LOCAL_MODULE := key  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
