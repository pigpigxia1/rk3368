LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)


LOCAL_SRC_FILES:=eeprom.c
LOCAL_MODULE := eeprom  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)
