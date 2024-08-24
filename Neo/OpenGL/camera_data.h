#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include "qmatrix4x4.h"

#include <Neo/scene/objects/components/neo_transform.h>
enum eCameraType
{
    CamTypePerspective,
    CamTypeOrthographic
};

class CameraData
{
public:
    CameraData();

    void SetCameraType(eCameraType t_Type);
    void SetOrthographicScale(double t_scale);

    void UpdateMatrix(int t_width, int t_height);
    QMatrix4x4 GetProjectionMatrix();
    NeoTransform *GetTransform();
    QMatrix4x4 GetTransformMatrix();

    void UpdateCamera(float t_width, float t_height);
    void UpdateCameraOrtho(float t_width, float t_height);

    void CameraCleanup();
    void CameraCleanupOrtho();

private:
    double l_OrthographicScale = 6.0;
    eCameraType m_CamType = CamTypePerspective;
    QMatrix4x4 m_ProjectionMatrix;
    NeoTransform *m_CompTransform = nullptr;

    //Perspective Variables
    double m_FieldOfView = 45.0f;
    double m_ClippingNear = 0.5f;
    double m_ClippingFar = 100.0f;
};

#endif // CAMERA_DATA_H
