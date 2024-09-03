#ifndef KEYFRAME_DATA_H
#define KEYFRAME_DATA_H

#include <QMap>
#include <QVector>
#include <QVector3D>


class KeyframeData
{
public:
    KeyframeData();
    void AddBoneTransform(QString t_boneId, QVector3D t_pos, QVector3D t_rot);

    QVector3D GetPosition(QString t_boneId);
    QVector3D GetRotation(QString t_boneId);
    QStringList GetBoneNames();

private:
    QStringList m_Bones = {};
    QMap<QString, QVector3D> m_Positions = {};
    QMap<QString, QVector3D> m_Rotations = {};
};

#endif // KEYFRAME_DATA_H
