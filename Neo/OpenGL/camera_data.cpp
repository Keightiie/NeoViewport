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
    if(m_CamType == CamTypeOrthographic)
    {
        qreal halfWidth = l_OrthographicScale * aspect / 2.0;
        qreal halfHeight = l_OrthographicScale / 2.0;
        m_ProjectionMatrix.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_ClippingNear, m_ClippingFar);
    }
    else
    {
        m_ProjectionMatrix.perspective(m_FieldOfView, aspect, m_ClippingNear, m_ClippingFar);
    }
}

QMatrix4x4 CameraData::GetProjectionMatrix(bool l_caluclate)
{
    if(!l_caluclate) return m_ProjectionMatrix;
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
