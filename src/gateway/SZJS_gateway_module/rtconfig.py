# bsp name
BSP = 'stm32f20x'

# toolchains
EXEC_PATH 	= 'D:/MentorGraphics/Sourcery_CodeBench_Lite_for_ARM_EABI/bin'
PREFIX = 'arm-none-eabi-'
CC = PREFIX + 'gcc'
CXX = PREFIX + 'g++'
AS = PREFIX + 'gcc'
AR = PREFIX + 'ar'
LINK = PREFIX + 'gcc'
TARGET_EXT = 'so'
SIZE = PREFIX + 'size'
OBJDUMP = PREFIX + 'objdump'
OBJCPY = PREFIX + 'objcopy'

DEVICE = ' -mcpu=cortex-m3'
CFLAGS = DEVICE + ' -mthumb -mlong-calls -Dsourcerygxx -O0 -fPIC'
AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp'
LFLAGS = DEVICE + ' -mthumb -Wl,-z,max-page-size=0x4 -shared -fPIC -e main -nostdlib'

CPATH = ''
LPATH = ''
