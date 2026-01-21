
# Build Options
#   change to "no" to disable the options, or define them in the Makefile in
#   the appropriate keymap folder that will get included automatically
#
#BOOTMAGIC_ENABLE = no       # Virtual DIP switch configuration(+1000)
#MOUSEKEY_ENABLE = no        # Mouse keys(+4700)
#EXTRAKEY_ENABLE = yes        # Audio control and System control(+450)
#CONSOLE_ENABLE = no         # Console for debug(+400)
#COMMAND_ENABLE = no         # Commands for debug and configuration
#NKRO_ENABLE = no            # Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
MIDI_ENABLE = no            # MIDI controls
AUDIO_ENABLE = no           # Audio output on port C6
UNICODE_ENABLE = no         # Unicode
BLUETOOTH_ENABLE = no       # Enable Bluetooth with the Adafruit EZ-Key HID
RGBLIGHT_ENABLE = no       # Enable WS2812 RGB underlight.
SWAP_HANDS_ENABLE = no      # Enable one-hand typing
LTO_ENABLE = yes
COMBO_ENABLE = no
TAP_DANCE_ENABLE = yes
ENCODER_ENABLE = yes

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no    # Breathing sleep LED during USB suspend

# If you want to change the display of OLED, you need to change here


# SPLIT_TRANSPORT = custom にすると
# quantum/split_common/  の transport.c, transactions.c
#	platforms/avr/drivers/ の i2c_master.c, i2c_slave.c, serial.c
# が読み込まれなくなるため、common_features.mk を参考にリロードする。

#SPLIT_TRANSPORT = custom

SRC +=  naginata_v13.c \
	ng_ex_tools.c
