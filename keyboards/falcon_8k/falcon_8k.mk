
MCU = STM32F405
USE_F405_APM = yes

TINYUSB_ENABLE = yes
EECONFIG_FLASH = yes
RGB_LINEAR_ENABLE = yes
VIAL_ENABLE = yes
NKRO_ENABLE = yes
PORT_SCAN_ENABLE = yes
STATE_SCAN_ENABLE = yes
DEBOUNCE_TYPE = none

LINKER_PATH = $(KEYBOARD_DIR)

UF2_ENABLE = yes
UF2_FAMILY = STM32F4

SRCS += $(KEYBOARD_DIR)/falcon_8k.c
