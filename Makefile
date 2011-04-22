include settings.mk



OBJS =		

LIBS =		-lpthread

TARGET =	lib/libpheet.o

TEST_OBJS = 

TEST_LIBS =	$(LIBS)

TEST_TARGET = bin/pheet_test

include src/sub.mk

lib/%.o : src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

#$(TARGET):	$(OBJS)
#	$(CXX) -c $(TARGET) $(OBJS) $(LIBS)

$(TARGET):	$(OBJS)
	ld -r -o $(TARGET) $(OBJS)

$(TEST_TARGET):	$(TEST_OBJS) $(OBJS) $(TARGET)
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJS) $(TARGET) $(TEST_LIBS)
	
all:	$(TARGET) $(TEST_TARGET)

test:	$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
