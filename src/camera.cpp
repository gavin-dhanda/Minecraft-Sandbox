#include "camera.h"

camera::camera()
{

}

glm::mat4 camera::getViewMatrix(const SceneCameraData& cameraMetaData) {

    glm::vec3 upMetaData = glm::vec3(cameraMetaData.up);
    glm::vec3 pos = cameraMetaData.pos;
    glm::vec3 cameraFront = glm::normalize(-cameraMetaData.look); // w
    glm::vec3 cameraRight = glm::normalize(upMetaData-glm::dot(upMetaData,cameraFront)*cameraFront); // v
    glm::vec3 cameraUp = glm::cross(cameraRight, cameraFront); // u

    glm::mat4 rotationMatrix(
        cameraUp.x, cameraRight.x, cameraFront.x, 0.0f,
        cameraUp.y, cameraRight.y, cameraFront.y, 0.0f,
        cameraUp.z, cameraRight.z, cameraFront.z, 0.0f,
        0.0f,0.0f,0.0f, 1.0f
        );

    glm::mat4 translationMatrix(
        1.0f,0.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,0.0f,
        0.0f,0.0f,1.0f,0.0f,
        -pos.x, -pos.y,-pos.z, 1.0f
        );

    glm::mat4 viewMatrix = rotationMatrix*translationMatrix;

    return viewMatrix;
}

glm::mat4 camera::getProjMatrix(const SceneCameraData& cameraMetaData, double width, double height, int farPlane, int nearPlane){

    // calculate relevant variables for constructing matrices
    float heightAngle = cameraMetaData.heightAngle;
    float aspectRatio = (width/height);
    float widthAngle = aspectRatio*heightAngle;
    float far = farPlane;
    float near = nearPlane;
    float scaleHeight = 1.0f / (tan(heightAngle / 2.0f) * far);
    float scaleWidth = 1.0f / (tan(widthAngle / 2.0f) * far);

    // create scale parallel and clip matrices and multiply them.
    glm::mat4 scaleMatrix = {
        scaleWidth, 0.0f,0.0f,0.0f,
        0.0f,scaleHeight,0.0f,0.0f,
        0.0f,0.0f,1.0f/far,0.0f,
        0.0f,0.0f,0.0f,1.0f
    };

    float cValue = -near/far;
    glm::mat4 parallelMatrix = {

        1.0f,0.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,0.0f,
        0.0f,0.0f,(1/(1+cValue)),-1.0,
        0.0f,0.0f,-cValue/(1+cValue),0.0f
    };

    glm::mat4 clippingMatrix = {
        1.0f,0.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,0.0f,
        0.0f,0.0f,-2.0f,0.0f,
        0.0f,0.0f,-1.0f,1.0f
    };

    // return proj matrix
    glm::mat4 projectionMatrix = clippingMatrix*parallelMatrix*scaleMatrix;
    return projectionMatrix;

}


