LIB_ORGOOGLE="-lglog -lortools -lrestbed -lJsonBox"
LIB_PATH="lib/"
INCLUDE_PATH="include/"

#PROGRAM=myknapsack.cpp
PROGRAM=maerskApi.cpp

g++ $PROGRAM -fPIC -std=c++11 -O4 \
	-I $INCLUDE_PATH \
	-L $LIB_PATH \
	$LIB_ORGOOGLE \
	-lpthread  \
	-o maerskApi

