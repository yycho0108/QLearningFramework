CC = g++
FLAGS = -std=c++11 -O3
LIBS = -L/usr/lib
INCS = -I/usr/include -I./include

BUILD = build
SRC = src


#SOURCES = $(wildcard $(SRC)/*.cpp)
#OBJECTS = $(pathsubst $(SRC)/%.cpp, $(BUILD)/%.o, $(SOURCES))
#RAW = Q \
#	  State \
#	  Action

#SOURCES = $(addprefix $(SRC)/, $(addsuffix .cpp, RAW))
#OBJECTS = $(addprefix $(BUILD)/,$(addsuffix .o, RAW))

#RAW_BUILD = $(addsuffix .o, RAW)

#RAW_SRC = Q.cpp \
#	   State.cpp \
#	   Action.cpp
#RAW_BUILD = $(RAW_SRC:.cpp=.o)

#SOURCES = $(SRC)/Q.cpp \
#		  $(SRC)/State.cpp \
#		  $(SRC)/Action.cpp

OBJECTS = $(BUILD)/Q.o \
		  $(BUILD)/State.o \
		  $(BUILD)/Action.o \
		  $(BUILD)/Utility.o

TARGET = $(BUILD)/main

all: dir $(TARGET)

dir:
	echo $(BUILD)
	mkdir -p $(BUILD)

$(BUILD)/Q.o: $(SRC)/Q.cpp $(BUILD)/State.o $(BUILD)/Action.o $(BUILD)/Utility.o
$(BUILD)/State.o: $(SRC)/State.cpp $(BUILD)/Action.o
$(BUILD)/Action.o: $(SRC)/Action.cpp
$(BUILD)/Utility.o: $(SRC)/Utility.cpp

$(TARGET) : $(OBJECTS) $(SRC)/main.cpp
	$(CC) $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
$(OBJECTS):
	$(CC) -c $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
clean:
	rm $(BUILD)/main build/*.o
#$(BUILD)/Q.o $(BUILD)/State.o $(BUILD)/Action.o:
#	$(CC) $(FLAGS) $(INCS) $^ -o $@ $(LIBS)


#$(TARGET) : Q.o State.o Action.o $(TARGET).cpp
#	$(CC) $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
#Q.o: Q.cpp State.o Action.o
#	$(CC) -c $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
#State.o: State.cpp Action.o
#	$(CC) -c $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
#Action.o: Action.cpp
#	$(CC) -c $(FLAGS) $(INCS) $^ -o $@ $(LIBS)
