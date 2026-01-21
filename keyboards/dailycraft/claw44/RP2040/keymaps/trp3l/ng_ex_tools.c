#include QMK_KEYBOARD_H
#include "naginata.h"
#include "ng_ex_tools.h"

static int n_modifier = 0;

bool is_mod_user(uint16_t keycode){
	return (IS_QK_MODS(keycode) || IS_QK_MOD_TAP(keycode)  );
}

void mods_count_user(uint16_t keycode, keyrecord_t *record){
	if(is_mod_user(keycode)){
		if(record->event.pressed){
			n_modifier++;
		}else{
			n_modifier--;
		}
	}
}

int get_mods_user(void){
	return n_modifier;
}

typedef struct {
uint16_t keycode;
uint16_t time;
bool double_tapped;
} event_prev_t;

static event_prev_t event_prev ={ .keycode = 0, .time = 0, .double_tapped = false };

event_prev_t *recode_prev = &event_prev;

uint16_t get_keycode_prev(void){
	return event_prev.keycode;
}
uint16_t get_pressed_time_prev(void){
	return event_prev.time;
}

bool get_double_tap_state(void){
	return event_prev.double_tapped;
}
void set_keycode_prev(uint16_t keycode, uint16_t time){
	event_prev.keycode = keycode;
	event_prev.time = time;
	if(event_prev.double_tapped)
		event_prev.double_tapped = (keycode == event_prev.keycode);
}

bool is_released_within_time( uint16_t keycode, uint16_t tapping_tarm){
	return ((keycode == event_prev.keycode) && timer_elapsed(event_prev.time) < tapping_tarm );
}

bool is_double_tapped_within_time( uint16_t keycode, uint16_t tapping_tarm){
	return event_prev.double_tapped = is_released_within_time( keycode, tapping_tarm);
}

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

//薙刀式combo拡張パッケージ
/*
typedef struct{
	uint8_t layer;
	uint16_t onkeys[2];
	uint16_t offkeys[2];
} ng_combo_action_t;

static ng_combo_action_t ng_combos[2];
void set_ngcombos(uint8_t ng_layer, uint16_t *onk, uint16_t *offk, uint8_t s_layer, uint16_t *s_on, uint16_t *s_off) {

  ng_combos[0].layer = ng_layer;
  ng_combos[0].onkeys[0]  = *onk;
  ng_combos[0].onkeys[1]  = *(onk+1);
  ng_combos[0].offkeys[0] = *offk;
  ng_combos[0].offkeys[1] = *(offk+1);

  ng_combos[1].layer = ng_layer;
  ng_combos[1].onkeys[0]  = *s_on;
  ng_combos[1].onkeys[1]  = *(s_on+1);
  ng_combos[1].offkeys[0] = *s_off;
  ng_combos[1].offkeys[1] = *(s_off+1);
}
bool process_ng_combo(uint16_t keycode, keyrecord_t *record){
	bool result = false;
	for(int i = 0; i < 2; i++){
		set_naginata(ng_combos[i].layer, ng_combos[i].onkeys, ng_combos[i].offkeys);
		result = enable_naginata(keycode, record);
		if(!result)
			break;
	}
	return result;
}

}*/

#define SAMEHAND_SHFT

//同手シフト拡張パッケージ
#ifdef SAMEHAND_SHFT
//通常のかな等と同じ定義で実装できます。
typedef struct {
  uint32_t key;
  //naginata_keymap を流用してkana[7]にすると上手くいかない。
  char kana[8];
} naginata_keymap_same;
//keymap.cに設定したLRを定義してください。
#define NG_LSHFT NG_SHFT2
#define NG_RSHFT NG_SHFT
//同手シフトを追加したい場合はここ
const PROGMEM naginata_keymap_same ngmaps[] = {
	{.key = B_SHFT|B_Q       , .kana = SS_LCTL("a")},
	{.key = B_SHFT|B_R       , .kana = SS_LCTL("d")},
	{.key = B_SHFT|B_A       , .kana = SS_LCTL("z")},
	{.key = B_SHFT|B_S       , .kana = SS_LCTL("s")},
	{.key = B_SHFT|B_D       , .kana = SS_LCTL("y")},
	{.key = B_SHFT|B_G       , .kana = SS_LCTL("f")},
	{.key = B_SHFT|B_C       , .kana = SS_LCTL("v")},
	{.key = B_SHFT|B_V       , .kana = SS_LCTL("c")},
	{.key = B_SHFT|B_B       , .kana = SS_LCTL("x")},

	{.key = B_SHFT|B_I       , .kana = "tu"        },
	{.key = B_SHFT|B_L       , .kana = "ra"        },
	{.key = B_SHFT|B_SCLN    , .kana = "re"        },
};
const uint16_t ng_elem_counts  = sizeof ngmaps   / sizeof ngmaps[0];
//3つの状態で前回の同手シフトを記録しておく。（keycombではshftの左右が判定できない）
enum samehand_statuses{
	NO_SFT,
	SH_LSHFT,
	SH_RSHFT,
};
static uint8_t samehand_status = 0;
static const uint32_t LEFT_KEY = B_Q|B_W|B_E|B_R|B_T|
								 B_A|B_S|B_D|B_F|B_G|
								 B_Z|B_X|B_C|B_V|B_B;

//naginata.c のninputs[]を使いたい。
uint16_t get_ninputs_i(uint8_t ng_chr);

uint8_t samehand_shft_state(void){
	uint8_t bstate = NO_SFT;
	for (int i=0; get_ninputs_i(i); i++){
		if(get_ninputs_i(i) == NG_LSHFT){
			if(bstate == SH_RSHFT) break;
			bstate = SH_LSHFT;
		}else if(get_ninputs_i(i) == NG_RSHFT) {
			if(bstate == SH_LSHFT) break;
			bstate = SH_RSHFT;
		}
	}
	if(samehand_status != NO_SFT && bstate == NO_SFT){
		return samehand_status;
	}else{
		return bstate;
	}
}
bool samehand_shft(int nt, uint32_t keycomb_buf){
	naginata_keymap_same bngmaps; // PROGMEM buffer
	//シフト入力中でない時は判定をスキップして終了。
	if(!(keycomb_buf & B_SHFT)) return false;

	  //キーコードに左手のキーが含まれている & Lシフトを押している場合に同手シフトを起動する。
	  //連続シフト中などで、Lシフトより先に右手のキーやRシフトを押していた場合は起動しない。
	  //左手のキーはNG_OOで検出しても良いが、分岐条件が膨大になるため機械的に導出できるkeycombを採用した。
	if(keycomb_buf & LEFT_KEY && samehand_shft_state() == SH_LSHFT ){

		for (int i = 0; i < ng_elem_counts; i++) {
			memcpy_P(&bngmaps, &ngmaps[i], sizeof(bngmaps));
			if (keycomb_buf == bngmaps.key) {
			  send_string(bngmaps.kana);
			  compress_buffer(nt);
			  samehand_status = SH_LSHFT;
			  return true;
			}
		}
		samehand_status = SH_LSHFT;
		return false;
	  //Lと同様。メモリ削減のため、右手のキーもLEFT_KEYで検出できるようにしてある。
	}else if(keycomb_buf & ~(LEFT_KEY|B_SHFT) && samehand_shft_state() == SH_RSHFT ){

		for (int i = 0; i < ng_elem_counts; i++) {
	        memcpy_P(&bngmaps, &ngmaps[i], sizeof(bngmaps));
	        if (keycomb_buf == bngmaps.key) {
	          send_string(bngmaps.kana);
	          compress_buffer(nt);
			  samehand_status = SH_RSHFT;
			  return true;
	        }
		  }
		samehand_status = SH_RSHFT;
        return false;
	}else{
		samehand_status = NO_SFT;
		return false;
	}
}
#endif
//オートリピート機能
#ifdef NG_AUTO_REPEAT

//オートリピート開始時に引っ掛かりを設定する。二連打とオートリピート発火を二回目押下後にも選択したいときはここ。
//#define FIRST_DELAY
#ifdef FIRST_DELAY
//引っ掛かりの長さ設定。ミリ秒単位。
#define F_DELAY_TIME 500
#endif
//オートリピートのトグル速度。ミリ秒。
#define AUTO_REPEAT_TOGGLE 30
//オートリピート発火を押下時ではなく、離した時からの経過時間で判定したい場合はコメントアウトを外してください。
//#define AUTO_REPEAT_TRIGGER_IS_RELEASED
//オートリピートを開始するために必要なキーの連打速度。ミリ秒。
#define AUTO_REPEAT_TAPPING_TERM 150
//オートリピートしたいキーをここに設定する。pgupとか追加したいときはここ。
const PROGMEM uint32_t auto_repeat_keycodemap[] = {
	B_T,
	B_Y,
	B_U,
	B_T|B_SHFT,
	B_Y|B_SHFT,
	B_D|B_F|B_J,
	B_D|B_F|B_K,
	B_D|B_F|B_L,
	B_D|B_F|B_M,
	B_D|B_F|B_COMM,
	B_D|B_F|B_DOT,

	B_C|B_V|B_U,
	B_C|B_V|B_I,
	B_C|B_V|B_O,
	B_C|B_V|B_P,
	B_C|B_V|B_J,
	B_C|B_V|B_K,
	B_C|B_V|B_L,
	B_C|B_V|B_SCLN,
};
const uint16_t auto_repeat_elem_counts  = sizeof auto_repeat_keycodemap / sizeof auto_repeat_keycodemap[0];

//naginata.cの変数を操作する。
uint32_t get_keycomb(void);
uint32_t get_ng_key(uint16_t keycode);
void add_chrcount(void);
void set_ninputs_i(uint16_t keycode);

static bool is_auto_repeating = false;
uint16_t last_auto_repeat_key_typed_time = 0;

void auto_repeat_matrix_scan(void){
	if(is_auto_repeating && timer_elapsed(last_auto_repeat_key_typed_time) > AUTO_REPEAT_TOGGLE){
#ifdef FIRST_DELAY
		if(last_auto_repeat_key_typed_time == 0)
			SEND_STRING(SS_DELAY(F_DELAY_TIME));
#endif
		set_ninputs_i(get_keycode_prev());
		add_chrcount();
		naginata_type();
		last_auto_repeat_key_typed_time = timer_read();
	}
}

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
		if( is_double_tapped_within_time(keycode, AUTO_REPEAT_TAPPING_TERM) ){

			uint32_t keycomb_s = (get_keycomb() | get_ng_key(keycode));// キーの重ね合わせ
			uint32_t keycomb_sb = 0UL;
			//キーコードの検索。
			for(int i = 0; i < auto_repeat_elem_counts; i++){
				memcpy_P(&keycomb_sb , &auto_repeat_keycodemap[i], sizeof(keycomb_sb));
				if(keycomb_sb == keycomb_s){//オートリピートのフラグをtrueに。matrix_scan_userに動作を引き継ぐ。
					is_auto_repeating =true;
					return true;
				}
			}
			//オートリピート対象のキーコードではなかった。
			return false;

		}else{//時間切れ or 連続入力ではない。
			return false;
		}

	}else{//key release
		if(is_auto_repeating){
			is_auto_repeating = false;
			last_auto_repeat_key_typed_time = 0;
		}
		#ifdef AUTO_REPEAT_TRIGGER_IS_RELEASED
			set_event_prev(keycode, record->event.time);
		#endif
		return false;
	}
}
#endif
