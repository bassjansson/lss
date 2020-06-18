CC       = gcc
CXX      = g++
CCFLAGS  = -Wall
CXXFLAGS = -Wall # -Wno-undefined-var-template
LDFLAGS  =
LDLIBS   = -lportaudio -lsndfile # -pthread -lwiringPi  -lfftw3f

TARGET  = engine
OBJECTS = Main.o porttime.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $(LDFLAGS) $(OBJECTS) $(LDLIBS)
	mv $(TARGET) ../
	rm $(OBJECTS)

.cpp.o:
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm ../$(TARGET)
	rm $(OBJECTS)

touch:
	touch ./*.cpp
	touch ./*.h
