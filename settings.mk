
OPTIMIZATION = -O3
CXXFLAGS = $(OPTIMIZATION) -Wall -fmessage-length=0 -std=c++0x -I.
CXX = clang++

TEST_LIBS = 
# Needed for lupiv test
#TEST_LIBS =	$(LIBS)  -lblas -llapack

