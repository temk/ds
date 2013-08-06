#
#
#
SRCS	= ${wildcard src/*.cpp}
OBJS	= ${SRCS:.cpp=.o}

LIB	= ../lib/libds-core.a
DLL	= ../lib/libds-core.so

CXXFLAGS = -fPIC -O2 -I include

TARGET	= ${LIB} ${DLL}
BOOST	= 

.PHONY: ${TARGET}
 
${DLL}: ${OBJS}
	${CC} -shared $< -o $@


clean:
	${RM} -f ${TARGET} ${OBJS}

