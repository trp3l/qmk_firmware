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

#include "kogatana.h"
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
	_LOWER,
	_RAISE
};

enum custom_keycodes {
  EISU = NG_SAFE_RANGE,
  KANA2,
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
	PRN_DANCE,
	CBR_DANCE,
	BRC_DANCE,
	COMMA_DANCE
};

qk_tap_dance_action_t tap_dance_actions[] = {
		[PRN_DANCE]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LPRN, JP_RPRN),
		[CBR_DANCE]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LCBR, JP_RCBR),
		[BRC_DANCE]  	 = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_RBRC),
		[COMMA_DANCE]  	 = ACTION_TAP_DANCE_DOUBLE(KC_COMMA, KC_SCOLON)
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
		LALT_T(KC_ESCAPE),	KC_Q, 		KC_A,	 KC_R,	 KC_D,	   KC_RIGHT,		KC_LEFT, KC_BSPACE,KC_U,	   KC_P,	KC_Z,		KC_PSCREEN,
		LSFT_T(KC_DELETE),	KC_W,	   KC_S,	 KC_H,	 KC_T,	   KC_G,			KC_Y,	 KC_I,	   KC_E,	   KC_O,	KC_N,	  	KC_BSPACE,
		LWIN(LSFT(KC_KPS)),	JP_COLN,	KC_F,	 KC_V,	 KC_C,	   KC_X,			KC_M,	 KC_L,	   KC_J,	   KC_B,	KC_K,	RWIN_T(KC_SLEP),
								TD(PRN_DANCE), LCTL_T(KC_BSPACE),  LT_TAB,			LT_ENTER, RSFT_T(KC_SPACE), TD(COMMA_DANCE)
		),


	[_NAGINATA] = LAYOUT(
        XXXXXXX, NG_V,    NG_Z,   NG_A,   NG_Q,   XXXXXXX, XXXXXXX,   NG_Q,    NG_A,    NG_Z,    NG_V, XXXXXXX,
        XXXXXXX, NG_SHFT, NG_X,   NG_S,   NG_W,   XXXXXXX, XXXXXXX,   NG_W,    NG_S,    NG_X, NG_SHFT, XXXXXXX,
        XXXXXXX, NG_N,    NG_C,   NG_D,   NG_E,   NG_T,       NG_T,   NG_E,    NG_D,    NG_C,    NG_N, XXXXXXX,
                                  NG_F,   NG_R,   NG_Y,       NG_Y,   NG_R,    NG_F
		),

	[_LOWER] = LAYOUT(
			JP_TILD ,JP_BSLS ,	JP_AT,	  JP_EXLM,	    	KC_QUES,	JP_PIPE,		_______ ,_______ ,KC_PGUP ,KC_INS  ,_______ ,_______ ,
			JP_UNDS ,JP_DQT,	JP_QUOT,  TD(BRC_DANCE),	TD(CBR_DANCE),	JP_GRV,			KC_PAUSE,KC_HOME ,KC_PGDN ,KC_END  ,_______ ,KC_BSPACE ,
			KC_INS  ,JP_HASH, 	KC_DLR,   JP_CIRC,		KC_RABK, 	JP_AMPR,		_______ ,_______ ,KC_APP  ,_______ ,_______ ,_______ ,
							  KC_LABK, 	    LCTL_T(KC_BSPACE),	LT_TAB,		XXXXXXX ,RSFT_T(KC_SPACE) ,TD(COMMA_DANCE)
		),


	[_RAISE] = LAYOUT(
		_______,JP_DOT, 	KC_F9,	KC_F10,	KC_F11,	KC_F12,			JP_YEN  ,KC_7,	 KC_8,	KC_9 ,	KC_SLASH,	JP_ASTR,
		_______,KC_SPACE,	KC_F5, 	KC_F6,	KC_F7,	KC_F8,		KC_UP   ,KC_4,	 KC_5,	KC_6, 	JP_EQL,		JP_PLUS,
		_______,KC_ENTER, 	KC_F1,	KC_F2,	KC_F3, 	KC_F4,		KC_DOWN ,KC_1,	 KC_2,	KC_3,	KC_MINUS,	KC_PERC,
									_______,_______,XXXXXXX,	KC_ENTER,JP_DOT, KC_0
		)


};


int RGB_current_mode;



void matrix_init_user(void) {

	  uint16_t ngonkeys[]  = {KC_Q, JP_COLN};
	  uint16_t ngoffkeys[] = {KC_Q, KC_W};
	  set_naginata(_NAGINATA, ngonkeys, ngoffkeys);

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
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  if (record->event.pressed) {
	#ifdef SSD1306OLED
		set_keylog(keycode, record);
	#endif
	// set_timelog();
  }

  switch (keycode) {
    case EISU:
         if (record->event.pressed) {
           // 薙刀式
           naginata_off();
           // 薙刀式
         }
         return false;
         break;
    case KANA2:
         if (record->event.pressed) {
           // 薙刀式
           naginata_on();
           // 薙刀式
    }

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

	default:
	  break;

  }//end of switch


  if (!process_naginata(keycode, record))
        return false;

  return true;
}
