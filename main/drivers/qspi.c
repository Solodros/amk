/**
 * @file qspi.c
 * @author astro
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#if defined(HC32F460)
#   include "qspi_hc32.c"
#elif defined(STM32F412Rx) || defined(STM32L476xx)
#   include "qspi_stm32.c"
#elif defined(STM32U575xx)
#   include "ospi_stm32.c"
#elif defined(AT32F405)
#   include "qspi_at32.c"
#else
#   error "qspi not available"
#endif