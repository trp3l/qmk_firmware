#define LAYOUT_wrapper(...)   LAYOUT(__VA_ARGS__)

// common
#define ____ KC_TRNS
#define XXXX KC_NO
#define RST RESET
#define VD KC__VOLDOWN
#define VU KC__VOLUP

// layer
#define L_SPC LT(_LOWER, KC_SPC)
#define R_ENT LT(_RAISE, KC_ENT)

// shift_t
#define S_EN LSFT_T(KC_LANG2)
#define S_JA LSFT_T(KC_LANG1)

// cmd_t
#define G_F LCMD_T(KC_F)
#define G_J RCMD_T(KC_J)

// ctl_t
#define C_G LCTL_T(KC_G)
#define C_H RCTL_T(KC_H)
#define C_BS LCTL_T(KC_BSPC)

// alt_t
#define A_D ALT_T(KC_D)
#define A_K ALT_T(KC_K)

// cmd+shift_t
#define GS_S SCMD_T(KC_S)
#define GS_L SCMD_T(KC_L)

#define __QWERTY_L1__ KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T
#define __QWERTY_L2__ KC_TAB, KC_A, GS_S, A_D, G_F, C_G
#define __QWERTY_L3__ KC_LEFT, KC_Z, KC_X, KC_C, KC_V, KC_B

#define __QWERTY_R1__ KC_Y, KC_U, KC_I, KC_O, KC_P, KC_MINS
#define __QWERTY_R2__ C_H, G_J, A_K, GS_L, KC_SCLN, KC_ESC 
#define __QWERTY_R3__ KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RGHT

#define __QUERTY_T3__ KC_UP, S_EN, L_SPC, R_ENT, S_JA, KC_DOWN
#define __QUERTY_T4__ KC_UP, S_EN, L_SPC, C_BS, C_BS, R_ENT, S_JA, KC_DOWN

#define __RAISE_L1__ ____,KC_BSLS,KC_EXLM,KC_AMPR,KC_PIPE,____
#define __RAISE_L2__ ____,KC_HASH,KC_GRV,KC_DQT,KC_QUOT,KC_TILD
#define __RAISE_L3__ ____,____,____,KC_LCBR,KC_LBRC,KC_LPRN

#define __RAISE_R1__ XXXX,KC_EQL,KC_PLUS,KC_ASTR,KC_PERC,KC_MINS
#define __RAISE_R2__ KC_LEFT,KC_DOWN,KC_UP,KC_RGHT,KC_DLR,____
#define __RAISE_R3__ KC_RPRN,KC_RBRC,KC_RCBR,KC_AT,KC_CIRC,____

#define __RAISE_T3__ ____,____,KC_BSPC,____,____,RESET
#define __RAISE_T4__ ____,____,KC_BSPC,____,____,____,____, RESET

/*
#define __LOWER_L1__ ____,____,____,____,____,____
#define __LOWER_L2__ ____,____,____,____,____,____
#define __LOWER_L3__ ____,____,____,____,____,____

#define __LOWER_R1__ ____,____,____,____,____,____
#define __LOWER_R2__ ____,____,____,____,____,____
#define __LOWER_R3__ ____,____,____,____,____,____

#define __LOWER_T3__ ____,____,____,____,____,____
#define __LOWER_T4__ ____,____,____,____,____,____,____,____
*/

#define __LOWER_L1__ ____,LCTL(KC_1),LCTL(KC_2),LCTL(KC_3),LCTL(KC_4),LCTL(KC_5)
#define __LOWER_L2__ ____,KC_1,KC_2,KC_3,KC_4,KC_5
#define __LOWER_L3__ ____,____,____,____,____,____

#define __LOWER_R1__ XXXX,KC_EQL,KC_PLUS,KC_ASTR,KC_PERC,KC_MINS
#define __LOWER_R2__ ____,KC_6,KC_7,KC_8,KC_9,KC_0
#define __LOWER_R3__ ____,____,____,____,____,____

#define __LOWER_T3__ RESET,____,____,KC_DEL,____,____
#define __LOWER_T4__ RESET,____,____,____,____,KC_DEL,____,____

#define __TRANSx6__ ____,____,____,____,____,____
#define __TRANSx4__ ____,____,____,____
#define __TRANSx2__ ____,____
