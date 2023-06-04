#include "PointCloud.hpp"

#include <fstream>
#include <sstream>


PointCloud::PointCloud(std::vector<Point> const & points_)
: points(points_)
{
  recomputeAABB();
}

PointCloud::PointCloud(std::vector<Vector3d> const & positions, std::vector<Vector3d> const & normals)
{

  for (size_t i = 0; i < positions.size(); ++i)
    points.push_back(Point(positions[i], normals[i]));

  recomputeAABB();
}

bool
PointCloud::load(std::string const & path)
{
  // Simple format: Each line is either
  //   x y z
  //    OR
  //   x y z nx ny nz
  //
  // where (nx, ny, nz) is the normal

    std::ifstream in(path.c_str());
    if (!in)
    {
        cout << "Could not open file for reading: " << path;
        return false;
    }

    std::string line;
    while (getline(in, line))
    {
        // Skip empty lines
        if (line.empty())
            continue;

        std::istringstream line_in(line);
        Vector3d p;
        if (!(line_in >> p[0] >> p[1] >> p[2]))
        {
            return false;
        }

        // Normal is optional
        Vector3d n;
        if (!(line_in >> n[0] >> n[1] >> n[2]))  // doesn't properly handle malformed lines, but we'll ignore this for now
            n = Vector3d::Zero();

        points.push_back(Point(p, n));
    }

      recomputeAABB();

      return true;
}

bool
PointCloud::save(std::string const & path) const
{
  std::ofstream out(path.c_str(), std::ios::binary);
  if (!out)
  {
    return false;
  }

  for (size_t i = 0; i < points.size(); ++i)
  {
      Vector3d const& p = points[i].position;
      Vector3d const& n = points[i].normal;
      out << p[0] << ' ' << p[1] << ' ' << p[2] << ' ' << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
  }

  return true;
}

//Graphics::Shader *
//createPointShader(Graphics::RenderSystem & rs)
//{
//  static std::string const VERTEX_SHADER =
//"void main()\n"
//"{\n"
//"  gl_Position = ftransform();\n"
//"  gl_FrontColor = gl_Color;\n"
//"  gl_BackColor = gl_Color;\n"
//"}\n";
//
//  static std::string const FRAGMENT_SHADER =
//"void main()\n"
//"{\n"
//"  gl_FragColor = gl_Color;\n"
//"}\n";
//
//  Graphics::Shader * shader = rs.createShader("Point Graphics::Shader");
//  if (!shader)
//    throw Error("Could not create point shader");
//
//  // Will throw errors on failure
//  shader->attachModuleFromString(Graphics::Shader::ModuleType::VERTEX, VERTEX_SHADER.c_str());
//  shader->attachModuleFromString(Graphics::Shader::ModuleType::FRAGMENT, FRAGMENT_SHADER.c_str());
//
//  return shader;
//}



void
PointCloud::recomputeAABB()
{
    bbox.SetNull();

    for (size_t i = 0; i < points.size(); ++i)
        bbox.AddPoint(points[i].position);
}

long
PointCloud::ransac(long num_iters, double slab_thickness, long min_points, OUT Slab & slab, OUT std::vector<Point *> & slab_points) const
{

    //   - Construct a kd-tree on the enabled points (remember to build the kd-tree with pointers to existing points -- you
    //     shouldn't be copying the points themselves, either explicitly or implicitly).
    //   - Generate num_iters random triplets of enabled points and fit a plane to them.
    //   - Using the kd-tree, see how many other enabled points are contained in the slab supported by this plane with thickness
    //     slab_thickness (extends to distance 0.5 * slab_thickness on each side of the plane).
    //   - If this number is >= min_points and > the previous maximum, the plane is the current best fit. Set the 'slab' argument
    //     to be the slab for this plane, and update slab_points to be the set of (enabled) matching points for this plane.
    //   - At the end, for visualization purposes, update the corners of the best slab using its set of matching points, and
    //     return the number of (enabled) matching points.

    static PointKDTree kdtree(points);
    long count = 0;
    for (int i = 0; i < num_iters; ++i) {
        // for max iters
        // sample three points
        Vector3d pts[3];
        for (int j = 0; j < 3; ++j) {
            long idx = points.size() * rand() / RAND_MAX;
            pts[j] = points[idx].position;
        }
        Vector3d u = pts[1] - pts[0];
        Vector3d v = pts[2] - pts[0];
        v -= u.normalized().dot(v) * u;
        Vector3d n = u.cross(v).normalized();

        Plane pl(n, pts[0]); // construct plane by normal and a contained point.
        slab = Slab(pl, slab_thickness);
        slab.updateCorners(points, u, v); // should write this...
        kdtree.rangeQuery(slab, slab_points);
        cout << "contains " <<  slab_points.size() <<"points" << endl;

    }
    return 0;
}

long
PointCloud::ransacMultiple(long num_planes, long num_iters, double slab_thickness, long min_points, vector<Slab> & slabs)
const
{
    for (size_t i = 0; i < points.size(); ++i)
        points[i].enabled = true;

      slabs.clear();
      for (long i = 0; i < num_planes; ++i)
      {
        Slab slab;
        std::vector<Point *> slab_points;

        long num_matching_pts = ransac(num_iters, slab_thickness, min_points, slab, slab_points);
        if (num_matching_pts <= 0)
          break;

        slabs.push_back(slab);

        // Don't consider these points in subsequent slabs
        for (size_t j = 0; j < slab_points.size(); ++j)
            slab_points[j]->enabled = false;
      }

  return (long)slabs.size();
}

void
PointCloud::adaptiveDownsample(std::vector<Slab> const & slabs)
{
  // TODO
}
