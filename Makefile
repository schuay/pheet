include settings.mk



OBJS =		

LIBS =		

TARGET =	bin/runtime

TEST_OBJS = 

TEST_LIBS =	

TEST_TARGET = bin/runtime_test

include src/sub.mk

lib/%.o : src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

$(TEST_TARGET):	$(TEST_OBJS)
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJS) $(TEST_LIBS)
	
all:	$(TEST_TARGET)

test:	$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
