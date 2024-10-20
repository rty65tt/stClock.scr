
VER=0.7

GCC=g++
RC=windres
EXE=build-${VER}.scr
LDFLAGS=-s -O2 -static -mwindows -lgdiplus -lcomctl32  -lcomdlg32 -lscrnsave -static-libgcc -static-libstdc++
CXXFLAGS=-O2 -Wl,-rpath
OBJS=source.o resource.res

.PHONY:all clean resource.res run

all:${EXE}

clean:
	rm -rf ${EXE}
	rm -rf ${OBJS}

%.o: %.cpp
	${GCC} ${CXXFLAGS} -c "$<" -o "$@"

resource.res: resource.rc source.def icon1.ico resource.h
	${RC} -i "$<" -O coff -o "$@"

${EXE}: ${OBJS}
	${GCC} ${OBJS} -o "$@" ${LDFLAGS} 
	${EXE} -s

