#ifndef __PointKDTree_hpp__
#define __PointKDTree_hpp__

#include "Common.hpp"
#include "Primitives.hpp"
//#include "PointCloud.hpp"
#include "AxisAlignedBox3.hpp"
#include "Slab.hpp"

/* A kd-tree on points. */
class PointKDTree
{
    PointKDTree(const PointKDTree&) = delete;
    PointKDTree& operator= (const PointKDTree&) = delete;

private:
    /** A node of the kd-tree. */
    struct Node
    {
        /** Constructor. */
        Node() : lo(NULL), hi(NULL) {}

        /** Destructor. */
        ~Node();

        AxisAlignedBox3 bbox;         ///< Bounding box of the node.
        Node* lo;                    ///< Child of the node with smaller coordinates.
        Node* hi;                    ///< Child of the node with larger coordinates.
        vector<Point*> points;  ///< Set of points in the node (empty unless leaf node).

        //visualization. used for leaf nodes only.
        Color color;

    };


    /** Construct from a set of points. The points must last as long this kd-tree does. */
    void build(std::vector<Point *> const & points);

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    void build(std::vector<Point> const & points);

    template <typename RangeT>
    void rangeQueryRecursive(const Node* node, RangeT& range, OUT std::vector<Point*>& points_in_range) const {

        //   - If node->lo && node->lo->bbox intersects range, rangeQuery(node->lo, range, points_in_range)
        //   - If node->hi && node->hi->bbox intersects range, rangeQuery(node->hi, range, points_in_range)
        if (!range.Intersects(node->bbox)) return; // exclusive.
        if (node->hi == NULL && node->lo == NULL) {
            // leaf node - check for all points.
            for (Point* pt : node->points) {
                if (range.ContainsPoint(pt->position)) points_in_range.push_back(pt);
            }
            return;
        }
        if (node->lo) rangeQueryRecursive(node->lo, range, points_in_range);
        if (node->hi) rangeQueryRecursive(node->hi, range, points_in_range);
    }


  public:
    /** The root node of the kd-tree. */
    Node* root;

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    PointKDTree(std::vector<Point> const & points);

    /** Construct from a set of points. The points must last as long this kd-tree does. */
    PointKDTree(std::vector<Point *> const & points);

    /** Construct from a point cloud. The point cloud must last as long this kd-tree does. */
    //PointKDTree(PointCloud const & pcloud);

    /** Destructor. */
    ~PointKDTree() { clear(); }

    /** Reset the tree to an empty state. */
    void clear() { delete root; root = NULL; }

    void Print(Node* n) {
        if (n->points.size() > 0) {
            cout << "[KDTree Node] " << endl;
            n->bbox.Print();
            for (Point* pt : n->points) {
                pt->Print();
            }

            return;
        }
        if (n->lo) {
            Print(n->lo);
        }
        if (n->hi) {
            Print(n->hi);

        }
    }
    /**
     * Do a range query, returning the set of points within a query range. The range type RangeT must support the following
     * functions:
     *
     *   bool ContainsPoint(Vector3d const & p) const;              // check if range contains a point
     *   bool Intersects(AxisAlignedBox3 const & box) const;  // check if range intersects bounding box (of kd-tree node)
     *
     * The AxisAlignedBox3 class already has these functions and can be used for testing.
     *
     * @param range The query range.
     * @param points_in_range Used to return the set of points within the query range.
     */
    template <typename RangeT>
    void rangeQuery(RangeT& range, OUT std::vector<Point*>& points_in_range) const
    {
        points_in_range.clear();
        rangeQueryRecursive(root, range, points_in_range);
    }

}; // class PointKDTree

#endif
