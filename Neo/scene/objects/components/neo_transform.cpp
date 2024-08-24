#include "neo_transform.h"

NeoTransform::NeoTransform() {}

void NeoTransform::TranslatePosition(QVector3D t_translation)
{
    m_Position += t_translation;
}

void NeoTransform::TranslateRotation(QVector3D t_translation)
{
    m_Rotation += t_translation;
}

void NeoTransform::SetPosition(QVector3D t_translation)
{
    m_Position = t_translation;
}

void NeoTransform::SetRotation(QVector3D t_translation)
{
    m_Rotation = t_translation;
}

QVector3D NeoTransform::GetPosition()
{
    return m_Position;
}

QVector3D NeoTransform::GetRotation()
{
    return m_Rotation;
}

QMatrix4x4 NeoTransform::GetMatrix()
{
    QMatrix4x4 l_ReturnMatrix;
    l_ReturnMatrix.setToIdentity();
    l_ReturnMatrix.rotate(m_Rotation.z(), 0.0f, 0.0f, 1.0f);
    l_ReturnMatrix.rotate(m_Rotation.x(), 1.0f, 0.0f, 0.0f);
    l_ReturnMatrix.rotate(m_Rotation.y(), 0.0f, 1.0f, 0.0f);
    l_ReturnMatrix.translate(m_Position);
    return l_ReturnMatrix;
}
