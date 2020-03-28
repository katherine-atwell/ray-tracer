#include <iostream>
#include <fstream>
#include "Scene.h"

using namespace std;
int main(int argc, char *argv[]) {
  Scene newScene(argv[1]);
  
  /*std::vector<Polygon> shapes;
  for (int i=0; i < (int) (newScene.shapes).size(); i++) {
    shapes.push_back(newScene.shapes[i]);
  }*/
  
  newScene.RayTrace();
  newScene.WriteToPPM();
  return 0;
}