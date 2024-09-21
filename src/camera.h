#ifndef CAMERA_H
#define CAMERA_H
#include <GL/glew.h>
#include <glm/glm.hpp>

struct SceneCameraData {
    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    float heightAngle; // The height angle of the camera in RADIANS

    float aperture;    // Only applicable for depth of field
    float focalLength; // Only applicable for depth of field
};

class camera
{
public:
    camera();
    static glm::mat4 getViewMatrix(const SceneCameraData& cameraMetaData);
    static glm::mat4 getProjMatrix(const SceneCameraData& cameraMetaData, double width, double height, int farPlane, int nearPlane);

    // Struct which contains data for the camera of a scene

};


#endif // CAMERA_H
