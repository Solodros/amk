
SRCS += \
	$(LPC55SDK_DIR)/$(MCU_TYPE)/clock_config.c \
	$(LPC55SDK_DIR)/$(MCU_TYPE)/peripherals.c \
	$(LPC55SDK_DIR)/$(MCU_TYPE)/pin_mux.c \

INCS += $(LPC55SDK_DIR)/$(MCU_TYPE)

APP_DEFS += \
-DCPU_LPC5516JBD64 \
-DCPU_LPC5516JBD64_cm33 \
-D__USE_CMSIS \
-D__NEWLIB__ \

SDK_DEFS += -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb

MCU_LD := LPC5516