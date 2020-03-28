This assignment was the first project, a ray-tracing project in which an image was generated 
based on the shapes in a scene and the position of the camera. We were given the details of a scene,
such as the background color and the shapes contained within the scene, via an nff file, which we parsed
and used to populate our geometries. My program uses an abstract Shape class which contains two derived 
classes: Polygon and Triangle. The Polygon class was populated directly by the nff file and the Triangle
class was used as a way to calculate the intersection between a ray and a polygon (create a fan of triangles
and then calculate the intersection for each individual triangle and take the one with the smallest t). Each
Polygon class had a vector of triangles which were all contained within the polygon. The ray-triangle
intersection formula was used to calcukate the intersection between a ray and a triangle. The color of an
individual pixel was determined by the color of the shape it intersected with. The array of colored pixels 
generated populated a PPM file that was created in order to hold the resulting image.

How to run: the program takes in one command line argument, the name of the file. As such, to run it, one
must type "./proj1 <filename>"

I have not used any sources for help outside of the textbook and the instructor.

