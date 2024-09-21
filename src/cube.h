#ifndef CUBE_H
#define CUBE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>




class Cube {

public:
    Cube(glm::mat4 ctm);
    std::vector<float> initialize(int param1, int param2, float texCoord1Top, float texCoord2Top);
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVAO(GLuint &vao);
    GLuint getVAO();

    glm::mat4 getCTM() const;
    std::vector<float> getVertexData();
    void setVertexData();
    void setID(int idNumber);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    int getID();
    glm::vec3 getPosition();


    float texCoord1;
    float texCoord2;
    float texCoord3;
    float texCoord4;

private:
        GLuint VAO, VBO;
        int m_param1;
        int m_param2;
        glm::mat4 transformationMatrix;
        glm::vec3 position;
        std::vector<float> m_vertexData;
        int id;

};



#endif
