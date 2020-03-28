#include "Scene.h"
#include <cmath>
#include "math.h"

using namespace std;

//sets origin and direction based on pixel's location in image
Ray::Ray(double i, double j, Camera camera) {
  double d = 1;
  //double d = abs(camera.from.z() - camera.at.z());
  //double h = 2*tan(camera.angle/2)/d;
  double h = (camera.top - camera.bottom);
  double x = camera.left + (j * h + h/2)/camera.resx*d; //I think we divide by resx?
  //double y = camera.left + (i * h + h/2)/camera.resy; //ditto for resy
  double y = camera.top - (i * h + h/2)/camera.resy*d;
  //double y = camera.bottom + (i * h + h/2)/camera.resy;
  double z = -1;
  Eigen::Vector3d s = x*camera.u + y*camera.v + z*camera.w;
  origin = camera.from;
  direction = (s).normalized();
}

Camera::Camera() {}

//sets camera parameters and calculates vectors u, v, and w
Camera::Camera(Eigen::Vector3d fromInput, Eigen::Vector3d atInput, Eigen::Vector3d upInput, double angleInput, double hitherInput, int resxInput, int resyInput) {
  from = fromInput;
  at = atInput;
  up = upInput;
  angle = angleInput;
  hither = hitherInput;
  resx = resxInput;
  resy = resyInput;
  d = 1;
  aspectRatio = resx/resy;
  left = -tan(angle/2)/d;
  right = tan(angle/2)/d;
  bottom = -tan(angle/2)/d/aspectRatio;
  top = tan(angle/2)/d/aspectRatio;
  w = (from - at).normalized();
  u = (up.cross(w)).normalized();
  v = (w.cross(u)).normalized();
  //aspect ratio is resx / resy
}

//parses file based on passed in filename
Scene::Scene(string filename) {
  //parse input from file whose name is given as argument
  ifstream inputFile;
  inputFile.open(filename);
  string backgroundColorChar;
  getline(inputFile, backgroundColorChar, ' ');
  inputFile >> backgroundColor.red;
  inputFile >> backgroundColor.green;
  inputFile >> backgroundColor.blue;
  inputFile.ignore(200, '\n');
  
  string viewingChar;
  getline(inputFile, viewingChar, '\n');
    
  string fromString;
  getline(inputFile, fromString, ' ');
  double x, y, z;
  inputFile >> x;
  inputFile >> y;
  inputFile >> z;
  Eigen::Vector3d from;
  from = Eigen::Vector3d(x, y, z);

  string atString;
  
  getline(inputFile, atString, ' ');
  
  inputFile >> x;
  inputFile >> y;
  inputFile >> z;

  Eigen::Vector3d at;
  at = Eigen::Vector3d(x, y, z);
  string upString;
  getline(inputFile, upString, ' ');
  
  inputFile >> x;
  inputFile >> y;
  inputFile >> z;
  Eigen::Vector3d up;
  up = Eigen::Vector3d(x, y, z);
    
  string angleString;
  getline(inputFile, angleString, ' ');
  
  double angleInput;
  double angle;
  inputFile >> angleInput;
  angle = angleInput * M_PI / 180;

  string hitherString;
  getline(inputFile, hitherString, ' ');
  
  double hither;
  inputFile >> hither;

  string resString;
  getline(inputFile, resString, ' ');
  
  int resx, resy;
  inputFile >> resx;
  inputFile >> resy;
  inputFile.ignore(200, '\n');
  string shapeString;
  getline(inputFile, shapeString, ' ');
  int unusedArg1, unusedArg2, unusedArg3;
  
  while (shapeString == "l") {
    inputFile >> unusedArg1;
    inputFile >> unusedArg2;
    inputFile >> unusedArg3;
    inputFile.ignore(200, '\n');
    getline(inputFile, shapeString, ' ');
  } 
  
  Color currColor;
  inputFile >> currColor.red;
  inputFile >> currColor.green;
  inputFile >> currColor.blue;
  inputFile.ignore(200, '\n');
  getline(inputFile, shapeString, ' ');
  Camera newCamera(from, at, up, angle, hither, resx, resy);
  camera = newCamera;

  while ((shapeString == "p" || shapeString == "s" || shapeString == "f") && !inputFile.eof()) {
    if (shapeString == "p") {
      int numVertices;
      inputFile >> numVertices;
      std::vector<Eigen::Vector3d> vertices;
      for (int i=0; i < numVertices; i++) {
        double x, y, z;
        inputFile >> x;
        inputFile >> y;
        inputFile >> z;
        Eigen::Vector3d newVertex(x, y, z);
        vertices.push_back(newVertex);
      }
      Polygon newPolygon(vertices);
      newPolygon.color = currColor;
      shapes.push_back(newPolygon);
      inputFile.ignore(200, '\n');
      getline(inputFile, shapeString, ' ');
    }
    else if (shapeString == "f") {
      inputFile >> currColor.red;
      inputFile >> currColor.green;
      inputFile >> currColor.blue;
      inputFile.ignore(200, '\n');
    }
  }
}


// Performs ray tracing by going through each pixel and determining whether a ray from the eye
// to that pixel intersects with any of the given shapes
void Scene::RayTrace() {
  for (int i=0; i < camera.resy; i++) { //rows
    vector<array<double,3>> row;
    for (int j=0; j < camera.resx; j++) { //columns
      Ray r(i, j, camera);
      Color c = backgroundColor;
      double t0 = 0;
      double t1 = numeric_limits<double>::infinity();
      HitRecord *hr = new HitRecord();
      hr->c = backgroundColor;
      for (int k=0; k < (int) shapes.size(); k++) {
        if(shapes[k].intersect(r, t0, t1, hr)) {
          c = hr->c;
          t1 = hr->t;
        }
      }
      array<double,3> pixelColor = {c.red * 255, c.green * 255, c.blue * 255};
      row.push_back(pixelColor);
    }
    pixels.push_back(row);
  }
}

//writes calculated vector of pixels to a PPM file
void Scene::WriteToPPM() {
  unsigned char pixelArray[3];
  FILE *f = fopen("hide.ppm","wb");
  fprintf(f, "P6\n%d %d\n%d\n", camera.resx, camera.resy, 255);
   for(int i = 0; i < camera.resy; ++i) {
     for(int j = 0; j < camera.resy; ++j) {
       pixelArray[0] = (unsigned char) pixels[i][j][0];
       pixelArray[1] = (unsigned char) pixels[i][j][1];
       pixelArray[2] = (unsigned char) pixels[i][j][2];
       fwrite(pixelArray, 1, 3, f);
     }
  }
  fclose(f);
}

Shape::Shape() {}

// determines whether or not the given ray intersects with the triangle at the specified interval (t0, t1)
// returns true if so, else returns false
bool Triangle::intersect(const Ray &r, double t0, double t1, HitRecord *hr) {
  Eigen::Matrix<double, 3, 3> M;
  M << (r.direction).x(), (vertex1-vertex2).x(), (vertex1-vertex3).x(), 
  (r.direction).y(), (vertex1-vertex2).y(), (vertex1-vertex3).y(),
  (r.direction).z(), (vertex1-vertex2).z(), (vertex1-vertex3).z();

  Eigen::Vector3d multipliedMatrix((vertex1-r.origin).x(), (vertex1-r.origin).y(), (vertex1-r.origin).z());
  Eigen::Matrix<double, 3, 3> mInverse;
  mInverse = M.inverse();
  Eigen::Vector3d x;
  x = mInverse * multipliedMatrix;
  double t = x.x();
  double beta = x.y();
  double gamma = x.z();

  if (t < t0 || t > t1) {
    return false;
  }
  if (gamma < 0 || gamma > 1) {
    return false;
  }
  if (beta < 0 || beta + gamma > 1) {
    return false;
  }
  hr->t = t;
  return true;
}

//determines whether a convex polygon intersects with a triangle. This is based on whether or not
bool Polygon::intersect(const Ray &r, double t0, double t1, HitRecord *hr) {
  bool hit = false;
  for (int i=0; i < (int) triangles.size(); i++) {
    if (triangles[i].intersect(r, t0, t1, hr)) {
      //cout << "Intersects: \n" << r.origin << r.direction << endl;
      t1 = hr->t;
      hr->c = color;
      hit = true;
    }
  }
  return hit;
}

// constructor for Polygon derived class, populates vertices vector based on the input vector and 
// creates a fan of triangles based on that
Polygon::Polygon(std::vector<Eigen::Vector3d> inputVertices) {
  //populates vertices vector
    for (int i=0; i < (int) inputVertices.size(); i++) {
      vertices.push_back(inputVertices[i]);
    }
  //populates triangles vector
    Eigen::Vector3d baseVertex = vertices[0];
    for (int i=1; i < (int) vertices.size() - 1; i++) {
      Triangle newTriangle = Triangle(baseVertex, vertices[i], vertices[i+1]);
      triangles.push_back(newTriangle);
    }
  }

//triangle constructor, populates vertices based on the input arguments
Triangle::Triangle(Eigen::Vector3d firstVertex, Eigen::Vector3d secondVertex, Eigen::Vector3d thirdVertex) {
  vertex1 = firstVertex;
  vertex2 = secondVertex;
  vertex3 = thirdVertex;
}

//overloaded operator (for debugging purposes)
ostream& operator<<(ostream& os, const Scene& scene) {
  for (int i=0; i < (int) scene.shapes.size(); i++) {
    Polygon currShape = scene.shapes[i];
    os << i << "\n";
    for (int i=0; i < (int) currShape.vertices.size(); i++) {
      os << currShape.vertices[i] << "\n";
    }
  }
  return os;
}

//overloaded operator (for debugging purposes)
ostream& operator<<(ostream& os, const Polygon& polygon) {
    for (int i=0; i < (int) polygon.vertices.size(); i++) {
      os << polygon.vertices[i] << "\n";
    }
  return os;
}

//overloaded operator (for debugging purposes)
ostream& operator<<(ostream& os, const Triangle& triangle) {
  os << triangle.vertex1 << "\n";
  os << triangle.vertex2 << "\n";
  os << triangle.vertex3 << "\n";
  return os;
}