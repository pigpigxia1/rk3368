LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=light.c
LOCAL_MODULE := light  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
