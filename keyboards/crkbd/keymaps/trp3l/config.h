/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


//#define USE_MATRIX_I2C

/* Select hand configuration */

//#define MASTER_LEFT
 #define MASTER_RIGHT
// #define EE_HANDS

//#define SSD1306OLED

#define USE_SERIAL_PD2

//#define TAPPING_FORCE_HOLD
//#define TAPPING_TERM 300
#define PERMISSIVE_HOLD
/*
#undef RGBLED_NUM
#define RGBLIGHT_ANIMATIONS
#define RGBLED_NUM 27
#define RGBLIGHT_LIMIT_VAL 120
#define RGBLIGHT_HUE_STEP 10
#define RGBLIGHT_SAT_STEP 17
#define RGBLIGHT_VAL_STEP 17
*/
//--------------------------------------------------------------------------------
#define KC_KPS KC_CAPSLOCK
//--------------------------------------------------------------------------------
#define NAGINATA_TATEGAKI
// #define NAGINATA_YOKOGAKI
#define NAGINATA_EDIT_WIN // JP106
// #define NAGINATA_EDIT_MAC // JP106
// #define MAC_LIVE_CONVERSION // Macでライブ変換をオンにしている場合
#define NAGINATA_KOUCHI_SHIFT
#define SAMEHAND_SHFT
//#define NG_AUTO_REPEAT
//--------------------------------------------------------------------------------
//#define COMBO_COUNT 2
//#define COMBO_TERM 100
#ifdef DELAY_STRINGS
	#define TAP_CODE_DELAY 1000
#endif
