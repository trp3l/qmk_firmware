# QMKのビルドシステムに対し、標準的なprintf実装を使うよう指示します
PRINTF_LIB = printf
COMMON_VPATH += $(LIB_PATH)/printf
SERIAL_DRIVER = vendor