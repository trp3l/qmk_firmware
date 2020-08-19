#この二つをonにしてください。

UNICODE_ENABLE = yes        # Unicode
MOUSEKEY_ENABLE = yes	    # Mouse keys(+4700)


#推奨。一気にコードが軽くなります。

LINK_TIME_OPTIMIZATION_ENABLE = yes

# If you want to change the display of OLED, you need to change here
SRC +=  naginata_v12.c \
	./lib/glcdfont.c \
        ./lib/rgb_state_reader.c \
        ./lib/layer_state_reader.c \
        ./lib/logo_reader.c \
        ./lib/keylogger.c \
        # ./lib/mode_icon_reader.c \
        # ./lib/host_led_state_reader.c \
        # ./lib/timelogger.c \
