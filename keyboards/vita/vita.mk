

MCU = STM32F722
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
MSC_ENABLE = yes
EECONFIG_FLASH = yes
#RGB_LINEAR_ENABLE = yes
#DATETIME_ENABLE = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
#RTOS_ENABLE = yes
DYNAMIC_CONFIGURATION = yes

LINKER_PATH = $(KEYBOARD_DIR)

#SRCS += $(KEYBOARD_DIR)/vita.c
SRCS += $(KEYBOARD_DIR)/display_vita.c
SRCS += $(MAIN_DIR)/drivers/st7735.c
