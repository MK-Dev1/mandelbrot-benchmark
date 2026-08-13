CXX = g++

CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pthread
LDLIBS = -lpng

TARGET = mandelbrot

SOURCES = src/mandelbrot.cpp \
          src/config.cpp \
          src/renderline.cpp \
          src/color.cpp \
          src/saveimage.cpp

OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
