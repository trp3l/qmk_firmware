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
#include "ng_ex_tools.h"
#include "keymap_japanese.h"

#if !defined(__AVR__)
  #include <string.h>
  // #define memcpy_P(des, src, len) memcpy(des, src, len)
#endif

#define NGBUFFER 10 // キー入力バッファのサイズ

#ifdef NAGINATA_TATEGAKI
  #define NGUP X_UP
  #define NGDN X_DOWN
  #define NGLT X_LEFT
  #define NGRT X_RIGHT
  #define NGKUP KC_UP
  #define NGKDN KC_DOWN
  #define NGKLT KC_LEFT
  #define NGKRT KC_RIGHT
#endif
#ifdef NAGINATA_YOKOGAKI
  #define NGUP X_LEFT
  #define NGDN X_RIGHT
  #define NGLT X_UP
  #define NGRT X_DOWN
  #define NGKUP KC_LEFT
  #define NGKDN KC_RIGHT
  #define NGKLT KC_UP
  #define NGKRT KC_DOWN
#endif

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)
//trp3l
//s_cutレイヤに関するコードを追加
static uint8_t s_cut_layer = 0;
static uint16_t s_on_keys[2];
static uint16_t s_off_keys[2];
//trp3l

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
//trp3l ng_ex_tools
#ifdef SAMEHAND_SHFT
uint16_t get_ninputs_i(uint8_t ng_chr){
	if(ng_chr <= ng_chrcount){
		return ninputs[ng_chr];
	}else{
		return 0;
	}
}
#endif
#ifdef NG_AUTO_REPEAT
uint32_t get_ng_key(uint16_t keycode){
	if(SAFE_RANGE <= keycode && keycode < NG_SAFE_RANGE){
		return ng_key[keycode - NG_Q];
	}else{
		return  0;
	}
}
uint32_t get_keycomb(void){
	return keycomb;
}
void add_chrcount(void){
	if(ng_chrcount+1 <= NGBUFFER)
		ng_chrcount++;
}
void set_ninputs_i(uint16_t keycode){
	if(SAFE_RANGE <= keycode && keycode < NG_SAFE_RANGE)
		ninputs[ng_chrcount] = keycode;
}
#endif
//_trp3l

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
/*typedef struct {
  uint32_t key;
  char kana[10];
} naginata_keymap_unicode;
*/
// IME変換する文字列
typedef struct {
  uint32_t key;
  char kana[25];
} naginata_keymap_ime;



//remapez カナのリマップがより簡単にできるようになりました。
//「き」と「は」を入れ替えたい場合はKANA_KIとKANA_HAが対象です。
#define KOGAKI   B_Q
#define KANA_KI  B_W
#define KANA_TE  B_E
#define KANA_SHI B_R
//T left
#define KANA_RO  B_A
#define KANA_KE  B_S
#define KANA_TO  B_D
#define KANA_KA  B_F
//G 撥音
#define KANA_HO  B_Z //sシフトの方も変更すること。
#define KANA_HI  B_X //sシフトの方も変更すること。
#define KANA_HA  B_C
#define KANA_KO  B_V
#define KANA_SO  B_B

//Y RIGHT
//U BS
#define KANA_RU  B_I
#define KANA_SU  B_O
#define KANA_RE  B_P

#define KANA_KU  B_H
#define KANA_A   B_J
#define KANA_I   B_K
#define KANA_U   B_L
//SCLON -
#define KANA_TA  B_N
#define KANA_NA  B_M
//COMMA ん
#define KANA_RA  B_DOT
#define KANA_HE  B_SLSH //sシフトの方も変更すること。


//Sシフト位置の清音
#define KANA_NU  (B_SHFT|B_W)
#define KANA_RI  (B_SHFT|B_E)
#define KANA_ME  (B_SHFT|B_R)

#define KANA_SE  (B_SHFT|B_A)
#define KANA_MU  (B_SHFT|B_S)
#define KANA_NI  (B_SHFT|B_D)
#define KANA_MA  (B_SHFT|B_F)
#define KANA_CHI (B_SHFT|B_G)

//Z,X,SLASH はシフト位置が未定義なので、例外として kana の後ろの"ho"とかを変更してください。
#define UNDEFINED_SHIFT_Z {.key = B_Z|B_SHFT , .kana = "ho"}
#define UNDEFINED_SHIFT_X {.key = B_X|B_SHFT , .kana = "hi"}
#define KANA_WO  (B_SHFT|B_C)
//V 読点
#define KANA_MI  (B_SHFT|B_B)

//RIGHT HAND
#define KANA_SA  (B_SHFT|B_U)
#define KANA_YO  (B_SHFT|B_I)
#define KANA_E   (B_SHFT|B_O)
#define KANA_TSU (B_SHFT|B_P)

#define KANA_YA  (B_SHFT|B_H)
#define KANA_NO  (B_SHFT|B_J)
#define KANA_MO  (B_SHFT|B_K)
#define KANA_WA  (B_SHFT|B_L)
#define KANA_YU  (B_SHFT|B_SCLN)

#define KANA_O   (B_SHFT|B_N)
//M 句点
#define KANA_NE  (B_SHFT|B_COMM)
#define KANA_HU  (B_SHFT|B_DOT)
#define UNDEFINED_SHIFT_SLSH {.key = B_SLSH|B_SHFT , .kana = "he"}

//D濁点 半濁点
#define DAK_L B_F
#define DAK_R B_J
#define HAN_L B_V
#define HAN_R B_M


//D濁点の左右判定
#define LEFT_KEY (B_Q|B_W|B_E|B_R|B_T|\
				  B_A|B_S|B_D|B_F|B_G|\
				  B_Z|B_X|B_C|B_V|B_B)

#define DAK(seion) ((seion & LEFT_KEY) <= 0 ? ((DAK_L|seion)& ~B_SHFT) : ((DAK_R|seion)& ~B_SHFT))
#define HAN(seion) ((seion & LEFT_KEY) <= 0 ? ((HAN_L|seion)& ~B_SHFT) : ((HAN_R|seion)& ~B_SHFT))

const PROGMEM naginata_keymap ngmap[] = {
  // 清音
  {.key = KANA_A                  , .kana = "a"       }, // あ
  {.key = KANA_I                  , .kana = "i"       }, // い
  {.key = KANA_U                  , .kana = "u"       }, // う
  {.key = KANA_E                  , .kana = "e"       }, // え
  {.key = KANA_O                  , .kana = "o"       }, // お
  {.key = KANA_KA                 , .kana = "ka"      }, // か
  {.key = KANA_KI                 , .kana = "ki"      }, // き
  {.key = KANA_KU                 , .kana = "ku"      }, // く
  {.key = KANA_KE                 , .kana = "ke"      }, // け
  {.key = KANA_KO                 , .kana = "ko"      }, // こ
  {.key = KANA_SA                 , .kana = "sa"      }, // さ
  {.key = KANA_SHI                , .kana = "si"      }, // し
  {.key = KANA_SU                 , .kana = "su"      }, // す
  {.key = KANA_SE                 , .kana = "se"      }, // せ
  {.key = KANA_SO                 , .kana = "so"      }, // そ
  {.key = KANA_TA                 , .kana = "ta"      }, // た
  {.key = KANA_CHI                , .kana = "ti"      }, // ち
  {.key = KANA_TSU                , .kana = "tu"      }, // つ

  {.key = KANA_TE                 , .kana = "te"      }, // て
  {.key = KANA_TO                 , .kana = "to"      }, // と
  {.key = KANA_NA                 , .kana = "na"      }, // な
  {.key = KANA_NI                 , .kana = "ni"      }, // に
  {.key = KANA_NU                 , .kana = "nu"      }, // ぬ
  {.key = KANA_NE                 , .kana = "ne"      }, // ね
  {.key = KANA_NO                 , .kana = "no"      }, // の
  {.key = KANA_HA                 , .kana = "ha"      }, // は
  {.key = KANA_HI                 , .kana = "hi"      }, // ひ
  {.key = KANA_HU                 , .kana = "hu"      }, // ふ
  {.key = KANA_HE                 , .kana = "he"      }, // へ
  {.key = KANA_HO                 , .kana = "ho"      }, // ほ
  {.key = KANA_MA                 , .kana = "ma"      }, // ま
  {.key = KANA_MI                 , .kana = "mi"      }, // み
  {.key = KANA_MU                 , .kana = "mu"      }, // む
  {.key = KANA_ME                 , .kana = "me"      }, // め
  {.key = KANA_MO                 , .kana = "mo"      }, // も
  {.key = KANA_YA                 , .kana = "ya"      }, // や
  {.key = KANA_YU                 , .kana = "yu"      }, // ゆ
  {.key = KANA_YO                 , .kana = "yo"      }, // よ
  {.key = KANA_RA                 , .kana = "ra"      }, // ら
  {.key = KANA_RI                 , .kana = "ri"      }, // り
  {.key = KANA_RU                 , .kana = "ru"      }, // る
  {.key = KANA_RE                 , .kana = "re"      }, // れ

  {.key = KANA_RE                 , .kana = "re"      }, // れ
  {.key = KANA_RO                 , .kana = "ro"      }, // ろ
  {.key = KANA_WA                 , .kana = "wa"      }, // わ
  {.key = KANA_WO                 , .kana = "wo"      }, // を
  {.key = B_COMM                  , .kana = "nn"      }, // ん
  {.key = B_SCLN                  , .kana = "-"       }, // ー
  UNDEFINED_SHIFT_Z,//シフト清音が未定義な位置の補完
  UNDEFINED_SHIFT_X,
  UNDEFINED_SHIFT_SLSH,


  // 濁音
  {.key = DAK(KANA_KA)                 , .kana = "ga"      }, // が
  //{.key = DAK(KANA_KA)|B_SHFT          , .kana = "ga"      }, // が(冗長)
  {.key = DAK(KANA_KI)                 , .kana = "gi"      }, // ぎ
  //{.key = DAK(KANA_KI)|B_SHFT          , .kana = "gi"      }, // ぎ(冗長)
  {.key = DAK(KANA_KU)                 , .kana = "gu"      }, // ぐ
  //{.key = DAK(KANA_KU)|B_SHFT          , .kana = "gu"      }, // ぐ(冗長)
  {.key = DAK(KANA_KE)                 , .kana = "ge"      }, // げ
  //{.key = DAK(KANA_KE)|B_SHFT          , .kana = "ge"      }, // げ(冗長)
  {.key = DAK(KANA_KO)                 , .kana = "go"      }, // ご
  //{.key = DAK(KANA_KO)|B_SHFT          , .kana = "go"      }, // ご(冗長)
  {.key = DAK(KANA_SA)                 , .kana = "za"      }, // ざ
  //{.key = DAK(KANA_SA)|B_SHFT          , .kana = "za"      }, // ざ(冗長)
  {.key = DAK(KANA_SHI)                , .kana = "zi"      }, // じ
  //{.key = DAK(KANA_SHI)|B_SHFT         , .kana = "zi"      }, // じ(冗長)
  {.key = DAK(KANA_SU)                 , .kana = "zu"      }, // ず
  //{.key = DAK(KANA_SU)|B_SHFT          , .kana = "zu"      }, // ず(冗長)
  {.key = DAK(KANA_SE)                 , .kana = "ze"      }, // ぜ
  //{.key = DAK(KANA_SE)|B_SHFT          , .kana = "ze"      }, // ぜ(冗長)
  {.key = DAK(KANA_SO)                 , .kana = "zo"      }, // ぞ
  //{.key = DAK(KANA_SO)|B_SHFT          , .kana = "zo"      }, // ぞ(冗長)
  {.key = DAK(KANA_TA)                 , .kana = "da"      }, // だ
  //{.key = DAK(KANA_TA)|B_SHFT          , .kana = "da"      }, // だ(冗長)
  {.key = DAK(KANA_CHI)                , .kana = "di"      }, // ぢ
  //{.key = DAK(KANA_CHI)|B_SHFT         , .kana = "di"      }, // ぢ(冗長)
  {.key = DAK(KANA_TSU)                , .kana = "du"      }, // づ
  //{.key = DAK(KANA_TSU)|B_SHFT         , .kana = "du"      }, // づ(冗長)
  {.key = DAK(KANA_TE)                 , .kana = "de"      }, // で
  //{.key = DAK(KANA_TE)|B_SHFT          , .kana = "de"      }, // で(冗長)
  {.key = DAK(KANA_TO)                 , .kana = "do"      }, // ど
  //{.key = DAK(KANA_TO)|B_SHFT          , .kana = "do"      }, // ど(冗長)
  {.key = DAK(KANA_HA)                 , .kana = "ba"      }, // ば
  //{.key = DAK(KANA_HA)|B_SHFT          , .kana = "ba"      }, // ば(冗長)
  {.key = DAK(KANA_HI)                 , .kana = "bi"      }, // び
  //{.key = DAK(KANA_HI)|B_SHFT          , .kana = "bi"      }, // び(冗長)
  {.key = DAK(KANA_HU)                 , .kana = "bu"      }, // ぶ
  //{.key = DAK(KANA_HU)|B_SHFT          , .kana = "bu"      }, // ぶ(冗長)
  {.key = DAK(KANA_HE)                 , .kana = "be"      }, // べ
  //{.key = DAK(KANA_HE)|B_SHFT          , .kana = "be"      }, // べ(冗長)

  {.key = DAK(KANA_HO)                 , .kana = "bo"      }, // ぼ
  //{.key = DAK(KANA_HO)|B_SHFT          , .kana = "bo"      }, // ぼ(冗長)
  {.key = DAK(KANA_U)                  , .kana = "vu"      }, // ゔ
  //{.key = DAK(KANA_U)|B_SHFT           , .kana = "vu"      }, // ゔ(冗長)

  // 半濁音
  {.key = HAN(KANA_HA)                 , .kana = "pa"      }, // ぱ
  //{.key = HAN(KANA_HA)|B_SHFT          , .kana = "pa"      }, // ぱ(冗長)
  {.key = HAN(KANA_HI)                 , .kana = "pi"      }, // ぴ
  //{.key = HAN(KANA_HI)|B_SHFT          , .kana = "pi"      }, // ぴ(冗長)
  {.key = HAN(KANA_HU)                 , .kana = "pu"      }, // ぷ
  //{.key = HAN(KANA_HU)|B_SHFT          , .kana = "pu"      }, // ぷ(冗長)
  {.key = HAN(KANA_HE)                 , .kana = "pe"      }, // ぺ
  //{.key = HAN(KANA_HE)|B_SHFT          , .kana = "pe"      }, // ぺ(冗長)
  {.key = HAN(KANA_HO)                 , .kana = "po"      }, // ぽ
  //{.key = HAN(KANA_HO)|B_SHFT          , .kana = "po"      }, // ぽ(冗長)

  // 小書き
  {.key = (KOGAKI|KANA_YA)& ~B_SHFT           , .kana = "xya"     }, // ゃ
  {.key =  KOGAKI|KANA_YA|   B_SHFT           , .kana = "xya"     }, // ゃ
  {.key = (KOGAKI|KANA_YU)& ~B_SHFT           , .kana = "xyu"     }, // ゅ
  {.key =  KOGAKI|KANA_YU|   B_SHFT           , .kana = "xyu"     }, // ゅ
  {.key = (KOGAKI|KANA_YO)& ~B_SHFT           , .kana = "xyo"     }, // ょ
  {.key =  KOGAKI|KANA_YO|   B_SHFT           , .kana = "xyo"     }, // ょ
  {.key = (KOGAKI|KANA_A)& ~B_SHFT            , .kana = "xa"      }, // ぁ
  {.key =  KOGAKI|KANA_A|   B_SHFT            , .kana = "xa"      }, // ぁ
  {.key = (KOGAKI|KANA_I)& ~B_SHFT            , .kana = "xi"      }, // ぃ
  {.key =  KOGAKI|KANA_I|   B_SHFT            , .kana = "xi"      }, // ぃ
  {.key = (KOGAKI|KANA_U)& ~B_SHFT            , .kana = "xu"      }, // ぅ
  // {.key = KOGAKI|KANA_U|   B_SHFT            , .kana = "xu"      }, // ぅ
  {.key = (KOGAKI|KANA_E)& ~B_SHFT            , .kana = "xe"      }, // ぇ
  {.key =  KOGAKI|KANA_E|   B_SHFT            , .kana = "xe"      }, // ぇ
  {.key = (KOGAKI|KANA_O)& ~B_SHFT            , .kana = "xo"      }, // ぉ
  {.key =  KOGAKI|KANA_O|   B_SHFT            , .kana = "xo"      }, // ぉ
  // {.key = (KOGAKI|KANA_WA)& ~B_SHFT           , .kana = "xwa"     }, // ゎ
  {.key =  KOGAKI|KANA_WA|  B_SHFT            , .kana = "xwa"     }, // ゎ
  {.key = B_G                             , .kana = "xtu"     }, // っ

  // 清音拗音 濁音拗音 半濁拗音
  {.key = (KANA_SHI|KANA_YA)& ~B_SHFT            , .kana = "sya"     }, // しゃ
  //{.key =  KANA_SHI|KANA_YA|   B_SHFT            , .kana = "sya"     }, // しゃ(冗長)
  {.key = (KANA_SHI|KANA_YU)& ~B_SHFT            , .kana = "syu"     }, // しゅ
  //{.key =  KANA_SHI|KANA_YU|   B_SHFT            , .kana = "syu"     }, // しゅ(冗長)
  {.key = (KANA_SHI|KANA_YO)& ~B_SHFT            , .kana = "syo"     }, // しょ
  //{.key =  KANA_SHI|KANA_YO|   B_SHFT            , .kana = "syo"     }, // しょ(冗長)
  {.key = (DAK(KANA_SHI)|KANA_YA)& ~B_SHFT       , .kana = "zya"     }, // じゃ
  //{.key =  DAK(KANA_SHI)|KANA_YA|   B_SHFT       , .kana = "zya"     }, // じゃ(冗長)
  {.key = (DAK(KANA_SHI)|KANA_YU)& ~B_SHFT       , .kana = "zyu"     }, // じゅ
  //{.key =  DAK(KANA_SHI)|KANA_YU|   B_SHFT       , .kana = "zyu"     }, // じゅ(冗長)
  {.key = (DAK(KANA_SHI)|KANA_YO)& ~B_SHFT       , .kana = "zyo"     }, // じょ
  //{.key =  DAK(KANA_SHI)|KANA_YO|   B_SHFT       , .kana = "zyo"     }, // じょ(冗長)
  {.key = (KANA_KI|KANA_YA)& ~B_SHFT             , .kana = "kya"     }, // きゃ
  //{.key =  KANA_KI|KANA_YA|   B_SHFT             , .kana = "kya"     }, // きゃ(冗長)
  {.key = (KANA_KI|KANA_YU)& ~B_SHFT             , .kana = "kyu"     }, // きゅ
  //{.key =  KANA_KI|KANA_YU|   B_SHFT             , .kana = "kyu"     }, // きゅ(冗長)
  {.key = (KANA_KI|KANA_YO)& ~B_SHFT             , .kana = "kyo"     }, // きょ
  //{.key =  KANA_KI|KANA_YO|   B_SHFT             , .kana = "kyo"     }, // きょ(冗長)
  {.key = (DAK(KANA_KI)|KANA_YA)& ~B_SHFT        , .kana = "gya"     }, // ぎゃ
  //{.key =  DAK(KANA_KI)|KANA_YA|   B_SHFT        , .kana = "gya"     }, // ぎゃ(冗長)
  {.key = (DAK(KANA_KI)|KANA_YU)& ~B_SHFT        , .kana = "gyu"     }, // ぎゅ
  //{.key =  DAK(KANA_KI)|KANA_YU|   B_SHFT        , .kana = "gyu"     }, // ぎゅ(冗長)
  {.key = (DAK(KANA_KI)|KANA_YO)& ~B_SHFT        , .kana = "gyo"     }, // ぎょ
  //{.key =  DAK(KANA_KI)|KANA_YO|   B_SHFT        , .kana = "gyo"     }, // ぎょ(冗長)
  {.key = (KANA_CHI|KANA_YA)& ~B_SHFT            , .kana = "tya"     }, // ちゃ
  //{.key =  KANA_CHI|KANA_YA|   B_SHFT            , .kana = "tya"     }, // ちゃ(冗長)
  {.key = (KANA_CHI|KANA_YU)& ~B_SHFT            , .kana = "tyu"     }, // ちゅ
  //{.key =  KANA_CHI|KANA_YU|   B_SHFT            , .kana = "tyu"     }, // ちゅ(冗長)
  {.key = (KANA_CHI|KANA_YO) & ~B_SHFT           , .kana = "tyo"     }, // ちょ
  //{.key =  KANA_CHI|KANA_YO|   B_SHFT            , .kana = "tyo"     }, // ちょ(冗長)
  {.key = (DAK(KANA_CHI)|KANA_YA)& ~B_SHFT       , .kana = "dya"     }, // ぢゃ
  //{.key =  DAK(KANA_CHI)|KANA_YA|   B_SHFT       , .kana = "dya"     }, // ぢゃ(冗長)
  {.key = (DAK(KANA_CHI)|KANA_YU)& ~B_SHFT       , .kana = "dyu"     }, // ぢゅ
  //{.key =  DAK(KANA_CHI)|KANA_YU|   B_SHFT       , .kana = "dyu"     }, // ぢゅ(冗長)
  {.key = (DAK(KANA_CHI)|KANA_YO)& ~B_SHFT       , .kana = "dyo"     }, // ぢょ
  //{.key =  DAK(KANA_CHI)|KANA_YO|   B_SHFT       , .kana = "dyo"     }, // ぢょ(冗長)
  {.key = (KANA_NI|KANA_YA)& ~B_SHFT             , .kana = "nya"     }, // にゃ
  //{.key =  KANA_NI|KANA_YA|   B_SHFT             , .kana = "nya"     }, // にゃ(冗長)
  {.key = (KANA_NI|KANA_YU)& ~B_SHFT             , .kana = "nyu"     }, // にゅ
  //{.key =  KANA_NI|KANA_YU|   B_SHFT             , .kana = "nyu"     }, // にゅ(冗長)
  {.key = (KANA_NI|KANA_YO)& ~B_SHFT             , .kana = "nyo"     }, // にょ
  //{.key =  KANA_NI|KANA_YO|   B_SHFT             , .kana = "nyo"     }, // にょ(冗長)
  {.key = (KANA_HI|KANA_YA)& ~B_SHFT             , .kana = "hya"     }, // ひゃ
  //{.key =  KANA_HI|KANA_YA|   B_SHFT             , .kana = "hya"     }, // ひゃ(冗長)
  {.key = (KANA_HI|KANA_YU)& ~B_SHFT             , .kana = "hyu"     }, // ひゅ
  //{.key =  KANA_HI|KANA_YU|   B_SHFT             , .kana = "hyu"     }, // ひゅ(冗長)
  {.key = (KANA_HI|KANA_YO)& ~B_SHFT             , .kana = "hyo"     }, // ひょ
  //{.key =  KANA_HI|KANA_YO|   B_SHFT             , .kana = "hyo"     }, // ひょ(冗長)
  {.key = (DAK(KANA_HI)|KANA_YA)& ~B_SHFT        , .kana = "bya"     }, // びゃ
  //{.key =  DAK(KANA_HI)|KANA_YA|   B_SHFT        , .kana = "bya"     }, // びゃ(冗長)
  {.key = (DAK(KANA_HI)|KANA_YU)& ~B_SHFT        , .kana = "byu"     }, // びゅ
  //{.key =  DAK(KANA_HI)|KANA_YU|   B_SHFT        , .kana = "byu"     }, // びゅ(冗長)
  {.key = (DAK(KANA_HI)|KANA_YO)& ~B_SHFT        , .kana = "byo"     }, // びょ
  //{.key =  DAK(KANA_HI)|KANA_YO|   B_SHFT        , .kana = "byo"     }, // びょ(冗長)
  {.key = (HAN(KANA_HI)|KANA_YA)& ~B_SHFT        , .kana = "pya"     }, // ぴゃ
  //{.key =  HAN(KANA_HI)|KANA_YA|   B_SHFT        , .kana = "pya"     }, // ぴゃ(冗長)
  {.key = (HAN(KANA_HI)|KANA_YU)& ~B_SHFT        , .kana = "pyu"     }, // ぴゅ
  //{.key =  HAN(KANA_HI)|KANA_YU|   B_SHFT        , .kana = "pyu"     }, // ぴゅ(冗長)
  {.key = (HAN(KANA_HI)|KANA_YO)& ~B_SHFT        , .kana = "pyo"     }, // ぴょ
  //{.key =  HAN(KANA_HI)|KANA_YO|   B_SHFT        , .kana = "pyo"     }, // ぴょ(冗長)
  {.key = (KANA_MI|KANA_YA)& ~B_SHFT             , .kana = "mya"     }, // みゃ
  //{.key =  KANA_MI|KANA_YA|   B_SHFT             , .kana = "mya"     }, // みゃ(冗長)
  {.key = (KANA_MI|KANA_YU)& ~B_SHFT             , .kana = "myu"     }, // みゅ
  //{.key =  KANA_MI|KANA_YU|   B_SHFT             , .kana = "myu"     }, // みゅ(冗長)
  {.key = (KANA_MI|KANA_YO)& ~B_SHFT             , .kana = "myo"     }, // みょ
  //{.key =  KANA_MI|KANA_YO|   B_SHFT             , .kana = "myo"     }, // みょ(冗長)
  {.key = (KANA_RI|KANA_YA)& ~B_SHFT             , .kana = "rya"     }, // りゃ
  //{.key =  KANA_RI|KANA_YA|   B_SHFT             , .kana = "rya"     }, // りゃ(冗長)
  {.key = (KANA_RI|KANA_YU)& ~B_SHFT             , .kana = "ryu"     }, // りゅ
  //{.key =  KANA_RI|KANA_YU|   B_SHFT             , .kana = "ryu"     }, // りゅ(冗長)
  {.key = (KANA_RI|KANA_YO)& ~B_SHFT             , .kana = "ryo"     }, // りょ
  //{.key =  KANA_RI|KANA_YO|   B_SHFT             , .kana = "ryo"     }, // りょ(冗長)

  // 清音外来音 濁音外来音
  {.key = (HAN(KANA_TE)|KANA_I)& ~B_SHFT         , .kana = "thi"     }, // てぃ
  //{.key =  HAN(KANA_TE)|KANA_I|   B_SHFT         , .kana = "thi"     }, // てぃ(冗長)
  {.key = (HAN(KANA_TE)|KANA_YU)& ~B_SHFT        , .kana = "thu"     }, // てゅ
  //{.key =  HAN(KANA_TE)|KANA_YU|   B_SHFT        , .kana = "thu"     }, // てゅ(冗長)
  {.key = (DAK(KANA_TE)|KANA_I)& ~B_SHFT         , .kana = "dhi"     }, // でぃ
  //{.key =  DAK(KANA_TE)|KANA_I|   B_SHFT         , .kana = "dhi"     }, // でぃ(冗長)
  {.key = (DAK(KANA_TE)|KANA_YU)& ~B_SHFT        , .kana = "dhu"     }, // でゅ
  //{.key =  DAK(KANA_TE)|KANA_YU|   B_SHFT        , .kana = "dhu"     }, // でゅ(冗長)
  {.key = (HAN(KANA_TO)|KANA_U)& ~B_SHFT         , .kana = "toxu"    }, // とぅ
  //{.key =  HAN(KANA_TO)|KANA_U|   B_SHFT         , .kana = "toxu"    }, // とぅ(冗長)
  {.key = (DAK(KANA_TO)|KANA_U)& ~B_SHFT         , .kana = "doxu"    }, // どぅ
  //{.key =  DAK(KANA_TO)|KANA_U|   B_SHFT         , .kana = "doxu"    }, // どぅ(冗長)
  {.key = (HAN(KANA_SHI)|KANA_E)& ~B_SHFT        , .kana = "sye"     }, // しぇ
  //{.key =  HAN(KANA_SHI)|KANA_E|   B_SHFT        , .kana = "sye"     }, // しぇ(冗長)
  {.key = (DAK(KANA_SHI)|KANA_E)& ~B_SHFT        , .kana = "zye"     }, // じぇ
  //{.key =  DAK(KANA_SHI)|KANA_E|   B_SHFT        , .kana = "zye"     }, // じぇ(冗長)
  {.key = (HAN(KANA_CHI)|KANA_E)& ~B_SHFT        , .kana = "tye"     }, // ちぇ
  //{.key =  HAN(KANA_CHI)|KANA_E|   B_SHFT        , .kana = "tye"     }, // ちぇ(冗長)
  {.key = (DAK(KANA_CHI)|KANA_E)& ~B_SHFT        , .kana = "dye"     }, // ぢぇ
  //{.key =  DAK(KANA_CHI)|KANA_E|   B_SHFT        , .kana = "dye"     }, // ぢぇ(冗長)
  {.key = (HAN(KANA_HU)|KANA_A)& ~B_SHFT         , .kana = "fa"      }, // ふぁ
  //{.key =  HAN(KANA_HU)|KANA_A|   B_SHFT         , .kana = "fa"      }, // ふぁ(冗長)
  {.key = (HAN(KANA_HU)|KANA_I)& ~B_SHFT         , .kana = "fi"      }, // ふぃ
  //{.key =  HAN(KANA_HU)|KANA_I|   B_SHFT         , .kana = "fi"      }, // ふぃ(冗長)
  {.key = (HAN(KANA_HU)|KANA_E)& ~B_SHFT         , .kana = "fe"      }, // ふぇ
  //{.key =  HAN(KANA_HU)|KANA_E|   B_SHFT         , .kana = "fe"      }, // ふぇ(冗長)
  {.key = (HAN(KANA_HU)|KANA_O)& ~B_SHFT         , .kana = "fo"      }, // ふぉ
  //{.key =  HAN(KANA_HU)|KANA_O|   B_SHFT         , .kana = "fo"      }, // ふぉ(冗長)
  {.key = (HAN(KANA_HU)|KANA_YU)& ~B_SHFT        , .kana = "fyu"     }, // ふゅ
  //{.key =  HAN(KANA_HU)|KANA_YU|   B_SHFT        , .kana = "fyu"     }, // ふゅ(冗長)
  {.key = (HAN(KANA_U)|KANA_I)& ~B_SHFT          , .kana = "wi"      }, // うぃ
  //{.key =  HAN(KANA_U)|KANA_I|   B_SHFT          , .kana = "wi"      }, // うぃ(冗長)
  {.key = (HAN(KANA_U)|KANA_E)& ~B_SHFT          , .kana = "we"      }, // うぇ
  //{.key =  HAN(KANA_U)|KANA_E|   B_SHFT          , .kana = "we"      }, // うぇ(冗長)
  {.key = (HAN(KANA_U)|KANA_O)& ~B_SHFT          , .kana = "uxo"     }, // うぉ
  //{.key =  HAN(KANA_U)|KANA_O|   B_SHFT          , .kana = "uxo"     }, // うぉ(冗長)
  {.key = (DAK(KANA_U)|KANA_A)& ~B_SHFT          , .kana = "va"      }, // ゔぁ
  //{.key =  DAK(KANA_U)|KANA_A|   B_SHFT          , .kana = "va"      }, // ゔぁ(冗長)
  {.key = (DAK(KANA_U)|KANA_I)& ~B_SHFT          , .kana = "vi"      }, // ゔぃ
  //{.key =  DAK(KANA_U)|KANA_I|   B_SHFT          , .kana = "vi"      }, // ゔぃ(冗長)
  {.key = (DAK(KANA_U)|KANA_E)& ~B_SHFT          , .kana = "ve"      }, // ゔぇ
  //{.key =  DAK(KANA_U)|KANA_E|   B_SHFT          , .kana = "ve"      }, // ゔぇ(冗長)
  {.key = (DAK(KANA_U)|KANA_O)& ~B_SHFT          , .kana = "vo"      }, // ゔぉ
  //{.key =  DAK(KANA_U)|KANA_O|   B_SHFT          , .kana = "vo"      }, // ゔぉ(冗長)
  {.key = (DAK(KANA_U)|KANA_YU)& ~B_SHFT         , .kana = "vuxyu"   }, // ゔゅ
  //{.key =  DAK(KANA_U)|KANA_YU|   B_SHFT         , .kana = "vuxyu"   }, // ゔゅ(冗長)
  {.key = (HAN(KANA_KU)|KANA_A)& ~B_SHFT         , .kana = "kuxa"    }, // くぁ
  //{.key =  HAN(KANA_KU)|KANA_A|   B_SHFT         , .kana = "kuxa"    }, // くぁ(冗長)
  {.key = (HAN(KANA_KU)|KANA_I)& ~B_SHFT         , .kana = "kuxi"    }, // くぃ
  //{.key =  HAN(KANA_KU)|KANA_I|   B_SHFT         , .kana = "kuxi"    }, // くぃ(冗長)
  {.key = (HAN(KANA_KU)|KANA_E)& ~B_SHFT         , .kana = "kuxe"    }, // くぇ
  //{.key =  HAN(KANA_KU)|KANA_E|   B_SHFT         , .kana = "kuxe"    }, // くぇ(冗長)
  {.key = (HAN(KANA_KU)|KANA_O)& ~B_SHFT         , .kana = "kuxo"    }, // くぉ
  //{.key =  HAN(KANA_KU)|KANA_O|   B_SHFT         , .kana = "kuxo"    }, // くぉ(冗長)
  {.key = (HAN(KANA_KU)|KANA_WA)& ~B_SHFT        , .kana = "kuxwa"   }, // くゎ
  //{.key =  HAN(KANA_KU)|KANA_WA|   B_SHFT        , .kana = "kuxwa"   }, // くゎ(冗長)
  {.key = (DAK(KANA_KU)|KANA_A)& ~B_SHFT         , .kana = "guxa"    }, // ぐぁ
  //{.key =  DAK(KANA_KU)|KANA_A|   B_SHFT         , .kana = "guxa"    }, // ぐぁ(冗長)
  {.key = (DAK(KANA_KU)|KANA_I)& ~B_SHFT         , .kana = "guxi"    }, // ぐぃ
  //{.key =  DAK(KANA_KU)|KANA_I|   B_SHFT         , .kana = "guxi"    }, // ぐぃ(冗長)
  {.key = (DAK(KANA_KU)|KANA_E)& ~B_SHFT         , .kana = "guxe"    }, // ぐぇ
  //{.key =  DAK(KANA_KU)|KANA_E|   B_SHFT         , .kana = "guxe"    }, // ぐぇ(冗長)
  {.key = (DAK(KANA_KU)|KANA_O)& ~B_SHFT         , .kana = "guxo"    }, // ぐぉ
  //{.key =  DAK(KANA_KU)|KANA_O|   B_SHFT         , .kana = "guxo"    }, // ぐぉ(冗長)
  {.key = (DAK(KANA_KU)|KANA_WA)& ~B_SHFT        , .kana = "guxwa"   }, // ぐゎ
  //{.key =  DAK(KANA_KU)|KANA_WA|   B_SHFT        , .kana = "guxwa"   }, // ぐゎ(冗長)
  {.key = (HAN(KANA_TSU)|KANA_A)& ~B_SHFT        , .kana = "tsa"     }, // つぁ
  //{.key =  HAN(KANA_TSU)|KANA_A|   B_SHFT        , .kana = "tsa"     }, // つぁ(冗長)
  {.key = (HAN(KANA_TSU)|KANA_I)& ~B_SHFT        , .kana = "tsi"     }, // つぃ
  //{.key =  HAN(KANA_TSU)|KANA_I|   B_SHFT        , .kana = "tsi"     }, // つぃ(冗長)
  {.key = (HAN(KANA_TSU)|KANA_E)& ~B_SHFT        , .kana = "tse"     }, // つぇ
  //{.key =  HAN(KANA_TSU)|KANA_E|   B_SHFT        , .kana = "tse"     }, // つぇ(冗長)
  {.key = (HAN(KANA_TSU)|KANA_O)& ~B_SHFT        , .kana = "tso"     }, // つぉ
  //{.key =  HAN(KANA_TSU)|KANA_O|   B_SHFT        , .kana = "tso"     }, // つぉ(冗長)
  //remapez
  // 追加
  {.key = B_SHFT            , .kana = " "},
  {.key = KOGAKI             , .kana = ""},
  {.key = B_V|B_SHFT        , .kana = ","},
  {.key = B_M|B_SHFT        , .kana = "."SS_TAP(X_ENTER)},
  {.key = B_U               , .kana = SS_TAP(X_BSPC)},
  {.key = B_T               , .kana = SS_TAP(NGRT)},
  {.key = B_Y               , .kana = SS_TAP(NGLT)},

  // enter
  {.key = B_V|B_M           , .kana = SS_TAP(X_ENTER)},
  // enter+シフト(連続シフト)
  {.key = B_SHFT|B_V|B_M    , .kana = SS_TAP(X_ENTER)},

  // 編集モード1
#ifdef NAGINATA_EDIT_WIN
  // {.key = B_J|B_K|B_Q       , .kana = ""},
  // {.key = B_J|B_K|B_W       , .kana = ""},
  {.key = B_J|B_K|B_E       , .kana = "dexi"},
  //{.key = B_J|B_K|B_R       , .kana = SS_LCTRL("s")},
  // {.key = B_J|B_K|B_T       , .kana = ""},

  {.key = B_D|B_F|B_Y       , .kana = SS_TAP(X_HOME)},
  // {.key = B_D|B_F|B_U       , .kana = ""},
  {.key = B_D|B_F|B_I       , .kana = SS_TAP(X_INT4)}, // 再変換
  //  {.key = B_D|B_F|B_O       , .kana = SS_TAP(X_DELETE)},  //trp3lコメントアウトに書き換えた。
  // {.key = B_D|B_F|B_P       , .kana = ""},

  // {.key = B_J|B_K|B_A       , .kana = ""},
  // {.key = B_J|B_K|B_S       , .kana = ""},
  //trp3l
  {.key = B_J|B_K|B_D       , .kana = "]"SS_TAP(X_ENTER)},//ホントは[
  //_trp3l
  {.key = B_J|B_K|B_F       , .kana = "?"SS_TAP(X_ENTER)},
  // {.key = B_J|B_K|B_G       , .kana = ""},
  // {.key = B_J|B_K|B_H       , .kana = ""},

  {.key = B_D|B_F|B_J       , .kana = SS_TAP(NGUP)},
  // {.key = B_D|B_F|B_K       , .kana = ""},
  // {.key = B_D|B_F|B_L       , .kana = ""},
  {.key = B_D|B_F|B_SCLN    , .kana = SS_LCTL("i")}, // カタカナ変換

  // {.key = B_J|B_K|B_Z       , .kana = ""},
  // {.key = B_J|B_K|B_X       , .kana = ""},
  //trp3l
  {.key = B_J|B_K|B_C       , .kana = SS_TAP(X_NONUS_HASH)SS_TAP(X_ENTER)},//ホントは]
  //_trp3l
  {.key = B_J|B_K|B_V       , .kana = "!"SS_TAP(X_ENTER)},
  // {.key = B_J|B_K|B_B       , .kana = ""},

  {.key = B_D|B_F|B_N       , .kana = SS_TAP(X_END)},
  {.key = B_D|B_F|B_M       , .kana = SS_TAP(NGDN)},
  // {.key = B_D|B_F|B_COMM       , .kana = ""},
  // {.key = B_D|B_F|B_DOT       , .kana = ""},
  {.key = B_D|B_F|B_SLSH    , .kana = SS_LGUI("u")}, // ひらがな変換
#endif
#ifdef NAGINATA_EDIT_MAC
  // {.key = B_J|B_K|B_Q       , .kana = ""},
  // {.key = B_J|B_K|B_W       , .kana = ""},
  {.key = B_J|B_K|B_E       , .kana = "dexi"},
  {.key = B_J|B_K|B_R       , .kana = SS_LGUI("s")},
  // {.key = B_J|B_K|B_T       , .kana = ""},

  // {.key = B_D|B_F|B_Y       , .kana = SS_TAP(X_HOME)},
  // {.key = B_D|B_F|B_U       , .kana = ""},
  {.key = B_D|B_F|B_I       , .kana = SS_TAP(X_LNG1)SS_TAP(X_LNG1)},
  {.key = B_D|B_F|B_O       , .kana = SS_TAP(X_DELETE)},
  // {.key = B_D|B_F|B_P       , .kana = ""},

  // {.key = B_J|B_K|B_A       , .kana = ""},
  // {.key = B_J|B_K|B_S       , .kana = ""},
  // {.key = B_J|B_K|B_D       , .kana = ""},
  {.key = B_J|B_K|B_F       , .kana = "?"SS_TAP(X_ENTER)},
  // {.key = B_J|B_K|B_G       , .kana = ""},
  // {.key = B_J|B_K|B_H       , .kana = ""},

  {.key = B_D|B_F|B_J       , .kana = SS_TAP(NGUP)},
  // {.key = B_D|B_F|B_K       , .kana = ""},
  // {.key = B_D|B_F|B_L       , .kana = ""},
  {.key = B_D|B_F|B_SCLN    , .kana = SS_LCTL("k")}, // カタカナ変換

  // {.key = B_J|B_K|B_Z       , .kana = ""},
  // {.key = B_J|B_K|B_X       , .kana = ""},
  //{.key = B_J|B_K|B_C       , .kana = ""},
  //_trp3l
  {.key = B_J|B_K|B_V       , .kana = "!"SS_TAP(X_ENTER)},
  // {.key = B_J|B_K|B_B       , .kana = ""},

  // {.key = B_D|B_F|B_N       , .kana = SS_TAP(X_END)},
  {.key = B_D|B_F|B_M       , .kana = SS_TAP(NGDN)},
  // {.key = B_D|B_F|B_COMM       , .kana = ""},
  // {.key = B_D|B_F|B_DOT       , .kana = ""},
  {.key = B_D|B_F|B_SLSH    , .kana = SS_LGUI("j")}, // ひらがな変換
#endif
  // 編集モード2
  {.key = B_M|B_COMM|B_G   , .kana = "   "},
#ifdef NAGINATA_EDIT_WIN
  //{.key = B_C|B_V|B_H      , .kana = SS_LCTRL("x")},
  //{.key = B_C|B_V|B_M      , .kana = SS_LCTRL("c")},
  //{.key = B_C|B_V|B_COMM   , .kana = SS_LCTRL("v")},
  //{.key = B_C|B_V|B_DOT    , .kana = SS_LCTRL("y")},
  //{.key = B_C|B_V|B_SLSH   , .kana = SS_LCTRL("z")},
#endif
#ifdef NAGINATA_EDIT_MAC
  {.key = B_C|B_V|B_H      , .kana = SS_LGUI("x")},
  {.key = B_C|B_V|B_M      , .kana = SS_LGUI("c")},
  {.key = B_C|B_V|B_COMM   , .kana = SS_LGUI("v")},
  {.key = B_C|B_V|B_DOT    , .kana = SS_LGUI("y")},
  {.key = B_C|B_V|B_SLSH   , .kana = SS_LGUI("z")},
#endif
};

const PROGMEM naginata_keymap_long ngmapl[] = {
  {.key = B_T|B_SHFT        , .kana = SS_LSFT(SS_TAP(NGRT))},
  {.key = B_Y|B_SHFT        , .kana = SS_LSFT(SS_TAP(NGLT))},
  {.key = B_SHFT|B_T        , .kana = SS_LSFT(SS_TAP(NGRT))},
  {.key = B_SHFT|B_Y        , .kana = SS_LSFT(SS_TAP(NGLT))},

  // 編集モード1
#ifdef NAGINATA_EDIT_WIN
  {.key = B_J|B_K|B_Q       , .kana = SS_LCTL(SS_TAP(X_END))},
  // {.key = B_J|B_K|B_W       , .kana = ""},
  // {.key = B_J|B_K|B_E       , .kana = ""},
  // {.key = B_J|B_K|B_R       , .kana = ""},
  // {.key = B_J|B_K|B_T       , .kana = ""},

  // {.key = B_D|B_F|B_Y       , .kana = ""},
  {.key = B_D|B_F|B_U       , .kana = SS_LSFT(SS_TAP(X_END))SS_TAP(X_BSPC)},
  // {.key = B_D|B_F|B_I       , .kana = ""},
//trp3l
  {.key = B_D|B_F|B_O       , .kana = "//"SS_TAP(X_F10) SS_TAP(X_ENTER)}, //DELキーから変更。
//trp3l"
  //{.key = B_D|B_F|B_P       , .kana = SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)},

  // {.key = B_J|B_K|B_A       , .kana = ""},
  // {.key = B_J|B_K|B_S       , .kana = ""},
  // {.key = B_J|B_K|B_D       , .kana = ""},
  // {.key = B_J|B_K|B_F       , .kana = ""},
  //trp3l
  {.key = B_J|B_K|B_G       , .kana = "stld"SS_TAP(X_SPACE)SS_TAP(X_ENTER)},
  //_trp3l
  {.key = B_D|B_F|B_H       , .kana = SS_TAP(X_ENTER)SS_TAP(X_END)},
  // {.key = B_D|B_F|B_J       , .kana = ""},
  {.key = B_D|B_F|B_K       , .kana = SS_LSFT(SS_TAP(NGUP))},
  {.key = B_D|B_F|B_L       , .kana = SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)},
  // {.key = B_D|B_F|B_SCLN    , .kana = ""},

  // {.key = B_J|B_K|B_Z       , .kana = ""},
  // {.key = B_J|B_K|B_X       , .kana = ""},
  // {.key = B_J|B_K|B_C       , .kana = ""},
  // {.key = B_J|B_K|B_V       , .kana = ""},
  // {.key = B_J|B_K|B_B       , .kana = ""},

  // {.key = B_D|B_F|B_N       , .kana = ""},
  // {.key = B_D|B_F|B_M       , .kana = ""},
  {.key = B_D|B_F|B_COMM    , .kana = SS_LSFT(SS_TAP(NGDN))},
  {.key = B_D|B_F|B_DOT     , .kana = SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)},
  // {.key = B_D|B_F|B_SLSH    , .kana = ""},
#endif
#ifdef NAGINATA_EDIT_MAC
  {.key = B_J|B_K|B_Q       , .kana = SS_LCMD(SS_TAP(NGRT))},
  // {.key = B_J|B_K|B_W       , .kana = ""},
  // {.key = B_J|B_K|B_E       , .kana = ""},
  // {.key = B_J|B_K|B_R       , .kana = ""},
  // {.key = B_J|B_K|B_T       , .kana = ""},

  {.key = B_D|B_F|B_Y       , .kana = SS_LCMD(SS_TAP(NGUP))}, // 行頭
  {.key = B_D|B_F|B_U       , .kana = SS_LSFT(SS_LCMD(SS_TAP(NGDN)))SS_LGUI("x")},
  // {.key = B_D|B_F|B_I       , .kana = ""},
  // {.key = B_D|B_F|B_O       , .kana = ""},
  {.key = B_D|B_F|B_P       , .kana = SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)},

  // {.key = B_J|B_K|B_A       , .kana = ""},
  // {.key = B_J|B_K|B_S       , .kana = ""},
  // {.key = B_J|B_K|B_D       , .kana = ""}, // wheel up
  // {.key = B_J|B_K|B_F       , .kana = ""}, // wheel down
  // {.key = B_J|B_K|B_G       , .kana = ""},

  {.key = B_D|B_F|B_H       , .kana = SS_TAP(X_ENTER)SS_LCMD(SS_TAP(NGDN))},
  // {.key = B_D|B_F|B_J       , .kana = ""},
  {.key = B_D|B_F|B_K       , .kana = SS_LSFT(SS_TAP(NGUP))},
  {.key = B_D|B_F|B_L       , .kana = SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)},
  // {.key = B_D|B_F|B_SCLN    , .kana = ""},

  // {.key = B_J|B_K|B_Z       , .kana = ""},
  // {.key = B_J|B_K|B_X       , .kana = ""},
  // {.key = B_J|B_K|B_C       , .kana = ""},
  // {.key = B_J|B_K|B_V       , .kana = ""},
  // {.key = B_J|B_K|B_B       , .kana = ""},

  {.key = B_D|B_F|B_N       , .kana = SS_LCMD(SS_TAP(NGDN))}, // 行末
  // {.key = B_D|B_F|B_M       , .kana = ""},
  {.key = B_D|B_F|B_COMM    , .kana = SS_LSFT(SS_TAP(NGDN))},
  {.key = B_D|B_F|B_DOT     , .kana = SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)},
  // {.key = B_D|B_F|B_SLSH    , .kana = ""},
#endif

// 編集モード2
#ifdef NAGINATA_EDIT_WIN
  // {.key = B_M|B_COMM|B_Q       , .kana = ""},
  // {.key = B_M|B_COMM|B_W       , .kana = ""},
  {.key = B_M|B_COMM|B_E       , .kana = SS_TAP(X_HOME)SS_TAP(X_ENTER)" "SS_TAP(X_END)},
  {.key = B_M|B_COMM|B_R       , .kana =SS_TAP(X_HOME)SS_TAP(X_ENTER)"   "SS_TAP(X_END)},
  // {.key = B_M|B_COMM|B_T       , .kana = ""},

  {.key = B_C|B_V|B_Y      , .kana = SS_LSFT(SS_TAP(X_HOME))},
  {.key = B_C|B_V|B_U       , .kana = SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)},
  {.key = B_C|B_V|B_I       , .kana = SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)},
  {.key = B_C|B_V|B_O       , .kana = SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)},
  {.key = B_C|B_V|B_P       , .kana = SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)},
  // {.key = B_M|B_COMM|B_A       , .kana = ""},
  // {.key = B_M|B_COMM|B_S       , .kana = ""},
  // {.key = B_M|B_COMM|B_D       , .kana = ""},
  // {.key = B_M|B_COMM|B_F       , .kana = ""},
  // {.key = B_M|B_COMM|B_G       , .kana = ""},

  // {.key = B_C|B_V|B_H       , .kana = ""},
  {.key = B_C|B_V|B_J       , .kana = SS_DOWN(X_LSFT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_UP(X_LSFT)},
  {.key = B_C|B_V|B_K       , .kana = SS_DOWN(X_LSFT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_UP(X_LSFT)},
  {.key = B_C|B_V|B_L       , .kana = SS_TAP(X_PGDN)},
  {.key = B_C|B_V|B_SCLN    , .kana = SS_TAP(X_PGUP)},

  {.key = B_C|B_V|B_N      , .kana = SS_LSFT(SS_TAP(X_END))},
#endif
#ifdef NAGINATA_EDIT_MAC
  // {.key = B_M|B_COMM|B_Q       , .kana = ""},
  // {.key = B_M|B_COMM|B_W       , .kana = ""},
  // {.key = B_M|B_COMM|B_E       , .kana = ""},
  {.key = B_M|B_COMM|B_R       , .kana = SS_LCMD(SS_TAP(NGUP))SS_TAP(X_ENTER)" "SS_LCMD(SS_TAP(NGDN))},
  // {.key = B_M|B_COMM|B_T       , .kana = ""},

  {.key = B_C|B_V|B_Y       , .kana = SS_DOWN(X_LSFT)SS_LCMD(SS_TAP(NGUP))SS_UP(X_LSFT)},
  {.key = B_C|B_V|B_U       , .kana = SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)},
  {.key = B_C|B_V|B_I       , .kana = SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)},
  {.key = B_C|B_V|B_O       , .kana = SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)SS_TAP(X_PGDN)},
  {.key = B_C|B_V|B_P       , .kana = SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)},
  // {.key = B_M|B_COMM|B_A       , .kana = ""},
  // {.key = B_M|B_COMM|B_S       , .kana = ""},
  // {.key = B_M|B_COMM|B_D       , .kana = ""},
  // {.key = B_M|B_COMM|B_F       , .kana = ""},
  // {.key = B_M|B_COMM|B_G       , .kana = ""},

  // {.key = B_C|B_V|B_H       , .kana = ""},
   {.key = B_C|B_V|B_J       , .kana = SS_DOWN(X_LSFT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_UP(X_LSFT)},
   {.key = B_C|B_V|B_K       , .kana = SS_DOWN(X_LSFT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_UP(X_LSFT)},
  {.key = B_C|B_V|B_L       , .kana = SS_TAP(X_PGDOWN)},
  {.key = B_C|B_V|B_SCLN    , .kana = SS_TAP(X_PGUP)},

  {.key = B_C|B_V|B_Y       , .kana = SS_DOWN(X_LSFT)SS_LCMD(SS_TAP(NGDN))SS_UP(X_LSFT)},
#endif


//trp3l 固有名詞モード
   {.key = B_S|B_D|B_Y    , .kana = SS_TAP(X_END) SS_LSFT(SS_TAP(X_UP) SS_TAP(X_END))},
   {.key = B_S|B_D|B_H    , .kana = SS_TAP(X_END) SS_LSFT(SS_TAP(X_HOME))},
 //_trp3l

};
/*
const PROGMEM naginata_keymap_unicode ngmapu[] = {
  // 編集モード2
#ifdef NAGINATA_EDIT_WIN
  {.key = B_J|B_K|B_W       , .kana = "FF5C"}, // ｜
  {.key = B_J|B_K|B_T       , .kana = "30FB"}, // ・
  {.key = B_J|B_K|B_A    , .kana = "FF08"}, // （
  {.key = B_J|B_K|B_S       , .kana = "300A"}, // 《
  {.key = B_J|B_K|B_D       , .kana = "300C"}, // 「
  {.key = B_J|B_K|B_G    , .kana = "2026 2026"}, // ……

  {.key = B_J|B_K|B_Z    , .kana = "FF09"}, // ）
  {.key = B_J|B_K|B_X     , .kana = "300B"}, // 》
  {.key = B_J|B_K|B_C       , .kana = "300D"}, // 」
  {.key = B_J|B_K|B_B    , .kana = "2500 2500"}, // ──

  {.key = B_M|B_COMM|B_T    , .kana = "3007"}, // 〇
  {.key = B_M|B_COMM|B_A    , .kana = "3010"}, // 【
  {.key = B_M|B_COMM|B_S    , .kana = "3008"}, // 〈
  {.key = B_M|B_COMM|B_D       , .kana = "300E"}, // 『

  {.key = B_M|B_COMM|B_Z    , .kana = "3011"}, // 】
  {.key = B_M|B_COMM|B_X    , .kana = "3009"}, // 〉
  {.key = B_M|B_COMM|B_C    , .kana = "300F"}, // 』
//trp3l 固有名詞モード
  {.key = B_S|B_D|B_I       , .kana = "2191"}, // ↑
  {.key = B_S|B_D|B_K       , .kana = "2193"}, // ↓
  {.key = B_S|B_D|B_J       , .kana = "2190"}, // ←
  {.key = B_S|B_D|B_L       , .kana = "2192"}, // →
  {.key = B_K|B_L|B_S       , .kana = "967D 51EA"},//
//_trp3l
#endif
};


const PROGMEM naginata_keymap_ime ngmapi[] = {
  // 編集モード2
#ifdef NAGINATA_EDIT_MAC
  {.key = B_J|B_K|B_W       , .kana = "nagitatesenn"}, // ｜
  {.key = B_J|B_K|B_T       , .kana = "nagichuutenn"}, // ・
  {.key = B_J|B_K|B_A       , .kana = "nagikakkohi6"}, // （
  {.key = B_J|B_K|B_S       , .kana = "nagikakkohi5"}, // 《
  {.key = B_J|B_K|B_D       , .kana = "nagikakkohi3"}, // 「
  {.key = B_J|B_K|B_G       , .kana = "nagitentenn"}, // ……

  {.key = B_J|B_K|B_Z       , .kana = "nagikakkomi6"}, // ）
  {.key = B_J|B_K|B_X       , .kana = "nagikakkomi5"}, // 》
  {.key = B_J|B_K|B_C       , .kana = "nagikakkomi3"}, // 」
  {.key = B_J|B_K|B_B       , .kana = "nagisensenn"}, // ──

  {.key = B_M|B_COMM|B_T    , .kana = "nagimaru"}, // 〇
  {.key = B_M|B_COMM|B_A    , .kana = "nagikakkohi1"}, // 【
  {.key = B_M|B_COMM|B_S    , .kana = "nagikakkohi2"}, // 〈
  {.key = B_M|B_COMM|B_D       , .kana = "nagikakkohi4"}, // 『

  {.key = B_M|B_COMM|B_Z    , .kana = "nagikakkomi1"}, // 】
  {.key = B_M|B_COMM|B_X    , .kana = "nagikakkomi2"}, // 〉
  {.key = B_M|B_COMM|B_C    , .kana = "nagikakkomi4"}, // 』

  {.key = B_M|B_COMM|B_E    , .kana = SS_LCMD(SS_TAP(NGUP))" "SS_LCMD(SS_TAP(X_RIGHT))},
  {.key = B_M|B_COMM|B_T    , .kana = SS_LCMD(SS_TAP(NGUP))"   "SS_LCMD(SS_TAP(X_RIGHT))},
#endif
};
*/
//trp3l
//s動作速度向上のために 文字テーブルの要素数を保存しておく
const uint16_t ng_elem_count  = sizeof ngmap   / sizeof ngmap[0];
const uint16_t ng_elem_countl = sizeof ngmapl  / sizeof ngmapl[0];
//const uint16_t ng_elem_countu = sizeof ngmapu  / sizeof ngmapu[0];
//const uint16_t ng_elem_counti = sizeof ngmapi  / sizeof ngmapi[0];





/*void set_naginata(uint8_t ng_layer, uint16_t *onk, uint16_t *offk) {
  naginata_layer = ng_layer;

  ngon_keys[0]  = *onk;
  ngon_keys[1]  = *(onk+1);
  ngoff_keys[0] = *offk;
  ngoff_keys[1] = *(offk+1);
}*/
//trp3l
//s_cutレイヤ用に後半を追加
void set_naginata(uint8_t ng_layer, uint16_t *onk, uint16_t *offk, uint8_t s_layer, uint16_t *s_on, uint16_t *s_off) {
  naginata_layer = ng_layer;

  ngon_keys[0]  = *onk;
  ngon_keys[1]  = *(onk+1);
  ngoff_keys[0] = *offk;
  ngoff_keys[1] = *(offk+1);


  s_cut_layer = s_layer;

  s_on_keys[0]  = *s_on;
  s_on_keys[1]  = *(s_on+1);
  s_off_keys[0] = *s_off;
  s_off_keys[1] = *(s_off+1);
}
//trp3l

// 薙刀式をオン
void naginata_on(void) {
  is_naginata = true;
  keycomb = 0UL;
  naginata_clear();
  layer_on(naginata_layer);

  tap_code(KC_LNG1); // Mac
  //trp3l
  //tap_code(KC_HENK);// Win
  tap_code(JP_KANA);
  //_trp3l
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  keycomb = 0UL;
  naginata_clear();
  layer_off(naginata_layer);

  tap_code(KC_LNG2); // Mac
  tap_code(JP_MHEN); // Win
}

//trp3l
void s_cut_on(void){
	keycomb = 0UL;
	naginata_clear();
	layer_on(s_cut_layer);
}

void s_cut_off(void){
	keycomb = 0UL;
	naginata_clear();
	layer_off(s_cut_layer);
}
//trp3l

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

// OSのかな/英数モードをキーボードに合わせる
void makesure_mode(void) {
  if (is_naginata) {
    tap_code(KC_LNG1); // Mac
    //trp3l
//    tap_code(KC_HENK); // Win
  tap_code(JP_KANA);
  //\trp3l
  } else {
    tap_code(KC_LNG2); // Mac
    tap_code(JP_MHEN); // Win
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
/*
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
*/
// modifierが押されたら薙刀式レイヤーをオフしてベースレイヤーに戻す// get_mods()がうまく動かない

//static int n_modifier = 0;


bool process_modifier(uint16_t keycode, keyrecord_t *record) {
	if(is_mod_user(keycode) ){
      if (record->event.pressed) {
        layer_off(naginata_layer);
      } else {
        if (!get_mods_user()) {
          layer_on(naginata_layer);
        }
      }
      return true;
  }
  return false;
}
//trp3l
/*
void process_numkey(uint16_t keycode, keyrecord_t *record){
  switch (keycode) {
    case KC_0:
    case KC_1:
    case KC_2:
    case KC_3:
    case KC_4:
    case KC_5:
    case KC_6:
    case KC_7:
    case KC_8:
    case KC_9:
    case KC_SLASH:
    case JP_ASTR:
    case JP_PLUS:
    case KC_MINUS:
    case JP_EQL:
    case KC_PERC:
    case JP_AMPR:
    case JP_PIPE:
    case KC_LABK:
    case KC_RABK:
      if(record->event.pressed){
        tap_code(KC_LANG2); // Mac
        tap_code(KC_MHEN); // Win
      }else{
        tap_code(KC_LANG1); // Mac
        tap_code(KC_KANA);
      }
      break;
  }
}*/
//_trp3l
static uint16_t fghj_buf = 0;

// 薙刀式の起動処理(COMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
/*#ifdef COMBO_ENABLE
  bool ng_combo_result = true;
  if(record->event.pressed){
    if (keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1]
//trp3l   s_cut レイヤに関する条件式を追加

	  ||keycode == s_on_keys[0]  || keycode == s_on_keys[1]  ||
		keycode == s_off_keys[0] || keycode == s_off_keys[1]
//trp3l
    ) {
      if(fghj_buf == 0){
        ng_combo_result = process_combo(keycode, record);
      }
    }else{
      ng_combo_result = process_combo(keycode, record);
    }
  }else{
    if (!(keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1]
//trp3l   s_cut レイヤに関する条件式を追加

	  ||keycode == s_on_keys[0]  || keycode == s_on_keys[1]  ||
		keycode == s_off_keys[0] || keycode == s_off_keys[1]
//trp3l
)) {
  ng_combo_result = process_combo(keycode, record);
  if(!ng_combo_result && fghj_buf > 0){
    tap_code(KC_3);
    tap_code(fghj_buf);
  }
  fghj_buf = 0;
  return ng_combo_result;
    }
  }
#endif*/
  if (record->event.pressed) {

    if (fghj_buf == 0 && (keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1]
//trp3l   s_cut レイヤに関する条件式を追加

	  ||keycode == s_on_keys[0]  || keycode == s_on_keys[1]  ||
		keycode == s_off_keys[0] || keycode == s_off_keys[1])
//trp3l
    ) {
  /*#ifdef COMBO_ENABLE
        ng_combo_result = process_combo(keycode, record);
  #endif*/
      fghj_buf = keycode;
      return false;
    } else if (fghj_buf > 0) {
/*#ifdef COMBO_ENABLE
      if (keycode != ngon_keys[0] && keycode != ngon_keys[1] &&
          keycode != ngoff_keys[0] && keycode != ngoff_keys[1]
  //trp3l   s_cut レイヤに関する条件式を追加
  	  &&keycode != s_on_keys[0]  && keycode != s_on_keys[1]  &&
  		keycode != s_off_keys[0] && keycode != s_off_keys[1]
  //trp3l
) {
  ng_combo_result = process_combo(keycode, record);
}
#endif*/
      if ((keycode == ngon_keys[0] && fghj_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && fghj_buf == ngon_keys[0])   ) {
//trp3l
//s_cut レイヤに関する条件式を追加
   	    s_cut_off();
//trp3l

        naginata_on();
        fghj_buf = 0;
        return false;

      } else if ((keycode == ngoff_keys[0] && fghj_buf == ngoff_keys[1]) ||
                 (keycode == ngoff_keys[1] && fghj_buf == ngoff_keys[0])   ) {
        naginata_off();
        fghj_buf = 0;
        return false;
//trp3l
//s_cut レイヤに関する条件式を追加
      }else if ((keycode == s_on_keys[0] && fghj_buf == s_on_keys[1]) ||
                (keycode == s_on_keys[1] && fghj_buf == s_on_keys[0])   ){
    	  s_cut_on();
    	  fghj_buf = 0;
    	  return false;

      }else if ((keycode == s_off_keys[0] && fghj_buf == s_off_keys[1]) ||
                (keycode == s_off_keys[1] && fghj_buf == s_off_keys[0])   ){
		  s_cut_off();
		  fghj_buf = 0;
		  return false;
//trp3l
      } else {
#ifdef COMBO_ENABLE
        if(!ng_combo_result){
            fghj_buf = 0;
            return false;
        }
#endif
        tap_code(fghj_buf);
        fghj_buf = 0;
        return true;
      }
    }
  } else {/*
#ifdef COMBO_ENABLE
  ng_combo_result = process_combo(keycode, record);
  if(ng_combo_result && fghj_buf > 0){
    tap_code(fghj_buf);
  }
  fghj_buf = 0;
  return ng_combo_result;
#else*/
    if (fghj_buf > 0) {
      tap_code(fghj_buf);
      fghj_buf = 0;
      return true;
    }
//#endif
  }
  fghj_buf = 0;
  return true;
}


// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {
  if (!is_naginata)
//trp3l
    return enable_naginata(keycode, record);
	//return process_ng_combo(keycode, record);
//trp3l
  if (process_modifier(keycode, record))
    return true;

//trp3l
  //process_numkey(keycode, record);
//_trp3l
#ifdef NG_AUTO_REPEAT
  if (auto_repeat_keycode(keycode, record))
	return true;
#endif

  if (record->event.pressed) {
    switch (keycode) {
    case NG_SHFT ... NG_SHFT2:
#ifndef NAGINATA_KOUCHI_SHIFT
            if (ng_chrcount >= 1) {
              naginata_type();
              keycomb = 0UL;
            }
            ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
            ng_chrcount++;
            keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
            return false;
            break;
#endif
    case NG_Q ... NG_SLSH:
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
        //trp3l  ここが真のキーをすべて離した場所。
        else{
        }
        //trp3l
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
  //naginata_keymap_unicode bngmapu; // PROGMEM buffer
  //naginata_keymap_ime bngmapi; // PROGMEM buffer

  // keycomb_bufはバッファ内のキーの組み合わせ、keycombはリリースしたキーを含んでいない
  uint32_t keycomb_buf = 0UL;

  // バッファ内のキーを組み合わせる
  for (int i = 0; i < nt; i++) {
    keycomb_buf |= ng_key[ninputs[i] - NG_Q];
  }
  // NG_SHFT2はスペースの代わりにエンターを入力する
  if (keycomb_buf == B_SHFT && ninputs[0] == NG_SHFT2) {
    tap_code(KC_BSPC);
    compress_buffer(nt);
    return true;
  }

  if (shifted) {
    // 連続シフトを有効にする
    if ((keycomb & B_SHFT) == B_SHFT) keycomb_buf |= B_SHFT;

    // 編集モードを連続する
    if ((keycomb & (B_D | B_F))    == (B_D | B_F))    keycomb_buf |= (B_D | B_F);
    if ((keycomb & (B_C | B_V))    == (B_C | B_V))    keycomb_buf |= (B_C | B_V);
    if ((keycomb & (B_J | B_K))    == (B_J | B_K))    keycomb_buf |= (B_J | B_K);
    if ((keycomb & (B_M | B_COMM)) == (B_M | B_COMM)) keycomb_buf |= (B_M | B_COMM);
 //trp31
    //h編集モード3を連続する
    if ((keycomb & (B_S | B_D))    == (B_S | B_D))    keycomb_buf |= (B_S | B_D);
    if ((keycomb & (B_K | B_L))    == (B_K | B_L))    keycomb_buf |= (B_K | B_L);
//_trp3l
    // 濁音、半濁音を連続する
    if ((keycomb & B_F) == B_F) keycomb_buf |= B_F;
    if ((keycomb & B_J) == B_J) keycomb_buf |= B_J;
    if ((keycomb & B_V) == B_V) keycomb_buf |= B_V;
    if ((keycomb & B_M) == B_M) keycomb_buf |= B_M;
  }

//trp3l 同手シフト用
#ifdef SAMEHAND_SHFT
  if( samehand_shft(nt, keycomb_buf) ) return true;
#endif
//_trp3l
  switch (keycomb_buf) {
    // send_stringできないキー、長すぎるマクロはここで定義
//trp3l
    case B_Y|B_U:
		s_cut_off();
//_trp3l
    case B_H|B_J:
      naginata_on();
      compress_buffer(nt);
      return true;
      break;
//trp3l
    case B_R|B_G:
    	s_cut_on();
//_trp3l
    case B_F|B_G:
      naginata_off();
      compress_buffer(nt);
      return true;
      break;
/*
#ifdef NAGINATA_EDIT_WIN
    case B_M|B_COMM|B_Q: //    x   x   x ENT ENT
      for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
          send_unicode_hex_string("3000"); // 全角スペース
        send_unicode_hex_string("00D7"); // ×
      }
      tap_code(KC_ENT);
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_W: // ここから末までふりがな定義
      send_unicode_hex_string("FF5C");
      tap_code(KC_ENT);
      tap_code(KC_END);
      send_unicode_hex_string("300A 300B");
      tap_code(KC_ENT);
      tap_code(KC_LEFT);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_V: // 会話とじ次段落
      send_unicode_hex_string("300D");
      tap_code(KC_ENT);
      tap_code(KC_ENT);
      tap_code(KC_SPC);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_F: // 会話とじ次ひらき
      send_unicode_hex_string("300D");
      tap_code(KC_ENT);
      tap_code(KC_ENT);
      send_unicode_hex_string("300C");
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_B: // 会話とじ次行の文
      send_unicode_hex_string("300D");
      tap_code(KC_ENT);
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
#endif
*/
#ifdef NAGINATA_EDIT_MAC
    case B_M|B_COMM|B_E: // ここから末までふりがな定義
      mac_send_string("nagitatesenn");
      tap_code(KC_ENT);
      tap_code(KC_END);
      mac_send_string("nagikakkohi5");
      mac_send_string("nagikakkomi5");
      tap_code(KC_LEFT);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_V: // 会話とじ次段落
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      tap_code(KC_SPC);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_F: // 会話とじ次ひらき
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      mac_send_string("nagikakkohi3");
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_B: // 会話とじ次行の文
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
#endif
//trp3l
    case B_S|B_D|B_N:
      send_string("<br>");
      tap_code(KC_F10);
      tap_code(KC_ENT);
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
//trp3l

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
      /*for (int i = 0; i < ng_elem_countu; i++) {
        memcpy_P(&bngmapu, &ngmapu[i], sizeof(bngmapu));
        if (keycomb_buf == bngmapu.key) {
          send_unicode_hex_string(bngmapu.kana);
          if(keycomb_buf != (B_J|B_K|B_T)){//勝手に改行されちゃうからコメントアウトしてみた。
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
      }*/
  }
  return false;
}
