#pragma once

#define PAL_USE_CALLBACKS            TRUE
#define PAL_USE_WAIT                 TRUE
/* OLED */
#ifdef OLED_ENABLE
#define HAL_USE_I2C TRUE
#endif
#include_next "halconf.h"