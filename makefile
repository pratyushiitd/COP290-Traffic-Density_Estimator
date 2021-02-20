run:
	g++ test.cpp -o testoutput -std=c++11 `pkg-config --cflags --libs opencv`
	./testoutput