#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/LU>

using namespace std;

struct Color {
  double red;
  double green;
  double blue;
};

struct HitRecord {
  float t;
  Color c;
};

class Camera {
  public:
  Camera();
  Camera(Eigen::Vector3d fromInput, Eigen::Vector3d atInput, Eigen::Vector3d upInput, double angleInput, double hitherInput, int resxInput, int resyInput);
  Eigen::Vector3d from;
  Eigen::Vector3d at;
  Eigen::Vector3d up;
  double angle;
  double hither;
  int resx;
  int resy;
  double left;
  double right;
  double top;
  double bottom;
  double d;
  double aspectRatio;
  Eigen::Vector3d u;
  Eigen::Vector3d v;
  Eigen::Vector3d w;
};

struct Ray {
  Ray(double i, double j, Camera camera);
  Eigen::Vector3d origin; //eye
  Eigen::Vector3d direction; //direction
};

class Shape {
  public:
  Shape();
  virtual bool intersect (const Ray &r, double t0, double t1, HitRecord *hr) = 0;
};

class Triangle {
  public:
  Triangle(Eigen::Vector3d firstVertex, Eigen::Vector3d secondVertex, Eigen::Vector3d thirdVertex);
  bool intersect (const Ray &r, double t0, double t1, HitRecord *hr);
  friend ostream& operator<<(ostream& os, const Triangle& triangle);
  private:
  Eigen::Vector3d vertex1;
  Eigen::Vector3d vertex2;
  Eigen::Vector3d vertex3;
};

class Polygon: public Shape {
  public:
  Polygon(std::vector<Eigen::Vector3d> inputVertices);
  bool intersect (const Ray &r, double t0, double t1, HitRecord *hr);
  //write a destructor
  std::vector<Eigen::Vector3d> vertices;
  std::vector<Triangle> triangles;
  friend ostream& operator<<(ostream& os, const Polygon& polygon);
  Color color;
};

class Scene {
  public:
  Scene(string filename);
  void calcIntersections();
  void RayTrace();
  void WriteToPPM();
  friend ostream& operator<<(ostream& os, const Scene& scene);
  Camera camera;
  std::vector<Polygon> shapes;
  Color backgroundColor;
  Eigen::Vector3d xDir;
  Eigen::Vector3d yDir;
  Eigen::Vector3d zDir;
  vector<vector<array<double, 3>>> pixels;
  //vector<int[3]> pixels;
};



#endif