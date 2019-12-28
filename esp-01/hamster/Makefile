MAKEFILESDIR := ../Makefiles

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/devhost/settings
include $(MAKEFILESDIR)/devhost/common

all: $(OBJS)
	make -C $(MAKEFILESDIR)/devhost -f common all
	$(CXX) $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o hamster.devhost

check:
	cppcheck --language=c++ --enable=all --template=gcc $(wildcard *.cpp)

clean:
	find /tmp/build/   -type f -print -delete
	find . -name "*.o" -type f -print -delete

print:
	$(info INCLUDE is $(INCLUDE))
	$(info OBJS is $(OBJS))
	$(info CXXOBJS is $(CXXOBJS))
	$(info COBJS is $(COBJS))

