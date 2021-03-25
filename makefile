CC = g++
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS  = -g -Wall
TARGET = main
BUILD_DIR = ./build
OUT_DIR = ./outputs
LIB = pkg-config --cflags --libs opencv
.DEFAULT:
	@echo make all to build and run
	@echo make build to build
	@echo make run to run the build
	@echo make clean to remove build files
help:
	@echo make all to build and run
	@echo make build to build
	@echo make run to run the build
	@echo make clean to remove build files
	
all: clean build run plot
clean:
	@echo Removing old build files :
	$(RM) -r $(BUILD_DIR)
	$(RM) *.jpg
build:
	@echo Building project :
	mkdir $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -o $(BUILD_DIR)/$(TARGET) $(TARGET).cpp -std=c++11 `$(LIB)`
	@echo Generated the executable without errors ...
	
run:
	@echo Running the executable ...
	cd $(BUILD_DIR); ./$(TARGET) $(PARAM)

plot:
	python3 plot.py
.PHONY: all
