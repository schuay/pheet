include settings.mk



OBJS =		

LIBS =		

TARGET =	lib/libpheet.o

TEST_OBJS = 

TEST_LIBS =	

TEST_TARGET = bin/pheet_test

include src/sub.mk

lib/%.o : src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(TARGET):	$(OBJS)
	$(CXX) -c $(TARGET) $(OBJS) $(LIBS)

$(TEST_TARGET):	$(TEST_OBJS) $(OBJS)
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJS) $(OBJS) $(TEST_LIBS)
	
all:	$(TEST_TARGET)

test:	$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
