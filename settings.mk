
OPTIMIZATION = -O3
#OPTIMIZATION = -O0 -g
CXXFLAGS_HOST = $(OPTIMIZATION) -Wall -fmessage-length=0 -std=c++11
CXXFLAGS_MIC = $(OPTIMIZATION) -Wall -fmessage-length=0 -std=c++11
CXX = icpc

LIB_PATH_HOST = -L../hwloc_host/src/.libs
LIB_PATH_MIC = -L../hwloc_mic/src/.libs

INCLUDE_PATH_HOST = -I. -I../hwloc_host/include -I../boost
INCLUDE_PATH_MIC = -I. -I../hwloc_mic/include -I../boost

TEST_LIBS = 
# Needed for lupiv test
#TEST_LIBS =	$(LIBS)  -lblas -llapack

