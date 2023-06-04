#ifndef __PointCloud_hpp__
#define __PointCloud_hpp__

#include "Primitives.hpp"
#include "Slab.hpp"
#include "Common.hpp"
#include "GlVis.h"
#include "AxisAlignedBox3.hpp"
#include "PointKDTree.hpp"
#include <vector>
#include <random>

// forward decls
class PointKDTree;

/** A set of 3D points. */
class PointCloud
{

public: 
    std::vector<Point> points;
    AxisAlignedBox3 bbox;


    /** Default constructor. */
    PointCloud() {}

    PointCloud(vector<Point> const & points_);
    PointCloud(vector<Vector3d> const & positions, std::vector<Vector3d> const & normals);

    /** Get the number of points. */
    long size() const { return (long)points.size(); }

    /** Check if the point cloud is empty. */
    bool isEmpty() const { return points.empty(); }

    /** Reset the point cloud to an empty state. */
    void clear() { points.clear(); }

    
    void draw(Scene & scene) {

        // shader
        ourShader.use();

        // camera projection
        Frustum frustum1 = scene.frustum;
        glm::mat4 projection = glm::perspective(frustum1.FoV, frustum1.aspect, frustum1.nearClippingDistance, frustum1.farClippingDistance);
        ourShader.setMat4("projection", projection);

        // camera view
        glm::mat4 view = scene.camera->GetViewMatrix();
        ourShader.setMat4("view", view);

        // pcloud's own transform
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);

        // draw
        glBindVertexArray(bufs.VAO);
        glPointSize(3.0f);
        glDrawArrays(GL_POINTS, 0, last_viewed_point);
    }

    void updateView() {
        if (!isVisInitialized) {
            data = (float*)calloc(2'000'000 * 8, sizeof(float));
            data_allocated_size = 2'000'000 * 8;
            glGenVertexArrays(1, &bufs.VAO);
            glGenBuffers(1, &bufs.VBO);
        }

        // add data
        glBindVertexArray(bufs.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, bufs.VBO);
        if (points.size() * 8 > data_allocated_size) {
            cout << "out of memory!" << endl;
            exit(-1);
        }
        for (int i = last_viewed_point; i < points.size(); ++i) {
            const Point& pt = points[i];
            for (int j = 0; j < 3; ++j) {
                data[9 * i + j] = pt.position[j];
            }
            for (int j = 0; j < 3; ++j) {
                data[9 * i + 3 + j] = pt.normal[j];
            }
            data[9*i + 6] = 1.0f; data[9*i+7] = 0.0f; data[9*i+8] = 0.0f;
        }
        last_viewed_point = points.size();
        glBufferData(GL_ARRAY_BUFFER, last_viewed_point*9*sizeof(float), data, GL_STATIC_DRAW);


        if (!isVisInitialized) {
            //positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            //colors
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            isVisInitialized = true;
        }


    }
    /** Add a point to the point cloud. */
    void addPoint(Point const& p) { 
        points.push_back(p); 
        bbox.AddPoint(p.position); 
    }

    /** Add a point, specified by its position and normal, to the point cloud. */
    void addPoint(Vector3d const& p, Vector3d const& n) { points.push_back(Point(p, n)); bbox.AddPoint(p); }

    /** Get the bounding box of the point cloud. */
    /* TODO : implement AABB */
    AxisAlignedBox3 const & getAABB() const { return bbox; }

    bool load(std::string const & path);
    bool save(std::string const & path) const;

    /**
     * Estimate the plane best fitting the largest subset of the points, using RANSAC. Ignore all points for which
     * Point::isEnabled() returns false.
     *
     * @param num_iters Number of random triplets to consider.
     * @param slab_thickness Twice the distance between the central plane and the slab surface.
     * @param min_points The minimum number of enabled points matching a plane for the latter to be considered a candidate.
     * @param slab The slab of the computed plane, if one is found [return value].
     * @param slab_points The points in the returned slab [return value].
     *
     * @return The number of enabled points matching the best plane, if one was found, else 0.
     */
    long ransac(long num_iters, double slab_thickness, long min_points, OUT Slab & slab,  OUT std::vector<Point *> & slab_points) const;

    /** Try to extract \a num_planes planes using RANSAC and return the number of planes actually found. */
    long ransacMultiple(long num_planes, long num_iters, double slab_thickness, long min_points, std::vector<Slab> & slabs) const;

    /** Adaptively resample the point cloud to reduce the number of points in smooth areas. Don't introduce any new points. */
    void adaptiveDownsample(std::vector<Slab> const & slabs);

  private:
    /** Recompute the bounding box. */
    void recomputeAABB();

    /* Visualization */
    Buffers bufs{};
    Shader ourShader;
    static const char USE_COLOR3 = 1;
    static const char USE_NORMALS = 1 << 1;
    char draw_mode;
    bool isVisInitialized = false;
    long last_viewed_point = 0;
    float* data;
    long data_allocated_size = 0;
}; // class PointCloud

#endif
