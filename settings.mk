
OPTIMIZATION = -O3
#OPTIMIZATION = -O0 -g
CXXFLAGS = $(OPTIMIZATION) -Wall -fmessage-length=0 -std=c++11
CXX = g++-4.7

LIB_PATH =
INCLUDE_PATH = -I.

TEST_LIBS = 
# Needed for lupiv test
#TEST_LIBS =	$(LIBS)  -lblas -llapack

# Flags for Intel Xeon Phi
CXXFLAGS_MIC = $(OPTIMIZATION) -Wall -fmessage-length=0 -std=c++11
LIB_PATH_MIC = 
INCLUDE_PATH_MIC = -I.

