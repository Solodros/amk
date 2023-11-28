
MCU = STM32F405
TINYUSB_ENABLE = yes
USE_F405_APM = yes

RGB_LINEAR_ENABLE = yes
RGB_MATRIX_ENABLE = yes
SIGNALRGB_ENABLE = yes
EECONFIG_FLASH = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
AMK_CUSTOM_MATRIX = ms 
DEBOUNCE_TYPE = none

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

SRCS += $(KEYBOARD_DIR)/ms_rabbit.c
SRCS += $(KEYBOARD_DIR)/apc_matrix.c

#NO_DEBUG := 1