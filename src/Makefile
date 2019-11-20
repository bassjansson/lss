CC       = gcc
CXX      = g++
CCFLAGS  = -Wall
CXXFLAGS = -Wall
LDFLAGS  =

TARGET  = lssapp
OBJECTS = main.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $(OBJECTS)

.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm $(TARGET)
	rm $(OBJECTS)

touch:
	touch ./*.cpp
	touch ./*.h
