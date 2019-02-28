LOCAL_PATH:= $(call my-dir)

#$(info my-dir=  $(call my-dir) )
#include $(all-subdir-makefiles)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=\
	card.c \
	uart.c \
	exception.c \
	des/des.c  \
	src/FM175xx/fm175xx.c \
	src/IOS14443/mifare_card.c \
	src/IOS14443/type_a.c \
	src/IOS14443/type_b.c \
	src/IOS14443/cpu_card.c \
	src/Utility/Utility.c   \
	src/Hid_Card/hid_card.c \
	main.c
	
	

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/des
	


LOCAL_MODULE := iccard  

LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)