#ifndef RESOURCE_UTILS_H
#define RESOURCE_UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "opengl_utils.h"


using namespace std;
VAO* loadSplineControlPoints(string path)
{
	//TODO: load spline control point data and return VAO
	//You can make use of getVAOFromAttribData in opengl_utils.h
    std::ifstream file(path);

    if (true == file.is_open()) {
        std::string s;
        // read number of vertices
        int N;
        file >> N;
        getline(file, s);
        
        // read vertices to the end of file
        
        // store vertex positions from file
        std::vector<float> attrib_data;

        // store the sizes of vertex data
        std::vector<unsigned int> attrib_sizes{ 3 };

        float x, y, z;

        for(int i = 0; i < N; ++i) {
            getline(file, s);
            std::istringstream line(s);
            line >> x >> y >> z;
            attrib_data.push_back(x);
            attrib_data.push_back(y);
            attrib_data.push_back(z);
        }

        // create VAO
        VAO* vao = getVAOFromAttribData(attrib_data, attrib_sizes);

        // clean up
        file.close();
        return vao;
    }
    else {
        std::cout << "spline vertex file open fail" << std::endl;
    }

    return nullptr;
}

VAO* loadBezierSurfaceControlPoints(string path)
{
    std::ifstream file(path);

    if (true == file.is_open()) {
        std::string s;
        // read number of vertices
        int N;
        file >> N;
        cout << "total patches: " << N << std::endl;
        getline(file, s);

        // read vertices to the end of file

        // store vertex positions from file
        std::vector<float> attrib_data;

        // store the sizes of vertex data
        std::vector<unsigned int> attrib_sizes{ 3 };

        float x, y, z;
        int Nx, Ny;

        for (int i = 0; i < N; ++i) {
            getline(file, s);
            std::istringstream line(s);
            line >> Nx >> Ny;
            for (int nx = 0; nx <= Nx; ++nx) {
                for (int ny = 0; ny <= Ny; ++ny) {
                    getline(file, s);
                    std::istringstream line(s);

                    line >> x >> y >> z;
                    attrib_data.push_back(x);
                    attrib_data.push_back(y);
                    attrib_data.push_back(z);
                }
            }
        }

        // create VAO
        VAO* vao = getVAOFromAttribData(attrib_data, attrib_sizes);

        // clean up
        file.close();
        return vao;
    }
    else {
        std::cout << "patch data vertex file open fail" << std::endl;
    }

    return nullptr;
}
#endif