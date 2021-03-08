CC = g++
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS  = -g -Wall
TARGET = cam
BUILD_DIR = ./outputs
TEMP=main
LIB = pkg-config --cflags --libs opencv
.DEFAULT:
	@echo make help to see commands
help: 
	@echo make all to build and run
	@echo make build to build
	@echo make run to run the build
	@echo make clean to remove build files
all: clean build run
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
	cd $(BUILD_DIR); ./$(TARGET)
test:
	$(CC) $(CPPFLAGS) -o $(BUILD_DIR)/$(TEMP) $(TEMP).cpp -std=c++11 `$(LIB)`
	cd $(BUILD_DIR); ./$(TEMP)
check:
	pip3 install -r requirements.txt
	python3 plot.py
.PHONY: test
