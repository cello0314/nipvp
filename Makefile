TARGET = nipvp
OBJS = main.o

CFLAGS = -O2 -Os -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -std=gnu99
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1

USE_PSPSDK_LIBC = 1
USE_PSPSDK_LIBS = 1

LIBS = -lpspsystemctrl_kernel

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
