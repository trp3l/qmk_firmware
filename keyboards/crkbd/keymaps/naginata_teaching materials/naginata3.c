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

#if !defined(__AVR__)
  #include <string.h>
  // #define memcpy_P(des, src, len) memcpy(des, src, len)
#endif

#define NGBUFFER 10 // キー入力バッファのサイズ

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。

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

// カナ変換テーブル
typedef struct {
  uint32_t key;
  char kana[7];
} naginata_keymap;

const PROGMEM naginata_keymap ngmap[] = {

  {.key = B_A                      , .kana = "ro"      }, // ろ
  {.key = B_S                      , .kana = "hi"      }, // ひ
  {.key = B_D                      , .kana = "to"      }, // と
  {.key = B_SHFT|B_A               , .kana = "se"      }, // せ
  {.key = B_SHFT|B_S               , .kana = "nu"      }, // ぬ
  {.key = B_SHFT|B_D               , .kana = "ni"      }, // に
  {.key = B_SHFT                   , .kana = " "},
};

// バッファをクリアする
void naginata_clear(void) {
  for (int i = 0; i < NGBUFFER; i++) {
    ninputs[i] = 0;
  }
  ng_chrcount = 0;
}

// バッファから先頭n文字を削除する
void compress_buffer(int n) {
  if (ng_chrcount == 0) return;
  for (int j = 0; j < NGBUFFER; j++) {
    if (j + n < NGBUFFER) {
      ninputs[j] = ninputs[j + n];
    } else {
      ninputs[j] = 0;
    }
  }
  ng_chrcount -= n;
}


// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
    switch (keycode) {
      case NG_A ... NG_SHFT2:
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_A]; // キーの重ね合わせ
        // バッファが一杯になったら処理を開始
        if (ng_chrcount >= NGBUFFER) {
          naginata_type();
        }
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
      case NG_A ... NG_SHFT2:
        // どれかキーを離したら処理を開始する
        keycomb &= ~ng_key[keycode - NG_A]; // キーの重ね合わせ
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
    int nt = ng_chrcount;

    while (nt > 0) {
      if (naginata_lookup(nt, false)) return;
      nt--; // 最後の1キーを除いて、もう一度仮名テーブルを検索する
    }
    compress_buffer(1);
  }
}
bool naginata_lookup(int nt, bool shifted){

    naginata_keymap bngmap; // PROGMEM buffer

    // keycomb_bufはバッファ内のキーの組み合わせ、keycombはリリースしたキーを含んでいない
    uint32_t keycomb_buf = 0UL;

    // バッファ内のキーを組み合わせる
    for (int i = 0; i < nt; i++) {
      keycomb_buf |= ng_key[ninputs[i] - NG_Q];
    }

    switch (keycomb_buf) {
      default:
        // キーから仮名に変換して出力する
        for (int i = 0; i < sizeof ngmap / sizeof bngmap; i++) {
          memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
          if (keycomb_buf == bngmap.key) {
            send_string(bngmap.kana);
            compress_buffer(nt);
            return true;
          }
        }
    }
    return false;
}
