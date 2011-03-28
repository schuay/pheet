include settings.mk

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		runtime.o

LIBS =

TARGET =	runtime

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
