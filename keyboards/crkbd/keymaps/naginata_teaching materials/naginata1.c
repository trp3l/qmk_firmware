/* Copyright 2018-2019 eswai <@eswai>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "naginata.h"

// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    switch (keycode) {
    case NG_A:
    	send_strings("ro");
        return false;
        break;
    case NG_S:
    	send_strings("ki");
        return false;
        break;
    case NG_D:
    	send_strings("to");
        return false;
        break;
    case NG_F:
    	send_strings("ka");
        return false;
        break;
    case NG_G:
    	send_strings("xtu");
        return false;
        break;
    case NG_SHFT:
    	send_strings(" ");
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
    case NG_A:
    case NG_S:
    case NG_D:
    case NG_F:
    case NG_G:
    case NG_SHFT:
        return false;
        break;
    }
  }
  return true;
}

