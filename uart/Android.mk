LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=uart.c \
					rili/rili.c
LOCAL_MODULE := uart  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)

LOCAL_SRC_FILES:=main.c 
			
LOCAL_MODULE := main  
LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)

