
all : sample1 sample2

sample1: sample1.cpp
	g++ $< -o $@

sample2: sample2.cpp
	g++ $< -o $@

