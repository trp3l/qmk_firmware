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
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)



#define B_A    (1UL<<1)
#define B_S    (1UL<<2)
#define B_D    (1UL<<3)
#define B_F    (1UL<<4)
#define B_G    (1UL<<5)

#define B_H    (1UL<<6)
#define B_J    (1UL<<7)

#define B_SHFT (1UL<<11)

// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Aを0にしている
const uint32_t ng_key[] = {

  [NG_A    - NG_A] = B_A,
  [NG_S    - NG_A] = B_S,
  [NG_D    - NG_A] = B_D,
  [NG_F    - NG_A] = B_F,
  [NG_G    - NG_A] = B_G,

  [NG_H    - NG_A] = B_H,
  [NG_J    - NG_A] = B_J,

  [NG_SHFT - NG_A] = B_SHFT,
};

// カナ変換テーブル
typedef struct {
  uint32_t key;
  char kana[7];
} naginata_keymap;

const PROGMEM naginata_keymap ngmap[] = {

  {.key = B_A                      , .kana = "ro"      }, // ろ
  {.key = B_S                      , .kana = "ke"      }, // ひ
  {.key = B_D                      , .kana = "to"      }, // と
  {.key = B_F                      , .kana = "ka"      }, // か
  {.key = B_G                      , .kana = "xtu"     }, // っ
  {.key = B_H                      , .kana = "ku"      }, // く
  {.key = B_J                      , .kana = "a"       }, // あ

  {.key = B_SHFT|B_A               , .kana = "se"      }, // せ
  {.key = B_SHFT|B_S               , .kana = "nu"      }, // ぬ
  {.key = B_SHFT|B_D               , .kana = "ni"      }, // に
  {.key = B_SHFT|B_F               , .kana = "ma"      }, // ま
  {.key = B_SHFT|B_G               , .kana = "ti"      }, // ち
  {.key = B_SHFT|B_H               , .kana = "ya"      }, // や
  {.key = B_SHFT|B_J               , .kana = "no"      }, // の

  {.key = B_J|B_A                  , .kana = "ze"      }, // ぜ
  {.key = B_J|B_S                  , .kana = "ge"      }, // げ
  {.key = B_J|B_D                  , .kana = "do"      }, // ど
  {.key = B_J|B_F                  , .kana = "ga"      }, // が
  {.key = B_J|B_G                  , .kana = "di"      }, // ぢ
  {.key = B_F|B_H                  , .kana = "gu"      }, // ぐ

  {.key = B_SHFT                   , .kana = " "},
};


// 薙刀式のレイヤー、オンオフするキー
void set_naginata(uint8_t layer, uint16_t *onk, uint16_t *offk) {
  naginata_layer = layer;
  ngon_keys[0] = *onk;
  ngon_keys[1] = *(onk+1);
  ngoff_keys[0] = *offk;
  ngoff_keys[1] = *(offk+1);
}

// 薙刀式をオン
void naginata_on(void) {
  is_naginata = true;
  keycomb = 0UL;
  naginata_clear();
  layer_on(naginata_layer);

  tap_code(KC_LANG1); // Mac
  tap_code(KC_HENK); // Win
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  keycomb = 0UL;
  naginata_clear();
  layer_off(naginata_layer);

  tap_code(KC_LANG2); // Mac
  tap_code(KC_MHEN); // Win
}

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

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

static uint16_t fghj_buf = 0;

// 薙刀式の起動処理(COMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {

    if (fghj_buf == 0 && (keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1])) {
      fghj_buf = keycode;
      return false;
    } else if (fghj_buf > 0) {
      if ((keycode == ngon_keys[0] && fghj_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && fghj_buf == ngon_keys[0])) {
        naginata_on();
        fghj_buf = 0;
        return false;
      } else if ((keycode == ngoff_keys[0] && fghj_buf == ngoff_keys[1]) ||
                 (keycode == ngoff_keys[1] && fghj_buf == ngoff_keys[0])) {
        naginata_off();
        fghj_buf = 0;
        return false;
      } else {
        tap_code(fghj_buf);
        fghj_buf = 0;
        return true;
      }
    }
  } else {
    if (fghj_buf > 0) {
      tap_code(fghj_buf);
      fghj_buf = 0;
      return true;
    }
  }
  fghj_buf = 0;
  return true;
}


// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {
  if (!is_naginata)
    // return true;
    return enable_naginata(keycode, record);

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
      keycomb_buf |= ng_key[ninputs[i] - NG_A];
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
