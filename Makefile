include settings.mk



OBJS =		

LIBS =		-lpthread -lblas -llapack

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
	
all:	headers $(TARGET) $(TEST_TARGET)

test:	$(TEST_TARGET)

headers:
	rsync -av --exclude "test*/" --exclude "test_settings.h" --include "*/" --include "*.h" --exclude "*" src/ inc/pheet/

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
	rm -f -R inc/pheet/*
