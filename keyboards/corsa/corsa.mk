
MCU = STM32F411
TINYUSB_ENABLE = yes
TINYUSB_USE_HAL = yes
EECONFIG_FLASH = yes
MSC_ENABLE = yes
DYNAMIC_CONFIGURATION = yes
#RGB_LINEAR_ENABLE = yes
RGB_MATRIX_ENABLE = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
AMK_CUSTOM_MATRIX = yes
DEBOUNCE_TYPE = asym_eager_defer_pk #sym_defer_pk


UF2_ENABLE = yes
UF2_FAMILY = STM32F4

LINKER_PATH = $(KEYBOARD_DIR)

SRCS += $(KEYBOARD_DIR)/corsa.c
SRCS += $(KEYBOARD_DIR)/cm2.c

SRCS += $(KEYBOARD_DIR)/display.c
SRCS += $(MAIN_DIR)/drivers/st7735.c

#RTOS_ENABLE = yes
#ASMFLAGS += -DSYSTEM_CLOCK=96000000