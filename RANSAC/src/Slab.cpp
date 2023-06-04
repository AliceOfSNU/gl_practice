#include "Slab.hpp"

double ccw(pdd a, pdd b, pdd c) {
    return a.first * b.second + b.first * c.second + c.first * a.second - a.first * c.second - c.first * b.second - b.first * a.second;
}

pdd sub(pdd a, pdd b) {
    return make_pair(a.first - b.first, a.second - b.second);
}

bool
Slab::ContainsPoint(Vector3d const& p)
{
    /* for testing */
    // for each point, check if contained in the aabb.

    //update transform matrix
    Vector3d u = corners[1] - corners[0];
    Vector3d v = corners[3] - corners[0];
    Vector3d up = plane.normal * thickness;


    InverseTransformScaled = Matrix3d();
    InverseTransformScaled.col(0) << u;
    InverseTransformScaled.col(1) << v;
    InverseTransformScaled.col(2) << up;

    Matrix3d TR = InverseTransformScaled.inverse();
    InverseTransformScaled = TR;

    Vector3d base_point = corners[0] - 0.5 * thickness * plane.normal;
    Vector3d transformed = InverseTransformScaled * (p-base_point);

    bool result = true;
    for (int i = 0; i < 3; ++i) {
        result &= (transformed[i] <= 1.0) && (transformed[i]) >= 0.0;
    }
    return result;
}


bool
Slab::Intersects(AxisAlignedBox3 const & box)
{
    // extract the vertices.
    Vector3d m_corners[8];

    // do translation and rotation
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 2; ++j) {
            m_corners[i + 4 * j] = corners[i];
        }
    }
   
    for (int i = 0; i < 4; ++i) {
        m_corners[i]  -= 0.5 * thickness * plane.normal;
        m_corners[4 + i] += 0.5 * thickness * plane.normal;
    }

    //extract corners of the aabb box.
    const Vector3d& high = box.high;
    const Vector3d& low = box.low;

    Vector3d box_corners[8]{
        low, Vector3d(high[0], low[1],low[2]), Vector3d(high[0], low[1],high[2]), Vector3d(low[0], low[1], high[2]),
        Vector3d(low[0], high[1],low[2]), Vector3d(high[0], high[1], low[2]), high,  Vector3d(low[0], high[1], high[2]),
    };

    // aabb contains this slab's point?
    for (int i = 0; i < 8; ++i) {
        if(box.ContainsPoint(m_corners[i])) return true;
    }
    
    // well than does this slab contains the box's point?
    for (int i = 0; i < 8; ++i) {
        if (ContainsPoint(box_corners[i])) return true;

    }
    

    // aabb's axis intersects with the planes?
    for (int ax = 0; ax < 3; ++ax) {

        for (const int* facei : box_faces_indices) {
            Vector3d u = m_corners[facei[1]] - m_corners[facei[0]]; //local x edge
            Vector3d v = m_corners[facei[3]] - m_corners[facei[0]]; //local z edge
            Vector3d n = u.cross(v);

            pdd proj[4];
            for (int i = 0; i < 4; ++i) {
                Vector3d pt = m_corners[facei[i]];
                if (ax == 0) {
                    proj[i].first = pt[1]; proj[i].second = pt[2];
                }
                else if (ax == 1) {
                    proj[i].first = pt[0]; proj[i].second = pt[2];
                }
                else {
                    proj[i].first = pt[0]; proj[i].second = pt[1];
                }
            }

            for (int j = 0; j < 4; j++) {
                //for the four edges of the aabb parallel to ax,
                const int* edge = box_edges_indices[4 * ax + j];
                Vector3d p1 = box_corners[edge[0]] - m_corners[facei[0]];
                Vector3d p2 = box_corners[edge[1]] - m_corners[facei[0]];
                if (n.dot(p1) * n.dot(p2) > 0) continue; //this 

                p1 = box_corners[edge[0]];
                pdd pt; //project to orhogonal plane to the current ax.
                if (ax == 0) {
                    pt.first = p1[1]; pt.second = p1[2];
                }
                else if (ax == 1) {
                    pt.first = p1[0]; pt.second = p1[2];
                }
                else {
                    pt.first = p1[0]; pt.second = p1[1];
                }

                //check if pt lies in the parallelogram 'proj',
                //in which case we are done.
                if ((ccw(proj[0], proj[1], pt) * ccw(proj[2], proj[3], pt) >= 0) &&
                    ((ccw(proj[1], proj[2], pt) * ccw(proj[3], proj[0], pt) >= 0))) return true;

            }

        }
    }

    // slab method to check if sides of the slab intersects the aabb.
    // for each of 12 edges
    double EPSILON = 1.0E-8;

    for (const int* edgei : box_edges_indices) {
        Vector3d& p1 = m_corners[edgei[0]];
        Vector3d& p2 = m_corners[edgei[1]];
        Vector3d rays[]{ p2 - p1, p1 - p2 }; //forward and backward tests.

        bool cross_flag = true;
        for (const Vector3d& ray : rays) {
            double dmax = 9.0E+18; double dmin = -9.0E+18;

            for (int ax = 0; ax < 3; ++ax) {
                if (ray[ax] < EPSILON) {
                    // ray parallel to ax: 
                    for (int i = 0; i < 3; ++i) {
                        if (i == ax) continue;
                        if (p1[i] > high[i] || p1[i] < low[i]) return false; // misses!
                    }
                }
                // ray non parallel
                double t1 = (high[ax] - p1[ax]) / ray[ax]; //bounded division.
                double t2 = (low[ax] - p1[ax]) / ray[ax];
                if (t1 < t2) {
                    dmax = min(dmax, t2); dmin = max(dmin, t1);
                }
                else {
                    dmax = min(dmax, t1); dmin = max(dmin, t2);
                }
            }

            if (dmax < dmin || dmax < 0) {
                cross_flag = false;
                break;
            }
        }
        if (cross_flag) return true; //if at least one segment touches the box, it's true!
    }

    return false; //default
}

void
Slab::updateCorners(std::vector<Point*> const& points, Vector3d u, Vector3d v)
{
    static int const NUM_ROTS = 64;

    if (points.empty())
        return;

    if (points.size() == 1)
    {
        if (points[0]->enabled)
            corners[0] = corners[1] = corners[2] = corners[3] = points[0]->position;

        return;
    }

    Vector3d c = plane.SampleOnePoint();
    Vector3d n = plane.normal;
    double min_u = -2.0; double max_u = 2.0;
    double min_v = -2.0; double max_v = 2.0;
    u.normalize(); v.normalize();
    corners[0] = c + min_u * u + min_v * v;
    corners[1] = c + min_u * u + max_v * v;
    corners[2] = c + max_u * u + max_v * v;
    corners[3] = c + max_u * u + min_v * v;
    //double min_area = 0;
    //for (int r = 0; r < NUM_ROTS; ++r)
    //{
    //    double angle = (r / (double)NUM_ROTS) * 2 * PI;
    //    double sin_a = std::sin(angle);
    //    double cos_a = std::cos(angle);
    //
    //    Vector3d rot_u = u * cos_a + sin_a * v;
    //    Vector3d rot_v = cos_a * v - sin_a * u;
    //
    //    double min_u = 0, min_v = 0, max_u = 0, max_v = 0;
    //    for (size_t i = 0; i < points.size(); ++i)
    //    {
    //        if (!points[i]->enabled)
    //            continue;
    //
    //        Vector3d diff = points[i]->position - c;
    //        double proj_u = diff.dot(rot_u);
    //        double proj_v = diff.dot(rot_v);
    //
    //        if (i > 0)
    //        {
    //            min_u = std::min(min_u, proj_u);
    //            min_v = std::min(min_v, proj_v);
    //
    //            max_u = std::max(max_u, proj_u);
    //            max_v = std::max(max_v, proj_v);
    //        }
    //        else
    //        {
    //            min_u = max_u = proj_u;
    //            min_v = max_v = proj_v;
    //        }
    //    }
    //
    //    double area = (max_u - min_u) * (max_v - min_v);
    //    if (r == 0 || area < min_area)
    //    {
    //        min_area = area;
    //        corners[0] = c + min_u * rot_u + min_v * rot_v;
    //        corners[1] = c + min_u * rot_u + max_v * rot_v;
    //        corners[2] = c + max_u * rot_u + max_v * rot_v;
    //        corners[3] = c + max_u * rot_u + min_v * rot_v;
    //
    //    }
    //}

    //update transform matrix
    u = corners[1] - corners[0];
    v = corners[3] - corners[0];
    Vector3d up = plane.normal.normalized() * thickness;
    //these vectors are orthogonal and non-unit;

    InverseTransformScaled.col(0) << u;
    InverseTransformScaled.col(1) << v;
    InverseTransformScaled.col(2) << up;
    Matrix3d TR = InverseTransformScaled.inverse();
    InverseTransformScaled = TR;
}

void Slab::draw(Scene& scene) {
    ourShader.use();
    // camera projection
    Frustum frustum1 = scene.frustum;
    glm::mat4 projection = glm::perspective(frustum1.FoV, frustum1.aspect, frustum1.nearClippingDistance, frustum1.farClippingDistance);
    ourShader.setMat4("projection", projection);

    // camera view
    glm::mat4 view = scene.camera->GetViewMatrix();
    ourShader.setMat4("view", view);

    // pcloud's own transform
    glm::vec3 pos{ position[0], position[1], position[2] };
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, (float)glm::radians(rotation[0]), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, (float)glm::radians(rotation[1]), glm::vec3(0.0, 1.0, 0.0));
    ourShader.setMat4("model", model);

    glBindVertexArray(bufs.VAO);
    glPointSize(3.0f);
    glDrawArrays(GL_POINTS, 0, 8);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs.EBO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, (void*)0);
    
}
void Slab::updateView(){
    if (!isVisInitialized) {
        bufs = Buffers();
        glGenVertexArrays(1, &bufs.VAO);
        glGenBuffers(1, &bufs.VBO);

        
        glGenBuffers(1, &bufs.EBO);
        unsigned int order[24]{
            0, 1, 1, 2, 2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            0, 4, 1, 5, 2, 6, 3, 7,
        };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*sizeof(unsigned int), order, GL_STATIC_DRAW);
    }

    float vertices[9 * 8]{ 0 };
    Vector3d m_corners[8];
    Vector3d normal = plane.normal;
    normal.normalize();
    for (int i = 0; i < 4; ++i) {
        m_corners[i] = corners[i] - 0.5 * thickness * normal;
        m_corners[4 + i] = corners[i] + 0.5 * thickness * normal;
    }


    // fill vertex array
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            //position
            vertices[9 * i + j] = m_corners[i][j];
            vertices[9 * (4 + i) + j] = m_corners[4 + i][j];
        }
        for (int j = 0; j < 3; ++j) {
            //color
            if (i == 3) continue; //BLCK
            vertices[9 * (4 + i) + 6 + j] = (i == j) ? 0.5f : 0.0f;
            vertices[9 * i + 6 + j] = (i == j) ? 1.0f : 0.0f;
        }
    }

    // fill buffer
    glBindVertexArray(bufs.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bufs.VBO);
    glBufferData(GL_ARRAY_BUFFER,9 * 8 * sizeof(float), vertices, GL_STATIC_DRAW);


    if (!isVisInitialized) {
        //position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        //normal(not used)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        //color
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        isVisInitialized = true;
    }
}
void
Slab::updateCorners(std::vector<Point> const & points, Vector3d u, Vector3d v)
{
  std::vector<Point *> pp(points.size());
  for (size_t i = 0; i < pp.size(); ++i)
    pp[i] = const_cast<Point *>(&points[i]);  // removing the const is not the greatest thing to do, be careful...

  updateCorners(pp, u, v);
}
