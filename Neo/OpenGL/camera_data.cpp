#include "camera_data.h"

#include <qmatrix4x4.h>
#include <QOpenGLWidget>

CameraData::CameraData()
{
    m_CompTransform = new NeoTransform();
}

void CameraData::SetCameraType(eCameraType t_Type)
{
    m_CamType = t_Type;
}

void CameraData::SetOrthographicScale(double t_scale)
{
    l_OrthographicScale = t_scale;
}

void CameraData::UpdateMatrix(int t_width, int t_height)
{
    qreal aspect = qreal(t_width) / qreal(t_width ? t_height : 1);
    m_ProjectionMatrix.setToIdentity();
    m_ProjectionMatrix.perspective(m_FieldOfView, aspect, m_ClippingNear, m_ClippingFar);
}

QMatrix4x4 CameraData::GetProjectionMatrix()
{
    return m_ProjectionMatrix * m_CompTransform->GetMatrix();
}

NeoTransform *CameraData::GetTransform()
{
    return m_CompTransform;
}

QMatrix4x4 CameraData::GetTransformMatrix()
{
    return m_CompTransform->GetMatrix();
}

void CameraData::UpdateCamera(float t_width, float t_height)
{
    if(m_CamType == CamTypeOrthographic)
    {
        UpdateCameraOrtho(t_width, t_height);
        return;
    }

    QMatrix4x4 projection;
    projection.perspective(45.0f, float(t_width) / float(t_height), 0.1f, 100.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.constData());
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void CameraData::UpdateCameraOrtho(float t_width, float t_height)
{
    float l_BiggestNumber = t_width;

    if(t_height > t_width)
    {
        l_BiggestNumber = t_width;
    }

    double l_height = t_height / l_BiggestNumber;
    double l_width = t_width / l_BiggestNumber;


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, l_width * l_OrthographicScale, 0, l_height * l_OrthographicScale, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void CameraData::CameraCleanup()
{
    if(m_CamType == CamTypeOrthographic)
    {
        CameraCleanupOrtho();
        return;
    }
}

void CameraData::CameraCleanupOrtho()
{
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_TEXTURE_2D);
}
