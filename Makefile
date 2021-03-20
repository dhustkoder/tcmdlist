
BIN=tcmdlist
LDFLAGS=-lpthread
CXXFLAGS=-std=c++14 -O3 -Wall

$(BIN): main.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@ 

