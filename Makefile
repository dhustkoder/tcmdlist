
BIN=tcmdlist
LDFLAGS=-lpthread

$(BIN): main.cpp
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@ 

