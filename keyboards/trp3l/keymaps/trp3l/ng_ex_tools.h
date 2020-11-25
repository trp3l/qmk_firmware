//mod の検出
bool is_mod_user(uint16_t keycode);
void mods_count_user(uint16_t keycode, keyrecord_t *record);
int get_mods_user(void);
//x 前回押したキー の保存用
uint16_t get_keycode_prev(void);
uint16_t get_pressed_time_prev(void);
void set_keycode_prev(uint16_t keycode, uint16_t time);
//x キーを指定時間以内に離した時の判定が長ったらしいので分離した。 第二変数に指定したい時間を書いてね。
//ex if(is_released_within_time(keycode 300))
bool is_released_within_time( uint16_t keycode, uint16_t tapping_tarm);
bool is_double_tapped_within_time( uint16_t keycode, uint16_t tapping_tarm);
bool get_double_tap_state(void);
/*
bool process_ng_combo(uint16_t keycode, keyrecord_t *record);
void set_ngcombos(uint8_t ng_layer, uint16_t *onk, uint16_t *offk, uint8_t s_layer, uint16_t *s_on, uint16_t *s_off);
*/
#ifdef SAMEHAND_SHFT
	bool samehand_shft(int nt, uint32_t keycomb_buf);
#endif
#ifdef NG_AUTO_REPEAT
	bool auto_repeat_keycode(uint16_t keycode, keyrecord_t *record);
	void auto_repeat_matrix_scan(void);
#endif
