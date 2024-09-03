#include "keyframe_data.h"

KeyframeData::KeyframeData() {}

void KeyframeData::AddBoneTransform(QString t_boneId, QVector3D t_pos, QVector3D t_rot)
{
    m_Bones.append(t_boneId);
    m_Positions[t_boneId] = t_pos;
    m_Rotations[t_boneId] = t_rot;
}

QVector3D KeyframeData::GetPosition(QString t_boneId)
{
    return m_Positions[t_boneId];
}

QVector3D KeyframeData::GetRotation(QString t_boneId)
{
    return m_Rotations[t_boneId];
}

QStringList KeyframeData::GetBoneNames()
{
    return m_Bones;
}


