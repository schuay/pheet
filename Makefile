include settings.mk

LIBS =		-lpthread -lhwloc

TEST_OBJS = 

TEST_LIBS =	$(LIBS)  -lblas -llapack

TEST_TARGET = bin/pheet_test

include test/sub.mk

lib/%.o : test/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<


$(TEST_TARGET):	$(TEST_OBJS)
	$(CXX) -o $(TEST_TARGET) $(TEST_OBJS) $(TEST_LIBS)
	
all:	$(TEST_TARGET)

test:	$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
	rm -f -R inc/pheet/*
