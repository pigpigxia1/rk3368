LOCAL_PATH:= $(call my-dir)

#$(info my-dir=  $(call my-dir) )
#include $(all-subdir-makefiles)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:=\
	card.c \
	des/des.c  \
	src/FM175xx/fm175xx.c \
	src/IOS14443/mifare_card.c \
	src/IOS14443/type_a.c \
	src/IOS14443/type_b.c \
	src/IOS14443/cpu_card.c \
	src/Utility/Utility.c   \
	main.c
	
	

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/des
	


LOCAL_MODULE := card  

LOCAL_MODULE_TAGS := optional  
include $(BUILD_EXECUTABLE)