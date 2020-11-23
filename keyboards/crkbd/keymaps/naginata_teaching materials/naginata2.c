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

#define NGBUFFER 3 // キー入力バッファのサイズ

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ


#define B_A    (1UL<<1)
#define B_S    (1UL<<2)
#define B_D    (1UL<<3)

#define B_SHFT (1UL<<11)

// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Aを0にしている
const uint32_t ng_key[] = {

  [NG_A    - NG_A] = B_A,
  [NG_S    - NG_A] = B_S,
  [NG_D    - NG_A] = B_D,

  [NG_SHFT - NG_A] = B_SHFT,
};


// バッファをクリアする
void naginata_clear(void) {
  for (int i = 0; i < NGBUFFER; i++) {
    ninputs[i] = 0;
  }
  ng_chrcount = 0;
}


// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
    switch (keycode) {
      case NG_A ... NG_SHFT:
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        // バッファが一杯になったら処理を開始
        if (ng_chrcount >= NGBUFFER) {
          naginata_type();
        }
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
      case NG_A ... NG_SHFT:
        // どれかキーを離したら処理を開始する
        if (ng_chrcount > 0) {
          naginata_type();
        }
        return false;
        break;
    }
  }
  return true;
}



// キー入力を文字に変換して出力する
void naginata_type(void) {
  // バッファの最初からnt文字目までを検索キーにする。
  // 一致する組み合わせがなければntを減らして=最後の1文字を除いて再度検索する。
	 for (int i = 0; i < nt; i++) {
	    keycomb_buf |= ng_key[ninputs[i] - NG_A];
	  }

	 switch (keycomb_buf) {

		case B_A:
			send_strings("ro");
			naginata_clear()
			return true;
			break;

		case B_S:
			send_strings("ki");
			naginata_clear()
			return true;
			break;

		case B_D:
			send_strings("to");
			naginata_clear()
			return true;
			break;

		case B_SHFT:
			send_strings(" ");
			naginata_clear()
			return true;
			break;
			//シフト文字
		case B_SHFT|B_A:
			send_strings("se");
			naginata_clear()
			return true;
			break;

		case B_SHFT|B_S:
			send_strings("mu");
			naginata_clear()
			return true;
			break;

		case B_SHFT|B_D:
			send_strings("ni");
			naginata_clear()
			return true;
			break;
	}
	return false;
}
