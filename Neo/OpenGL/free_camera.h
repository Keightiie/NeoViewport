#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H
#include "camera_data.h"
class FreeCamera : CameraData
{
public:
    FreeCamera();
    void UpdateCamera();

private:
    //QMap<eDebugKeyInputs, bool> m_PressedKeys = {};
};

#endif // FREE_CAMERA_H
