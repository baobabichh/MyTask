CC=g++
CFLAGS= -c --std=c++17  -O3
LDFLAGS= -pthread   
SOURCES=ConsoleApplication30.cpp MYLIB.a
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=run

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 
