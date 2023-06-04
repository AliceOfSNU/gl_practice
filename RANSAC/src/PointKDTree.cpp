#include "PointKDTree.hpp"
#include <stack>
PointKDTree::Node::~Node()
{
  delete lo;
  delete hi;
}

PointKDTree::PointKDTree(std::vector<Point> const & points)
: root(NULL)
{
  build(points);
}

PointKDTree::PointKDTree(std::vector<Point *> const & points)
: root(NULL)
{
  build(points);
}

//PointKDTree::PointKDTree(PointCloud const & pcloud)
//: root(NULL)
//{
//  build(pcloud.points);
//}

void
PointKDTree::build(std::vector<Point> const & points)
{
    std::vector<Point*> pp(points.size());
    for (size_t i = 0; i < pp.size(); ++i)
        pp[i] = const_cast<Point*>(&points[i]);  // removing the const is not the greatest thing to do, be careful...

    build(pp);
}

void
PointKDTree::build(std::vector<Point *> const & points)
{
    srand(100);
    static size_t const MAX_POINTS_PER_LEAF = 5;
    static size_t const MEDIAN_SAMPLING_COUNT = 50;

    Node* curr_node;
    stack<Node*> st;
    
    root = new Node();
    st.push(root);
    for (Point* pt : points) {
        root->bbox.AddPoint(pt->position);
    }
    root->points = points;
    while (!st.empty()) {
        curr_node = st.top(); st.pop();
        if (curr_node->points.size() > MAX_POINTS_PER_LEAF) {
            curr_node->hi = new Node();
            curr_node->lo = new Node();

            //choose axis to divide.
            const Vector3d& hp = curr_node->bbox.high;
            const Vector3d& lp = curr_node->bbox.low;

            int div_ax = 0;
            float max_dim = 0.0f;
            for (int ax = 0; ax < 3; ++ax) {
                if (hp[ax] - lp[ax] > max_dim) {
                    div_ax = ax; max_dim = hp[ax] - lp[ax];
                }
            } //split along the longest axis

            float median = (hp[div_ax] + lp[div_ax]) / 2.0f;
            curr_node->lo->bbox = AxisAlignedBox3(lp, hp);
            curr_node->lo->bbox.high[div_ax] = median;
            curr_node->hi->bbox = AxisAlignedBox3(lp, hp);
            curr_node->hi->bbox.low[div_ax] = median;

            for (Point* pt : curr_node->points) {
                if (pt->position[div_ax] > median) {
                    curr_node->hi->points.push_back(pt);
                }
                else {
                    curr_node->lo->points.push_back(pt);
                }
            }
            curr_node->points.clear();
            st.push(curr_node->hi);
            st.push(curr_node->lo);
        }
        else {
            // leaf node
            // assign any color for visualization
            curr_node->color = Color(rand() / RAND_MAX, rand()/RAND_MAX, rand()/RAND_MAX);
        }
    }

    

    // A kd-tree is just a binary search tree, and is constructed in a near-identical way.
    //
    // - Initially assign (pointers to) all points to the root node.
    // - Recursively subdivide the points, splitting the parent box in half along the longest axis and creating two child nodes
    //   for each split. Stop when number of points in node <= MAX_POINTS_PER_LEAF.
    // - Don't forget to save space by clearing the arrays of points in internal nodes. Only the leaves need to store references
    //   to points.
}

