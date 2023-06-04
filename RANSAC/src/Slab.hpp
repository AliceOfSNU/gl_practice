#ifndef __Slab_hpp__
#define __Slab_hpp__

#include "Primitives.hpp"
#include "AxisAlignedBox3.hpp"
#include "Common.hpp"
#include <vector>
#include "GLVis.h"

class Slab
{
  private:
    Plane plane;
    double thickness;
    Vector3d corners[4];

    /* visualization */
    bool isVisInitialized = false;
    Buffers bufs;
    


  public:
    //these attributes are for manually moving the slab. not needed when constructor defines the coordinates.
    Vector3d position;
    Vector3d rotation;

    Matrix3d InverseTransformScaled; // each side of the slab maps to unit length. 

    Slab() : thickness(0) {}

    Slab(Plane const & plane_, double thickness_) : plane(plane_), thickness(thickness_) {}


    // set manually
    Slab(double width, double height, double thickness_) : thickness(thickness_) {
        corners[0] = Vector3d(-0.5 * width, 0.0, -0.5 * height);
        corners[1] = Vector3d(-0.5 * width, 0.0, 0.5 * height);
        corners[2] = Vector3d(0.5 * width, 0.0, 0.5 * height);
        corners[3] = Vector3d(0.5 * width, 0.0, -0.5 * height);
        plane.dist = 0;
        plane.normal = Vector3d(0, 1.0, 0);

        position.setZero();
        rotation.setZero();
        InverseTransformScaled.setIdentity();
        updateView();
    }

    
    /** Check if the slab contains a point (points lying exactly on the slab are assumed to be contained). */
    bool ContainsPoint(Vector3d const & p);

    /** Check if the slab intersects an axis-aligned bounding box. */
    bool Intersects(AxisAlignedBox3 const & box);

    /** Get a corner of the slab. */
    Vector3d const & getCorner(int i) const
    {
      return corners[i];
    }

    /**
     * Update the corners of the slab to fit the projection of a set of points onto the supporting plane. Only points with
     * Point::isEnabled() = true are considered.
     */
    void updateCorners(std::vector<Point *> const & points, Vector3d u, Vector3d v);

    /**
     * Update the corners of the slab to fit the projection of a set of points onto the supporting plane. Only points with
     * Point::isEnabled() = true are considered.
     */
    void updateCorners(std::vector<Point> const & points, Vector3d u, Vector3d v);

    
    /* Visualization */
    Shader ourShader;

    void updateView();

    void draw(Scene& scene);

}; // class Slab

#endif
