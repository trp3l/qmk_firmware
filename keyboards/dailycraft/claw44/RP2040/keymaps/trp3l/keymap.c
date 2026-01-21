#include QMK_KEYBOARD_H
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif

#include "keymap_japanese.h"

#include "naginata.h"
NGKEYS naginata_keys;
#include "ng_ex_tools.h"
extern keymap_config_t keymap_config;

//#define RKJX


extern uint8_t is_master;


enum keymap_layers {
	_DEF,
	_NAGINATA,
	_S_CUT,
	_SFT,
	_PAINT,
	_LOWER,
	_RAISE,
  _QWERTY, 
  _LAYERS
};

enum custom_keycodes {
  S_CUT = NG_SAFE_RANGE,
  S_TEXT,
  ALT_TAB,
  COMENT_S,
  COMENT_E,
  NG_ON,
  NG_OFF
  //I2C_TEST
};

//tap_danc----------------------------------------------------------------------
enum tap_danc_key_codes{
	_PRN,
	_CBR,
	_BRC,
	COMMA_SCOL,
	ENT_ESC
};

tap_dance_action_t tap_dance_actions[] = {
		[_PRN]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LPRN, JP_RPRN),
		[_CBR]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LCBR, JP_RCBR),
		[_BRC]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_RBRC),
		[COMMA_SCOL]  	 = ACTION_TAP_DANCE_DOUBLE(KC_COMMA, KC_SCLN),
		[ENT_ESC]		 = ACTION_TAP_DANCE_DOUBLE(KC_ENTER, KC_ESCAPE)
};
//layout----------------------------------------------------------------------
#define LT_TAB LT(_RAISE,KC_TAB)
#define LT_ENT LT(_LOWER,KC_ENTER)
#define LT_BS LT(_S_CUT, KC_BSPC)
#define CTL_T_BS LCTL_T(KC_BSPC)
#define LT_SPC LT(_SFT, KC_SPC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_DEF] = LAYOUT(
    XXXXXXX,  KC_ESC,          KC_A,   KC_R,    KC_D,    KC_RGHT,                      KC_LEFT, KC_BSPC, KC_U, KC_P, KC_Z, XXXXXXX,
    KC_LCTL,  KC_W,            KC_S,   KC_H,    KC_T,    KC_G,     XXXXXXX,XXXXXXX,    KC_Y,    KC_I,    KC_E, KC_O, KC_N, XXXXXXX,
    XXXXXXX,  LGUI_T(JP_COLN), KC_F,   KC_V,    KC_C,    KC_X,     XXXXXXX,XXXXXXX,    KC_M,    KC_L,    KC_J, KC_B, KC_K, KC_RGUI,
                                   TD(_PRN), LT_BS, LT_TAB, KC_LALT,        KC_RALT, LT_ENT,    LT_SPC,  TD(COMMA_SCOL)
    ),

	[_NAGINATA] = LAYOUT(
		_______ , NG_Q,    NG_W,    NG_E,   NG_R,   NG_T,                      NG_Y,    NG_U,    NG_I,    NG_O,    NG_P,    _______,
		_______ , NG_A,    NG_S,    NG_D,   NG_F,   NG_G,  XXXXXXX,XXXXXXX,    NG_H,    NG_J,    NG_K,    NG_L,    NG_SCLN, _______,
		XXXXXXX , NG_Z,    NG_X,    NG_C,   NG_V,   NG_B,  XXXXXXX,XXXXXXX,    NG_N,    NG_M,    NG_COMM, NG_DOT,  NG_SLSH, _______,
                     _______,NG_SHFT2,  LT_TAB, _______ ,                  _______, LT_ENT,  NG_SHFT, _______
		),

	[_S_CUT] = LAYOUT(
		XXXXXXX,    XXXXXXX,    LCTL(KC_A),	KC_LALT,	  LCTL(KC_D),	XXXXXXX,		                  XXXXXXX,  KC_HOME,      	KC_END,         LCTL(KC_P),  XXXXXXX,    XXXXXXX,
	  LCTL(KC_W), LCTL(KC_Z), LCTL(KC_S), LCTL(KC_Y),	LCTL(KC_H),	LCTL(KC_F), XXXXXXX,XXXXXXX,	KC_UP,    LCTL(KC_LEFT),  LCTL(KC_RGHT),  LCTL(KC_O),  LCTL(KC_N),	XXXXXXX,
		XXXXXXX,    XXXXXXX,    LCTL(KC_O),	LCTL(KC_V), LCTL(KC_C),	LCTL(KC_X), XXXXXXX,XXXXXXX, 	KC_DOWN,	KC_LEFT,	      KC_RGHT,        XXXXXXX,     XXXXXXX,	    KC_RGUI,
							              XXXXXXX,     XXXXXXX,	  XXXXXXX,	  TO(_LAYERS),                _______,  XXXXXXX, KC_LSFT,	XXXXXXX
		),
  [_SFT] = LAYOUT(
    XXXXXXX, LSFT(KC_Q), LSFT(KC_A), LSFT(KC_R), LSFT(KC_D), LSFT(KC_RGHT),                 LSFT(KC_LEFT),XXXXXXX,    LSFT(KC_U), LSFT(KC_P), LSFT(KC_Z), XXXXXXX,
    KC_LCAP, LSFT(KC_W), LSFT(KC_S), LSFT(KC_H), LSFT(KC_T), LSFT(KC_G), XXXXXXX,XXXXXXX,   LSFT(KC_Y),   LSFT(KC_I), LSFT(KC_E), LSFT(KC_O), LSFT(KC_N), XXXXXXX,
    XXXXXXX, JP_ASTR,    LSFT(KC_F), LSFT(KC_V), LSFT(KC_C), LSFT(KC_X), XXXXXXX,XXXXXXX,   LSFT(KC_M),   LSFT(KC_L), LSFT(KC_J), LSFT(KC_B), LSFT(KC_K), XXXXXXX,
                                   TD(_PRN), LT_BS, LT_TAB, TO(_LAYERS),                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
    ),

	[_PAINT] = LAYOUT(
		XXXXXXX,          KC_A,   KC_LSFT,  KC_LALT,	KC_D,	  S_TEXT,		                  NG_ON,    TO(_DEF),  KC_H,    KC_P,     XXXXXXX,  XXXXXXX,
		LT(_RAISE,KC_E),  KC_Z,   KC_S,     KC_Y,	    KC_SPC,	KC_F,     XXXXXXX,XXXXXXX,  KC_LEFT,  KC_UP,	   KC_DOWN, KC_RGHT,  KC_TAB,   LT(_LOWER,KC_BSPC),
		XXXXXXX,          KC_N,   KC_O,     KC_V,	    KC_C,	  KC_X,     XXXXXXX,XXXXXXX,  KC_M,     KC_L, 	   KC_J,    KC_B,     KC_K,     XXXXXXX,
							                KC_W,     KC_LCTL,  KC_H,   TD(ENT_ESC),		            XXXXXXX,  LT_ENT,    KC_B,	  KC_G
		),

	[_LOWER] = LAYOUT(
		XXXXXXX ,JP_TILD,   JP_AT,   KC_LABK,    KC_QUES,		KC_RABK,		                JP_BSLS,       KC_INS,    KC_PGUP,  KC_PAUSE ,KC_PSCR, XXXXXXX,
		JP_UNDS ,JP_DQUO,   JP_QUOT, TD(_BRC),   TD(_CBR),	JP_AMPR,  XXXXXXX,XXXXXXX,	LCTL(KC_UP),   XXXXXXX,   KC_PGDN,  XXXXXXX   ,JP_EQL,  KC_BSPC,
		XXXXXXX ,JP_HASH,   KC_DLR,  JP_CIRC,    JP_EXLM,		JP_PIPE,	XXXXXXX,XXXXXXX,  LCTL(KC_DOWN), COMENT_S,  KC_APP,   COMENT_E ,KC_MINUS, XXXXXXX,
		          				  JP_GRV,  CTL_T_BS,   LT_TAB,    TO(_LAYERS),                XXXXXXX,  XXXXXXX ,XXXXXXX, XXXXXXX
		),

	[_RAISE] = LAYOUT(
		XXXXXXX,    KC_Q, 	    LALT(KC_SPC),	KC_F11,	     KC_DEL,  KC_F12,		                  JP_YEN,   KC_7,   KC_8,  KC_9,	KC_SLASH,	JP_ASTR,
		LALT(KC_F4),LCTL(KC_W),	LGUI(KC_S), 	LGUI(KC_E),	 KC_SPC,  ALT_TAB,  XXXXXXX,XXXXXXX,	KC_UP,    KC_4,   KC_5,  KC_6, 	JP_EQL,		JP_PLUS,
		XXXXXXX,    LSFT(KC_CAPS), 	KC_F2,	  LGUI(KC_V),  KC_ENT,  KC_F5,    XXXXXXX,XXXXXXX,	KC_DOWN,  KC_1,   KC_2,  KC_3,	KC_MINUS,	KC_PERC,
								      XXXXXXX,	XXXXXXX,  XXXXXXX,  TO(_LAYERS),          _______,	KC_ENTER, JP_DOT, KC_0
		),

    [_QWERTY] = LAYOUT(
    KC_ESC, KC_Q,   KC_W,   KC_E,    KC_R,    KC_T,                    KC_Y,    KC_I,  KC_U,    KC_O,   KC_P,     KC_MINS,
    KC_TAB, KC_A,   KC_S,   KC_D,    KC_F,    KC_G,  XXXXXXX,XXXXXXX,  KC_H,    KC_J,  KC_K,    KC_L,   KC_SCLN,  KC_QUOT,
    KC_LSFT,KC_Z,   KC_X,   KC_C,    KC_V,    KC_B,  XXXXXXX,XXXXXXX,  KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH,  KC_RGUI,
                      XXXXXXX, CTL_T_BS, LT_TAB, TO(_LAYERS),        KC_RALT, LT_ENT, RSFT_T(KC_SPACE), TD(COMMA_SCOL)
    ),

    [_LAYERS] = LAYOUT(
    XXXXXXX, TO(_QWERTY), XXXXXXX,    TO(_RAISE), TO(_DEF), XXXXXXX,                    XXXXXXX,  XXXXXXX,    XXXXXXX, TO(_PAINT),XXXXXXX,  XXXXXXX,
    XXXXXXX, XXXXXXX,     TO(_S_CUT), XXXXXXX,    XXXXXXX,      XXXXXXX,  XXXXXXX,XXXXXXX,  XXXXXXX,  XXXXXXX,    XXXXXXX, XXXXXXX, NG_ON, XXXXXXX,
    XXXXXXX, XXXXXXX,     TO(_SFT),   XXXXXXX,    XXXXXXX,      XXXXXXX,  XXXXXXX,XXXXXXX,  XXXXXXX,  TO(_LOWER), XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                      XXXXXXX,  XXXXXXX,  XXXXXXX,  XXXXXXX,                    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
    )
};

void keyboard_post_init_user(void){
  uint16_t ngonkeys[]  = {KC_Y, KC_I};
  uint16_t ngoffkeys[] = {KC_T, KC_G};
  //s_cut切り替え
  uint16_t s_onkeys[]  = {KC_G, KC_D};
  uint16_t s_offkeys[] = {KC_D, KC_C};
  set_naginata(_NAGINATA, ngonkeys, ngoffkeys,  _PAINT, s_onkeys, s_offkeys);
  //s_cutここまで
  //set_naginata(_NAGINATA, ngonkeys, ngoffkeys);

#  ifdef NAGINATA_EDIT_MAC
	 set_unicode_input_mode(UC_OSX);
#  endif
#  ifdef NAGINATA_EDIT_WIN
//	  set_unicode_input_mode(UC_WINC);
#endif

#ifdef RKJX
  uint8_t number_of_rkjx_layers =  (sizeof rkjx_keymaps_R / sizeof rkjx_keymaps_R[0]) - 1;
  rkjx_init(number_of_rkjx_layers);
#endif
}

void matrix_scan_user(void) {
#ifdef NG_AUTO_REPEAT
   auto_repeat_matrix_scan();
#endif
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {        // left ↑
        if (clockwise) {
            tap_code(KC_AUDIO_VOL_UP);
        } else {
            tap_code(KC_AUDIO_VOL_DOWN);
        }
    /*
    } else if (index == 1) { // left ↓
        if (clockwise) {
            tap_code(KC_3);
        } else {
            tap_code(KC_4);
        }
    } else if (index == 2) { // right ↑
        if (clockwise) {
            tap_code(KC_5);
        } else {
            tap_code(KC_6);
        }
    } else if (index == 3) { // right ↓
        if (clockwise) {
            tap_code(KC_7);
        } else {
            tap_code(KC_8);
        }
    */
    }
    
    return true;
}

void  housekeeping_task_user(void){
  #ifdef RKJX
    rkjx_matrix_scan();
  #endif
}

void oneshot_mods_changed_user(uint8_t mods) {
  if(mods & MOD_MASK_SHIFT){

  }
}

//macro-------------------------------------------------------------------------
bool alt_tab_state = false;
//note : keyrecord_t は tmk_core/common/action.h line51 に定義してあるよ
//event.pressed は tmk_core/common/keycode.h line35
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	mods_count_user(keycode, record);

  switch (keycode) {
    case S_CUT:
	  if (record->event.pressed) {
		 if(is_double_tapped_within_time(keycode, 200)){
			 register_code(KC_BSPC);
		 }else{
			 layer_on(_S_CUT);
			 register_code(KC_LCTL);
		 }
		 set_keycode_prev(keycode, record->event.time);
	  }else{
		  if(get_double_tap_state()){
			 unregister_code(KC_BSPC);
		  }else{
			 unregister_code(KC_LCTL);
			 layer_off(_S_CUT);
			 if(is_released_within_time(keycode, 500)){
				tap_code(KC_BSPC);
			 }
		  }
	  }
	  return false;
 	  break;

   case S_TEXT:
    	if(record->event.pressed){
    		tap_code(KC_T);
    		if(!get_mods_user()){
    			layer_off(_PAINT);
    			naginata_on();
    		}
        set_keycode_prev(keycode, record->event.time);
	    }
	    return false;
	    break;

   case ALT_TAB:
        if(record->event.pressed){
          if(get_keycode_prev() != ALT_TAB){
            register_code(KC_LALT);
          }
          tap_code(KC_TAB);
          alt_tab_state = true;
          set_keycode_prev(keycode, record->event.time);
        }
        return false;
        break;

    case LT_TAB:
      if (!record->event.pressed && alt_tab_state) {
        unregister_code(KC_LALT);
        alt_tab_state =false;
      }
      break;

  case COMENT_S:
    if(record->event.pressed){
      SEND_STRING("/\"");// "/*" jisの*はUSのダブルクォーテーション。
      set_keycode_prev(keycode, record->event.time);
    }
    return false;
    break;

  case COMENT_E:
    if (record->event.pressed) {
      SEND_STRING("\"/");// "*/" jisの*はUSのダブルクォーテーション。
      set_keycode_prev(keycode, record->event.time);
    }
    return false;
    break;

  case NG_ON:
    if (record->event.pressed) {
      naginata_on();
      set_keycode_prev(keycode, record->event.time);
    }
    return false;
    break;
  case NG_OFF:
    if (record->event.pressed) {
      naginata_off();
      set_keycode_prev(keycode, record->event.time);
    }
    return false;
    break;
      

/*
    case LSFT(KC_UP):
    case LSFT(KC_DOWN):
      if(!record->event.pressed){
        tap_code(KC_END);
      }
      break;

    case LALT(KC_SPC):
    if(!record->event.pressed){
       tap_code(KC_N);
    }
     break;

   case RGUI_T(KC_SLEP):
		   if(!record->event.pressed && is_released_within_time(keycode, 300)){
			   layer_off(_PAINT);
			   naginata_on();
		   }
   break;
*/

  }//end of switch


  if (!process_naginata(keycode, record)){
	  if(record->event.pressed){
		  set_keycode_prev(keycode, record->event.time);
	  }
      return false;
  }

  if(record->event.pressed){
    //if(get_keycode_prev() == LT_SPC) layer_off(_SFT);
    
    set_keycode_prev(keycode, record->event.time);
  }

  return true;
}
