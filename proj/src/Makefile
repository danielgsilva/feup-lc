# name of the program (Minix service)
PROG=proj 

# source code files to be compiled
SRCS = proj.c video.c utils.c timer.c claustro.c kbc.c kbd.c action.c sprite.c mouse.c rtc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
