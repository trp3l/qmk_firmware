#include QMK_KEYBOARD_H
#include "bootloader.h"
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif
#include "keymap_jp.h"

#include "naginata.h"
NGKEYS naginata_keys;

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;


enum keymap_layers {
	_XIN,
	_NAGINATA,
	_S_CUT,
	_LOWER,
	_RAISE
};

enum custom_keycodes {
  S_CUT_W = NG_SAFE_RANGE,
  S_TEXT,
  BACKLIT,
  RGBRST
};

//comboは使わなくなりました。-------------------------------------------------------------------------

/*enum combo_events {
  S_CUT_ON_CMB,
  S_CUT_OFF_CMB
};
const uint16_t PROGMEM s_cut_on_combo[]  = {KC_D, KC_R, COMBO_END};
const uint16_t PROGMEM s_cut_off_combo[] = {KC_C, KC_D, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
  [S_CUT_ON_CMB]		= COMBO_ACTION(s_cut_on_combo),
  [S_CUT_OFF_CMB]		= COMBO_ACTION(s_cut_off_combo),
};

void process_combo_event(uint8_t combo_index, bool pressed) {
  switch(combo_index) {
  	case S_CUT_ON_CMB:
        if (pressed) {
        tap_code(KC_BSPC);
      	 layer_on(_S_CUT);
        }
        break;
    case S_CUT_OFF_CMB:
      if (pressed) {
    	 layer_off(_S_CUT);
      }
      break;
  }
}*/
//tap_danc----------------------------------------------------------------------
enum tap_danc_key_codes{
	_PRN,
	_CBR,
	_BRC,
	COMMA_SCOL,
	ENT_ESC
};

qk_tap_dance_action_t tap_dance_actions[] = {
		[_PRN]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LPRN, JP_RPRN),
		[_CBR]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LCBR, JP_RCBR),
		[_BRC]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_RBRC),
		[COMMA_SCOL]  	 = ACTION_TAP_DANCE_DOUBLE(KC_COMMA, KC_SCOLON),
		[ENT_ESC]		 = ACTION_TAP_DANCE_DOUBLE(KC_ENTER, KC_ESCAPE)
};
//layout----------------------------------------------------------------------
#define KC______ KC_TRNS
#define KC_XXXXX KC_NO
#define KC_LOWER LOWER
#define KC_RAISE RAISE
#define KC_LRST  RGBRST



#define LT_TAB LT(_RAISE,KC_TAB)
#define LT_ENTER LT(_LOWER,KC_ENTER)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_XIN] = LAYOUT(
		LALT_T(KC_ESCAPE),	KC_Q, 		KC_A,	 KC_R,	 KC_D,	   KC_RIGHT,		KC_LEFT, KC_BSPACE,KC_U,	   KC_P,	KC_Z,	RCTL_T(KC_PSCREEN),
		LSFT_T(KC_DELETE),	KC_W,		KC_S,	 KC_H,	 KC_T,	   KC_G,			KC_Y,	 KC_I,	   KC_E,	   KC_O,	KC_N,	KC_BSPACE,
	 LWIN_T(LSFT(KC_KPS)),	JP_COLN,	KC_F,	 KC_V,	 KC_C,	   KC_X,			KC_M,	 KC_L,	   KC_J,	   KC_B,	KC_K,	RWIN_T(KC_SLEP),
								TD(_PRN),  S_CUT_W,  LT_TAB,		LT_ENTER, RSFT_T(KC_SPACE), TD(COMMA_SCOL)
		),


	[_NAGINATA] = LAYOUT(
		_______ ,NG_Q,	   NG_W,	NG_E,	NG_R,	  NG_RIGHT,  NG_LEFT,    NG_U,    NG_I,    NG_O,    NG_P,    _______,
		_______ ,NG_A,	   NG_S,	NG_D,	NG_F,	  NG_G, 		NG_H,    NG_J,    NG_K,    NG_L,    NG_SCLN, _______,
		_______ ,NG_Z,	   NG_X,	NG_C,	NG_V,	  NG_B, 		NG_N,    NG_M,    NG_COMM, NG_DOT,  NG_SLSH, _______,
								  _______, NG_SHFT2,  LT_TAB, 	   LT_ENTER, NG_SHFT, _______
		),


	[_S_CUT] = LAYOUT(
		  TD(ENT_ESC),  KC_A,	KC_LSHIFT,	KC_LALT,	KC_D,		S_TEXT,		KC_Y,	 KC_I,	   KC_UP,  KC_DELETE,   KC_PAUSE,   KC_PSCREEN,
	  LT(_RAISE,KC_E),  KC_S, 	KC_Z,		KC_Y,		KC_SPACE,	KC_F,		XXXXXXX, KC_LEFT, KC_DOWN, KC_RIGHT,	KC_TAB,		LT(_LOWER,KC_BSPACE),
				 KC_R,  KC_Q,	KC_O,		KC_V,		KC_C,		KC_X,		KC_J,	 KC_H,	  XXXXXXX, KC_SCOLON,   KC_P,	    KC_RWIN,
											KC_W,  		KC_LCTRL,	KC_N,	   KC_ENTER, KC_B,	KC_G
		),

	[_LOWER] = LAYOUT(
			JP_TILD ,JP_BSLS ,	JP_AT,	  JP_EXLM, 	    	KC_QUES,		JP_PIPE,		_______ ,_______ ,KC_PGUP ,KC_INS  ,KC_PAUSE,_______ ,
			JP_UNDS ,JP_DQT,	JP_QUOT,  TD(_BRC), 		TD(_CBR),		JP_GRV,			_______ ,KC_HOME ,KC_PGDN ,KC_END  ,_______ ,KC_BSPACE ,
			KC_INS  ,JP_HASH, 	KC_DLR,   JP_CIRC,			KC_RABK, 		JP_AMPR,		_______ ,_______ ,KC_APP  ,_______ ,_______ ,_______ ,
											KC_LABK, 	 LCTL_T(KC_BSPACE),	  LT_TAB,	   XXXXXXX,  RSFT_T(KC_SPACE) ,TD(COMMA_SCOL)
		),


	[_RAISE] = LAYOUT(
		_______,JP_DOT, 	KC_F9,	KC_F10,	KC_F11,	KC_F12,		JP_YEN  ,KC_7,	 KC_8,	KC_9 ,	KC_SLASH,	JP_ASTR,
		_______,KC_SPACE,	KC_F5, 	KC_F6,	KC_F7,	KC_F8,		KC_UP   ,KC_4,	 KC_5,	KC_6, 	JP_EQL,		JP_PLUS,
		_______,KC_ENTER, 	KC_F1,	KC_F2,	KC_F3, 	KC_F4,		KC_DOWN ,KC_1,	 KC_2,	KC_3,	KC_MINUS,	KC_PERC,
									_______,_______,XXXXXXX,	KC_ENTER, JP_DOT, KC_0
		)


};


int RGB_current_mode;



void matrix_init_user(void) {

	  uint16_t ngonkeys[]  = {KC_Y, KC_I};
	  uint16_t ngoffkeys[] = {KC_T, KC_G};
   //s_cut切り替え
	  uint16_t s_onkeys[]  = {KC_G, KC_D};
	  uint16_t s_offkeys[] = {KC_D, KC_C};
	  set_naginata(_NAGINATA, ngonkeys, ngoffkeys,  _S_CUT, s_onkeys, s_offkeys);
   //s_cutここまで
	  //set_naginata(_NAGINATA, ngonkeys, ngoffkeys);

	  #ifdef NAGINATA_EDIT_MAC
	  set_unicode_input_mode(UC_OSX);
	  #endif
	  #ifdef NAGINATA_EDIT_WIN
	  set_unicode_input_mode(UC_WINC);
	  #endif


    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLED

//macro-------------------------------------------------------------------------
bool s_cut_pressed = false;
bool modifier_pressed = false;
static uint16_t W_pressed_time = 0;
//note : keyrecord_t は tmk_core/common/action.h line51 に定義してあるよ
//event.pressed は tmk_core/common/keycode.h line35
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
	#ifdef SSD1306OLED
		set_keylog(keycode, record);
	#endif
	// set_timelog();
  }

  switch (keycode) {
    case S_CUT_W:
 		  if (record->event.pressed) {
 			 s_cut_pressed = true;
 			W_pressed_time = record->event.time;
 			 layer_on(_S_CUT);
 			 SEND_STRING(SS_DOWN(X_LCTRL));
 		  }else{
 			 SEND_STRING(SS_UP(X_LCTRL));
 			 layer_off(_S_CUT);
 			 if(s_cut_pressed && TIMER_DIFF_16(record->event.time, W_pressed_time) < 500){
 				tap_code(KC_BSPACE);
 			}
     		s_cut_pressed = false;
     		W_pressed_time = 0;
 		  }
 	  return false;
 	  break;

   case S_TEXT:
	if(record->event.pressed){
		tap_code(KC_T);
		if(!modifier_pressed){
//		if(get_mods() != 0){
			layer_off(_S_CUT);
			naginata_on();
		}
	}
	return false;
	break;

    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
      break;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;

    case KC_LCTRL:
    case KC_LSHIFT:
    case KC_LALT:
    	if (record->event.pressed) {
    		modifier_pressed = true;
    	}else{
    		modifier_pressed = false;
    	}
    	break;

	default:
		if (record->event.pressed) {
		s_cut_pressed = false;
		}

  }//end of switch


  if (!process_naginata(keycode, record))
        return false;

  return true;
}
