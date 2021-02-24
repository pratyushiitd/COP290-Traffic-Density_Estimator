CC = g++
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS  = -g -Wall
TARGET = cam
BUILD_DIR = ./outputs
all: clean
	mkdir $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -o ./$(BUILD)/$(TARGET) $(TARGET).cpp -std=c++11 `pkg-config --cflags --libs opencv`
	cd $(BUILD_DIR)
	./cam
clean:
	$(RM) -r $(BUILD_DIR)