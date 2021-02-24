CC = g++
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS  = -g -Wall
TARGET = cam
BUILD_DIR = ./outputs
LIB = pkg-config --cflags --libs opencv

all: clean build run
clean:
	@echo Removing build files
	$(RM) -r $(BUILD_DIR)
build:
	mkdir $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -o $(BUILD_DIR)/$(TARGET) $(TARGET).cpp -std=c++11 `$(LIB)`
run:
	cd $(BUILD_DIR); ./$(TARGET)
	