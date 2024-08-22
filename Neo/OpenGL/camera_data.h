#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

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

    void UpdateCamera(float t_width, float t_height);
    void UpdateCameraOrtho(float t_width, float t_height);

    void CameraCleanup();
    void CameraCleanupOrtho();

private:
    double l_OrthographicScale = 6.0;
    eCameraType m_CamType = CamTypePerspective;
};

#endif // CAMERA_DATA_H
