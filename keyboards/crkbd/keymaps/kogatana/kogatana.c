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
#include "kogatana.h"

#if !defined(__AVR__)
  #include <string.h>
  // #define memcpy_P(des, src, len) memcpy(des, src, len)
#endif

#define NGBUFFER 10 // キー入力バッファのサイズ

#ifdef NAGINATA_TATEGAKI
  #define NGUP X_UP
 // #define NGDN X_DOWN
 // #define NGLT X_LEFT
 // #define NGRT X_RIGHT
  #define NGKUP KC_UP
 // #define NGKDN KC_DOWN
 // #define NGKLT KC_LEFT
 // #define NGKRT KC_RIGHT*/
#endif
#ifdef NAGINATA_YOKOGAKI
  #define NGUP X_LEFT
 // #define NGDN X_RIGHT
 // #define NGLT X_UP
 // #define NGRT X_DOWN
  #define NGKUP KC_LEFT
 // #define NGKDN KC_RIGHT
 // #define NGKLT KC_UP
 // #define NGKRT KC_DOWN
#endif

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)

// 31キーを32bitの各ビットに割り当てる
#define B_Q    (1UL<<0)
#define B_W    (1UL<<1)
#define B_E    (1UL<<2)
#define B_R    (1UL<<3)
#define B_T    (1UL<<4)

#define B_Y    (1UL<<5)
#define B_U    (1UL<<6)
#define B_I    (1UL<<7)
#define B_O    (1UL<<8)
#define B_P    (1UL<<9)

#define B_A    (1UL<<10)
#define B_S    (1UL<<11)
#define B_D    (1UL<<12)
#define B_F    (1UL<<13)
#define B_G    (1UL<<14)

#define B_H    (1UL<<15)
#define B_J    (1UL<<16)
#define B_K    (1UL<<17)
#define B_L    (1UL<<18)
#define B_SCLN (1UL<<19)

#define B_Z    (1UL<<20)
#define B_X    (1UL<<21)
#define B_C    (1UL<<22)
#define B_V    (1UL<<23)
#define B_B    (1UL<<24)

#define B_N    (1UL<<25)
#define B_M    (1UL<<26)
#define B_COMM (1UL<<27)
#define B_DOT  (1UL<<28)
#define B_SLSH (1UL<<29)

#define B_SHFT (1UL<<30)

// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Qを0にしている
const uint32_t ng_key[] = {
  [NG_Q    - NG_Q] = B_Q,
  [NG_W    - NG_Q] = B_W,
  [NG_E    - NG_Q] = B_E,
  [NG_R    - NG_Q] = B_R,
  [NG_T    - NG_Q] = B_T,

  [NG_Y    - NG_Q] = B_Y,
  [NG_U    - NG_Q] = B_U,
  [NG_I    - NG_Q] = B_I,
  [NG_O    - NG_Q] = B_O,
  [NG_P    - NG_Q] = B_P,

  [NG_A    - NG_Q] = B_A,
  [NG_S    - NG_Q] = B_S,
  [NG_D    - NG_Q] = B_D,
  [NG_F    - NG_Q] = B_F,
  [NG_G    - NG_Q] = B_G,

  [NG_H    - NG_Q] = B_H,
  [NG_J    - NG_Q] = B_J,
  [NG_K    - NG_Q] = B_K,
  [NG_L    - NG_Q] = B_L,
  [NG_SCLN - NG_Q] = B_SCLN,

  [NG_Z    - NG_Q] = B_Z,
  [NG_X    - NG_Q] = B_X,
  [NG_C    - NG_Q] = B_C,
  [NG_V    - NG_Q] = B_V,
  [NG_B    - NG_Q] = B_B,

  [NG_N    - NG_Q] = B_N,
  [NG_M    - NG_Q] = B_M,
  [NG_COMM - NG_Q] = B_COMM,
  [NG_DOT  - NG_Q] = B_DOT,
  [NG_SLSH - NG_Q] = B_SLSH,

  [NG_SHFT - NG_Q] = B_SHFT,
  [NG_SHFT2 - NG_Q] = B_SHFT,
};

// カナ変換テーブル
typedef struct {
uint32_t key;
char kana[7];
} naginata_keymap;

// ロング
typedef struct {
  uint32_t key;
  char kana[22];
} naginata_keymap_long;

// UNICODE
typedef struct {
  uint32_t key;
  char kana[10];
} naginata_keymap_unicode;

// IME変換する文字列
typedef struct {
  uint32_t key;
  char kana[25];
} naginata_keymap_ime;


const PROGMEM naginata_keymap ngmap[] = {
  // 清音

  {.key = B_T                  , .kana = "e"               },
  {.key = B_Y                  , .kana = SS_TAP(X_BSPACE)  },

  {.key = B_Q                  , .kana = "h"               },
  {.key = B_W                  , .kana = "r"               },
  {.key = B_E                  , .kana = "a"               },
  {.key = B_R                  , .kana = "i"               },


  {.key = B_A                  , .kana = "t"               },
  {.key = B_S                  , .kana = "k"               },
  {.key = B_D                  , .kana = "o"               },
  {.key = B_F                  , .kana = "u"               },

  {.key = B_Z                  , .kana = "s"               },
  {.key = B_X                  , .kana = "n"               },
  {.key = B_C                  , .kana = "-"               },

  {.key = B_V                  , .kana = "y"               },
  {.key = B_SHFT                  , .kana = " "               },
  {.key = B_N                  , .kana = SS_TAP(X_ENTER)   },

  {.key = B_Q|B_W              , .kana = "p"               },
  {.key = B_A|B_S              , .kana = "m"               },
  {.key = B_Z|B_X              , .kana = "w"               },


//shft
  {.key = B_V|B_T              , .kana = "e"               },
  {.key = B_V|B_Y              , .kana = "."SS_TAP(X_ENTER)},

  {.key = B_V|B_Q              , .kana = "b"               },
  {.key = B_V|B_W              , .kana = ","               },
  {.key = B_V|B_E              , .kana = "a"               },
  {.key = B_V|B_R              , .kana = "i"               },


  {.key = B_V|B_A              , .kana = "d"               },
  {.key = B_V|B_S              , .kana = "g"               },
  {.key = B_V|B_D              , .kana = "o"               },
  {.key = B_V|B_F              , .kana = "u"               },

  {.key = B_V|B_Z              , .kana = "z"               },
  {.key = B_V|B_X              , .kana = "nn"              },
  {.key = B_V|B_C              , .kana = "x"               },

  // {.key = B_V                , .kana = ""                },
  //{.key = B_V|B_SHFT           , .kana = ""     },
  //{.key = B_V|B_N              , .kana = ""     },

  {.key = B_V|B_Q|B_W          , .kana = "f"               },
  {.key = B_V|B_Z|B_X          , .kana = "v"               },


 //shft2
  {.key = B_SHFT|B_T                  , .kana = SS_TAP(X_UP)   },
  {.key = B_SHFT|B_Y                  , .kana = "/"            },

  {.key = B_SHFT|B_Q                  , .kana = ""             },//シフトキー
  {.key = B_SHFT|B_W                  , .kana = SS_TAP(X_LEFT) },
  {.key = B_SHFT|B_E                  , .kana = SS_TAP(X_DOWN) },
  {.key = B_SHFT|B_R                  , .kana = SS_TAP(X_RIGHT)},

  // {.key = B_SHFT|B_A                  , .kana = ""               },//三点リーダ
  {.key = B_SHFT|B_S                  , .kana = "!"SS_TAP(X_ENTER)},
  {.key = B_SHFT|B_D                  , .kana = "?"SS_TAP(X_ENTER)},
  // {.key = B_SHFT|B_F                  , .kana = ""               },//ダッシュ

#ifdef NAGINATA_EDIT_WIN
  {.key = B_SHFT|B_Z                  , .kana = SS_LCTL("s")     },
  {.key = B_SHFT|B_X                  , .kana = SS_LCTL("x")     },
  // {.key = B_SHFT|B_C                  , .kana = ""               },//カッコ

  // {.key = B_SHFT|B_V                  , .kana =  SS_LCTRL("v")    },
  // {.key = B_SHFT|                  , .kana = ""               },
  // {.key = B_SHFT|B_N                  , .kana = ""               },
#endif
#ifdef NAGINATA_EDIT_MAC
  {.key = B_SHFT|B_Z                  , .kana = SS_LGUI("s")     },
  {.key = B_SHFT|B_X                  , .kana = SS_LGUI("x")     },
  // {.key = B_SHFT|B_C                  , .kana = ""               },//カッコ

  // {.key = B_SHFT|B_V                  , .kana =  SS_LGUI("v")    },
  // {.key = B_SHFT                  , .kana = ""               },
  // {.key = B_SHFT|B_N                  , .kana = ""               },
#endif


//shft3
  // {.key = B_N|B_T                  , .kana = "e"               },
  // {.key = B_N|B_Y                  , .kana = SS_TAP(X_BSPACE)  },

  // {.key = B_N|B_Q                  , .kana = "h"               },
  // {.key = B_N|B_W                  , .kana = "r"               },
  //  {.key = B_N|B_E                  , .kana = "a"               },
  // {.key = B_N|B_R                  , .kana = "i"               },


  // {.key = B_N|B_A                  , .kana = "t"               },
  // {.key = B_N|B_S                  , .kana = "k"               },
  // {.key = B_N|B_D                  , .kana = "o"               },
  // {.key = B_N|B_F                  , .kana = "u"               },

  // {.key = B_N|B_Z                  , .kana = "s"               },
  // {.key = B_N|B_X                  , .kana = "n"               },
  // {.key = B_N|B_C                  , .kana = "-"               },

  // {.key = B_N|B_V                  , .kana = "y"               },
  // {.key = B_N|B_SHFT                  , .kana = " "               },
  // {.key = B_N                  , .kana = ""   },
};

const PROGMEM naginata_keymap_long ngmapl[] = {
  {.key = B_SHFT|B_Q|B_T                  , .kana = SS_LSFT(SS_TAP(X_UP) )   },
  {.key = B_SHFT|B_Q|B_W                  , .kana = SS_LSFT(SS_TAP(X_LEFT) ) },
  {.key = B_SHFT|B_Q|B_E                  , .kana = SS_LSFT(SS_TAP(X_DOWN) ) },
  {.key = B_SHFT|B_Q|B_R                  , .kana = SS_LSFT(SS_TAP(X_RIGHT) )},
};

const PROGMEM naginata_keymap_unicode ngmapu[] = {
  // 編集モード2
#ifdef NAGINATA_EDIT_WIN
  // {.key =        , .kana = "FF5C"}, // ｜
  // {.key =        , .kana = "30FB"}, // ・
  //  {.key =       , .kana = "FF08"}, // （
  // {.key =        , .kana = "300A"}, // 《
  // {.key =        , .kana = "300C"}, // 「
  {.key = B_SHFT|B_A       , .kana = "2026 2026"}, // ……

  // {.key =        , .kana = "FF09"}, // ）
  // {.key =        , .kana = "300B"}, // 》
  // {.key =        , .kana = "300D"}, // 」
  {.key = B_SHFT|B_F       , .kana = "2500 2500"}, // ──

  // {.key =        , .kana = "3007"}, // 〇
  // {.key =        , .kana = "3010"}, // 【
  // {.key =        , .kana = "3008"}, // 〈
  // {.key =        , .kana = "300E"}, // 『

  // {.key =        , .kana = "3011"}, // 】
  //  {.key =       , .kana = "3009"}, // 〉
  //  {.key =       , .kana = "300F"}, // 』

#endif
};

const PROGMEM naginata_keymap_ime ngmapi[] = {
  // 編集モード2
#ifdef NAGINATA_EDIT_MAC
  // {.key = B_J|B_K|B_W       , .kana = "nagitatesenn"}, // ｜
  // {.key = B_J|B_K|B_T       , .kana = "nagichuutenn"}, // ・
  // {.key = B_J|B_K|B_A       , .kana = "nagikakkohi6"}, // （
  // {.key = B_J|B_K|B_S       , .kana = "nagikakkohi5"}, // 《
  // {.key = B_J|B_K|B_D       , .kana = "nagikakkohi3"}, // 「
   {.key = B_SHFT|B_A       , .kana = "nagitentenn"}, // ……

  // {.key = B_J|B_K|B_Z       , .kana = "nagikakkomi6"}, // ）
  // {.key = B_J|B_K|B_X       , .kana = "nagikakkomi5"}, // 》
  // {.key = B_J|B_K|B_C       , .kana = "nagikakkomi3"}, // 」
  {.key = B_SHFT|B_F        , .kana = "nagisensenn"}, // ──

  // {.key = B_M|B_COMM|B_T    , .kana = "nagimaru"}, // 〇
  // {.key = B_M|B_COMM|B_A    , .kana = "nagikakkohi1"}, // 【
  // {.key = B_M|B_COMM|B_S    , .kana = "nagikakkohi2"}, // 〈
  // {.key = B_M|B_COMM|B_D       , .kana = "nagikakkohi4"}, // 『

  // {.key = B_M|B_COMM|B_Z    , .kana = "nagikakkomi1"}, // 】
  // {.key = B_M|B_COMM|B_X    , .kana = "nagikakkomi2"}, // 〉
  // {.key = B_M|B_COMM|B_C    , .kana = "nagikakkomi4"}, // 』

  // {.key = B_M|B_COMM|B_E    , .kana = SS_LCMD(SS_TAP(NGUP))" "SS_LCMD(SS_TAP(X_RIGHT))},
  // {.key = B_M|B_COMM|B_T    , .kana = SS_LCMD(SS_TAP(NGUP))"   "SS_LCMD(SS_TAP(X_RIGHT))},
#endif
};
//trp3l
//s動作速度向上のために 文字テーブルの要素数を保存しておく
const uint16_t ng_elem_count  = sizeof ngmap   / sizeof ngmap[0];
const uint16_t ng_elem_countl = sizeof ngmapl  / sizeof ngmapu[0];
const uint16_t ng_elem_countu = sizeof ngmapu  / sizeof ngmapu[0];
const uint16_t ng_elem_counti = sizeof ngmapi  / sizeof ngmapi[0];



//オートリピート機能
#ifdef NG_AUTO_REPEAT

//オートリピート開始時に引っ掛かりを設定する。二連打とオートリピート発火を二回目押下後にも選択したいときはここ。
//マスター側のキーボードはなぜかスキャンが遅いので、ラグくて調整が難しい。（おま環？）
//#define FIRST_DELAY
#ifdef FIRST_DELAY
//引っ掛かりの長さ設定。ミリ秒単位。
#define f_delay_time 500
#endif
//オートリピートのトグル速度。ミリ秒。
#define auto_repeat_toggle 30
//オートリピート発火を押下時ではなく、離した時からの経過時間で判定したい場合はコメントアウトを外してください。
//#define AUTO_REPEAT_TRIGGER_IS_RELEASED
//オートリピートを開始するために必要なキーの連打速度。ミリ秒。
#define auto_repeat_tapping_term 180
//オートリピートしたいキーをここに設定する。pgupとか追加したいときはここ。
const PROGMEM uint32_t auto_repeat_keycodemap[] = {
		B_Y,
		B_SHFT|B_T,
		B_SHFT|B_W,
		B_SHFT|B_E,
		B_SHFT|B_R,
		B_SHFT|B_Q|B_T,
		B_SHFT|B_Q|B_W,
		B_SHFT|B_Q|B_E,
		B_SHFT|B_Q|B_R,
};
const uint16_t auto_repeat_elem_counts  = sizeof auto_repeat_keycodemap / sizeof auto_repeat_keycodemap[0];

static uint16_t auto_repeat_timer = 0;
static uint16_t auto_repeat_keycode_prev = 0;

bool auto_repeat_keycode(uint16_t keycode, keyrecord_t *record){

	if(record->event.pressed){
		//薙刀キー以外は判定をスキップして終了。
		switch (keycode) {
		    case NG_Q ... NG_SHFT2:
				break;
		    default:
		    	return false;
		    	break;
		}
		if(TIMER_DIFF_16(record->event.time, auto_repeat_timer) < auto_repeat_tapping_term && auto_repeat_keycode_prev == keycode){

			uint32_t keycomb_s = keycomb;
			keycomb_s |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
			uint32_t keycomb_sb = 0UL;
			//キーコードの検索。
			for(int i = 0; i < auto_repeat_elem_counts; i++){
				memcpy_P(&keycomb_sb , &auto_repeat_keycodemap[i], sizeof(keycomb_sb));
				if(keycomb_sb == keycomb_s){
					//初期遅延を発生させ、二連打とオートリピート発火を汲み分ける。
					#ifdef FIRST_DELAY
					SEND_STRING(SS_DELAY(f_delay_time));
					matrix_scan();
					if(!matrix_is_on(record->event.key.row, record->event.key.col)){
						auto_repeat_keycode_prev = keycode;
						return false;
					}
					#endif
					//キーを離すまでキーコードを発行し続ける。トグル速度を早くするとラグが出てしまう。
					while( matrix_is_on(record->event.key.row, record->event.key.col) ){
						ninputs[ng_chrcount] = keycode;
						ng_chrcount++;
						naginata_type();
						SEND_STRING(SS_DELAY(auto_repeat_toggle));
						matrix_scan();
					}
					auto_repeat_keycode_prev = keycode;
					#ifndef AUTO_REPEAT_TRIGGER_IS_RELEASED
						auto_repeat_timer = record->event.time;
					#endif
					return true;
				}
			}
			//オートリピート対象のキーコードではなかった。
			return false;

		}else{//時間切れ or 連続入力ではない。
			#ifndef AUTO_REPEAT_TRIGGER_IS_RELEASED
				auto_repeat_timer = record->event.time;
			#endif
			return false;
		}

	}else{//key release
		auto_repeat_keycode_prev = keycode;
		#ifdef AUTO_REPEAT_TRIGGER_IS_RELEASED
			auto_repeat_timer = record->event.time;
		#endif
		return false;
	}
}
#endif
void set_naginata(uint8_t ng_layer, uint16_t *onk, uint16_t *offk) {
  naginata_layer = ng_layer;

  ngon_keys[0]  = *onk;
  ngon_keys[1]  = *(onk+1);
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
  //trp3l
  //tap_code(KC_HENK);// Win
  tap_code(KC_KANA);
  //_trp3l
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

// OSのかな/英数モードをキーボードに合わせる
void makesure_mode(void) {
  if (is_naginata) {
    tap_code(KC_LANG1); // Mac
    //trp3l
//    tap_code(KC_HENK); // Win
  tap_code(KC_KANA);
  //\trp3l
  } else {
    tap_code(KC_LANG2); // Mac
    tap_code(KC_MHEN); // Win
  }
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

#ifdef MAC_LIVE_CONVERSION
static bool is_live_conv = true;
#else
static bool is_live_conv = false;
#endif

void mac_live_conversion_toggle() {
  is_live_conv = !is_live_conv;
}

void mac_live_conversion_on() {
  is_live_conv = true;
}

void mac_live_conversion_off() {
  is_live_conv = false;
}

void mac_send_string(const char *str) {
  send_string(str);
  if (!is_live_conv) tap_code(KC_SPC);
  tap_code(KC_ENT);
}

// modifierが押されたら薙刀式レイヤーをオフしてベースレイヤーに戻す
// get_mods()がうまく動かない
static int n_modifier = 0;

bool process_modifier(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_LCTRL:
    case KC_LSHIFT:
    case KC_LALT:
    case KC_LGUI:
    case KC_RCTRL:
    case KC_RSHIFT:
    case KC_RALT:
    case KC_RGUI:
    case LCTL_T(0x01) ... LCTL_T(0xFF):
    case LSFT_T(0x01) ... LSFT_T(0xFF):
    case LALT_T(0x01) ... LALT_T(0xFF):
    case LGUI_T(0x01) ... LGUI_T(0xFF):
    case RCTL_T(0x01) ... RCTL_T(0xFF):
    case RSFT_T(0x01) ... RSFT_T(0xFF):
    case RALT_T(0x01) ... RALT_T(0xFF):
    case RGUI_T(0x01) ... RGUI_T(0xFF):
      if (record->event.pressed) {
        n_modifier++;
        layer_off(naginata_layer);
      } else {
        n_modifier--;
        if (n_modifier == 0) {
          layer_on(naginata_layer);
        }
      }
      return true;
      break;
  }
  return false;
}

static uint16_t fghj_buf = 0;

// 薙刀式の起動処理(COMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {

    if (fghj_buf == 0 && (keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1]
    )) {
      fghj_buf = keycode;
      return false;
    } else if (fghj_buf > 0) {
      if ((keycode == ngon_keys[0] && fghj_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && fghj_buf == ngon_keys[0])   ) {
        naginata_on();
        fghj_buf = 0;
        return false;

      } else if ((keycode == ngoff_keys[0] && fghj_buf == ngoff_keys[1]) ||
                 (keycode == ngoff_keys[1] && fghj_buf == ngoff_keys[0])   ) {
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

  if (process_modifier(keycode, record))
    return true;

#ifdef NG_AUTO_REPEAT
  if (auto_repeat_keycode(keycode, record))
	return true;
#endif

  if (record->event.pressed) {
    switch (keycode) {
    case NG_Q ... NG_SHFT2:
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
        // バッファが一杯になったら処理を開始
        if (ng_chrcount >= NGBUFFER) {
          naginata_type();
        }
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
    case NG_Q ... NG_SHFT2:
        // どれかキーを離したら処理を開始する
        keycomb &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ
        if (ng_chrcount > 0){
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
  // bバッファの最初からnt文字目までを検索キーにする。
  // e一致する組み合わせがなければntを減らして=最後の1文字を除いて再度検索する。
  int nt = ng_chrcount;

  while (nt > 0) {
    if (naginata_lookup(nt, true)) return; // 連続シフト有効で探す
    if (naginata_lookup(nt, false)) return; // 連続シフト無効で探す
    nt--; // 最後の1キーを除いて、もう一度仮名テーブルを検索する
  }
  compress_buffer(1);
}
// bバッファの頭からnt文字の範囲を検索キーにしてテーブル検索し、文字に変換して出力する
// 検索に成功したらtrue、失敗したらfalseを返す
bool naginata_lookup(int nt, bool shifted) {
  naginata_keymap bngmap; // PROGMEM buffer
  naginata_keymap_long bngmapl; // PROGMEM buffer
  naginata_keymap_unicode bngmapu; // PROGMEM buffer
  naginata_keymap_ime bngmapi; // PROGMEM buffer

  // keycomb_bufはバッファ内のキーの組み合わせ、keycombはリリースしたキーを含んでいない
  uint32_t keycomb_buf = 0UL;

  // バッファ内のキーを組み合わせる
  for (int i = 0; i < nt; i++) {
    keycomb_buf |= ng_key[ninputs[i] - NG_Q];
  }
  // NG_SHFT2はスペースの代わりにエンターを入力する
  if (keycomb_buf == B_SHFT && ninputs[0] == NG_SHFT2) {
    tap_code(KC_ENTER);
    compress_buffer(nt);
    return true;
  }
  if (shifted) {
    // 連続shft
    if ((keycomb & B_V) == B_V) keycomb_buf |= B_V;
    // 連続shft2
    if ((keycomb & B_SHFT) == B_SHFT) keycomb_buf |= B_SHFT;
    //連続shft3
    if ((keycomb & B_N) == B_N) keycomb_buf |= B_N;
    // シフトキーを連続する
    if ((keycomb & (B_SHFT|B_Q) ) == (B_SHFT|B_Q) ) keycomb_buf |= (B_SHFT|B_Q);
  }


  switch (keycomb_buf) {
    // send_stringできないキー、長すぎるマクロはここで定義
    case B_V|B_N:
      naginata_on();
      compress_buffer(nt);
      return true;
      break;

    case B_V|B_SHFT:
	if(ninputs[1] == NG_SHFT || (ninputs[0] == NG_SHFT && !ninputs[1])){
		  naginata_off();
	}else{
	#ifdef NAGINATA_EDIT_WIN
		register_code(KC_LCTRL);
		tap_code(KC_V);
		unregister_code(KC_LCTRL);
	#endif
	#ifdef NAGINATA_EDIT_MAC
		register_code(KC_LGUIL);
		tap_code(KC_V);
		unregister_code(KC_GUIL);
	#endif
	}
	compress_buffer(nt);
	return true;
	break;

#ifdef NAGINATA_EDIT_WIN
    case B_SHFT|B_C: //カギかっこ
      send_unicode_hex_string("300C");
      send_unicode_hex_string("300D");
      tap_code(KC_ENT);
      tap_code(NGKUP);
      compress_buffer(nt);
      return true;
      break;
#endif
#ifdef NAGINATA_EDIT_MAC
    case B_SHFT|B_C: //カギかっこ
      mac_send_string("nagikakkohi3");
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      tap_code(NGKUP);
      compress_buffer(nt);
      return true;
      break;
#endif
    default:
      // キーから仮名に変換して出力する

      // 通常の仮名
      for (int i = 0; i < ng_elem_count; i++) {
    	//ng_elem_countは703行で定義。文字テーブルの要素数です。
        memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
        if (keycomb_buf == bngmap.key) {
          send_string(bngmap.kana);
          compress_buffer(nt);
          return true;
        }
      }
      // 仮名ロング
      for (int i = 0; i < ng_elem_countl; i++) {
        memcpy_P(&bngmapl, &ngmapl[i], sizeof(bngmapl));
        if (keycomb_buf == bngmapl.key) {
          send_string(bngmapl.kana);
          compress_buffer(nt);
          return true;
        }
      }
      // UNICODE文字
      for (int i = 0; i < ng_elem_countu; i++) {
        memcpy_P(&bngmapu, &ngmapu[i], sizeof(bngmapu));
        if (keycomb_buf == bngmapu.key) {
          send_unicode_hex_string(bngmapu.kana);
          if(keycomb_buf != (B_J|B_K|B_T)){
              tap_code(KC_ENT);
          }
          compress_buffer(nt);
          return true;
        }
      }
      // IME変換する文字
      for (int i = 0; i < ng_elem_counti; i++) {
        memcpy_P(&bngmapi, &ngmapi[i], sizeof(bngmapi));
        if (keycomb_buf == bngmapi.key) {
          mac_send_string(bngmapi.kana);
          compress_buffer(nt);
          return true;
        }
      }
  }
  return false;
}

