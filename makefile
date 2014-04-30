all:
	g++ -o main main.cpp `pkg-config --cflags --libs opencv`
	clang GetWindowID.m -o GetWindowID -framework CoreGraphics -framework Cocoa
