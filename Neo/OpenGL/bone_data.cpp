#include "bone_data.h"
#include <QtMath>

BoneData::BoneData(int l_boneId, QVector3D l_Position, QVector3D l_rotation, QMatrix4x4 *l_matrix)
{
    m_Transform = new NeoTransform;
    m_id = l_boneId;
    m_Position = l_Position;
    m_Rotation = l_rotation;
    p_RenderMatrix = l_matrix;
}

void BoneData::AppendChild(BoneData *l_child)
{
    m_Children.append(l_child);
}

void BoneData::SetRelativePosition(QVector3D l_Position)
{
    m_RelativePosition.setX(l_Position.x() / 32);
    m_RelativePosition.setY(l_Position.y() / 32);
    m_RelativePosition.setZ(l_Position.z() / 32);
}

QQuaternion BoneData::EulerToQuaternion(const QVector3D &eulerAngles)
{
    qreal rollRad = DegreesToRadians(eulerAngles.x());
    qreal pitchRad = DegreesToRadians(eulerAngles.y());
    qreal yawRad = DegreesToRadians(eulerAngles.z());

    return QQuaternion::fromEulerAngles(QVector3D(rollRad, pitchRad, yawRad));
}

QMatrix4x4 BoneData::GetMatrix()
{
    return *p_RenderMatrix;
}


void BoneData::ParentUpdated(QMatrix4x4 l_Parent)
{
    m_ParentMatrix = l_Parent;
    //m_ParentMatrix.translate(m_RelativePosition);
    UpdateMatrix();
}

void BoneData::UpdateMatrix()
{
    *p_RenderMatrix = m_ParentMatrix;

    QQuaternion mNewRotation = QQuaternion::fromEulerAngles(m_Transform->GetRotation());
    //QQuaternion yAxisRotation = QQuaternion::fromEulerAngles(m_Rotation.x(), m_Rotation.y(), m_Rotation.z()); // 90 degrees on the Y axis
    //QQuaternion InverseyAxisRotation = QQuaternion::fromEulerAngles(-m_Rotation.x(), -m_Rotation.y(), -m_Rotation.z());



    //Set Bind POsitions
    p_RenderMatrix->translate(m_Position);

    //Update
    //p_RenderMatrix->rotate(yAxisRotation);
    p_RenderMatrix->rotate(mNewRotation);
    //p_RenderMatrix->rotate(InverseyAxisRotation);

    p_RenderMatrix->translate(m_Transform->GetPosition());

    //Reset Bind Position
    p_RenderMatrix->translate(-m_Position);


    for(BoneData * l_child : m_Children)
    {
        l_child->ParentUpdated(*p_RenderMatrix);
    }
}

QVector<BoneData *> BoneData::GetChildren()
{
    QVector<BoneData*> l_returnData;

    for(BoneData * l_child : m_Children)
    {
        l_returnData.append(l_child);
        l_returnData.append(l_child->GetChildren());
    }

    return l_returnData;
}

void BoneData::SetName(QString t_name)
{
    m_Name = t_name;
}

QString BoneData::GetName()
{
    return m_Name;
}

int BoneData::GetId()
{
    return m_id;
}

NeoTransform *BoneData::GetTransform()
{
    return m_Transform;
}
qreal BoneData::DegreesToRadians(qreal degrees) {
    return degrees * M_PI / 180.0;
}
