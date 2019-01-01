LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=spitest.c
LOCAL_MODULE := spitest  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)



include $(CLEAR_VARS)

LOCAL_SRC_FILES:=spidev_test.c
LOCAL_MODULE := spidev_test  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)