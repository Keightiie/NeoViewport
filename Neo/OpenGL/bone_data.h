#ifndef BONE_DATA_H
#define BONE_DATA_H

#include <QVector>
#include <QMatrix4x4>
#include <QtMath>

#include <Neo/scene/objects/components/neo_transform.h>

class BoneData
{
public:
    BoneData(int l_boneId, QVector3D l_Position, QVector3D l_Rotation, QMatrix4x4 *l_matrix);

    void ParentUpdated(QMatrix4x4 l_Parent);

    void UpdateMatrix();

    void AppendChild(BoneData * l_child);

    void SetName(QString t_name);
    void SetRelativePosition(QVector3D l_Position);

    QQuaternion EulerToQuaternion(const QVector3D& eulerAngles);
    qreal DegreesToRadians(qreal degrees);

    int GetId();
    NeoTransform *GetTransform();
    QVector<BoneData*> GetChildren();
    QMatrix4x4 GetMatrix();
    QString GetName();




private:
    int m_id = -1;
    QString m_Name = "";
    QVector3D m_RelativePosition;
    QVector3D m_Position;
    QVector3D m_Rotation;
    NeoTransform *m_Transform = nullptr;

    QMatrix4x4 *p_RenderMatrix;
    QMatrix4x4 m_ParentMatrix;

    QVector<BoneData*> m_Children = {};
};

#endif // BONE_DATA_H
