include settings.mk



OBJS =		lib/test/runtime_tests.o lib/test/Test.o lib/test/sorting/SortingTests.o lib/test/sorting/Reference/ReferenceSTLSort.o

LIBS =		

TARGET =	bin/runtime_test

lib/%.o : src/%.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
