#ifndef __Primitives_hpp__
#define __Primitives_hpp__


#include "Common.hpp"
using Edge = pair<Vector3d, Vector3d>;


/* A 3d point, defined by position and normal. */
struct Point
{
    Vector3d position;
    Vector3d normal;
    mutable bool enabled;

    Point() : enabled(true) {}
    Point(Vector3d const & p, Vector3d const & n = Vector3d::Zero()) : position(p), normal(n), enabled(true) {}
    Point(double x, double y, double z, Vector3d const& n = Vector3d::Zero()):position(Vector3d(x, y, z)), normal(n), enabled(true) {}
    
    
    void Print() {
        cout << "[point] pos: " << position << ", normal: " << normal << endl;
    }
};

/* A 3d plane, defined by its normal(can be in two orientations) and its distance from origin*/
struct Plane {
    Vector3d normal = Vector3d::Zero();
    double dist = 0.0;

    Plane(){}
    Plane(Vector3d const & n, Vector3d const & p) {
        normal = n.normalized();
        dist = normal.dot(p);
    }

    Vector3d SampleOnePoint() {
        return normal * dist;
    }

    void Print() {
        cout << "[plane] normal: " << normal << ", dist: " << dist << endl;
    }

};

// lowest indexes first,
// vec([1]-[0]) x vec([3]-[0]) gives outwards normal
// normals are x, y, z directions
int const box_faces_indices[6][4]{
    {0, 3, 7, 4},
    {1, 5, 6, 2},
    {0, 1, 2, 3},
    {4, 7, 6, 5},
    {0, 4, 5, 1},
    {3, 2, 6, 7},
};

// x, y, z
int const box_edges_indices[12][2]{
    {0, 1}, {3, 2}, {7, 6}, {4, 5},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
    {0, 3}, {1, 2}, {5, 6}, {4, 7},
};




#endif
