all:
	g++ -o main main.cpp `pkg-config --cflags --libs opencv`
