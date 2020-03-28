X = g++
CXXFLAGS = -Wall -I/usr/include/eigen3/

proj1: Scene.o main.cpp
	$(CXX) $(CXXFLAGS) Scene.o main.cpp -o proj1
  
Scene.o: Scene.cpp Scene.h
	$(CXX) $(CXXFLAGS) -c Scene.cpp

clean:
	rm *.o*
	rm *~ 

run:
	./proj1