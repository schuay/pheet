include settings.mk

LIBS =		-lpthread -lhwloc

TEST_OBJS_HOST =
TEST_OBJS_MIC =  

TEST_TARGET_HOST = bin/pheet_test
TEST_TARGET_MIC = bin_mic/pheet_test

include test/sub.mk

lib/%.o : test/%.cpp
	$(CXX) -c $(CXXFLAGS_HOST) $(INCLUDE_PATH_HOST) -o $@ $<
 
lib_mic/%.o : test/%.cpp
	$(CXX) -c -mmic $(CXXFLAGS_MIC) $(INCLUDE_PATH_MIC) -o $@ $<


$(TEST_TARGET_HOST):	$(TEST_OBJS_HOST)
	$(CXX) -o $(TEST_TARGET_HOST) $(TEST_OBJS_HOST) $(LIB_PATH_HOST) $(LIBS) $(TEST_LIBS)

$(TEST_TARGET_MIC):	$(TEST_OBJS_MIC)
	$(CXX) -mmic -o $(TEST_TARGET_MIC) $(TEST_OBJS_MIC) $(LIB_PATH_MIC) $(LIBS) $(TEST_LIBS)
  	
all:	$(TEST_TARGET_HOST) $(TEST_TARGET_MIC)

host:	$(TEST_TARGET_HOST)

mic: $(TEST_TARGET_MIC)

test:	$(TEST_TARGET_HOST) $(TEST_TARGET_MIC)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS_HOST) $(TEST_OBJS_MIC) $(TEST_TARGET_HOST) $(TEST_TARGET_MIC)
	rm -f -R inc/pheet/*
