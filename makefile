run:
	g++ test.cpp -o testoutput -std=c++11 `pkg-config --cflags --libs opencv`
	./testoutput
run2:
	g++ test2.cpp -o test2output -std=c++11 `pkg-config --cflags --libs opencv`
	./test2output
all:
	g++ cam.cpp -o cam -std=c++11 `pkg-config --cflags --libs opencv`
	./cam
