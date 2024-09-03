#ifndef NEO_ARMATURE_H
#define NEO_ARMATURE_H

#include <Neo/OpenGL/bone_data.h>


class NeoArmature
{
public:
    NeoArmature();

    void AddBone(int t_Id, QString m_name, QVector3D t_PosAbso, QVector3D t_PosRela, int m_Parent);

    BoneData *GetBone(QString m_name);
    BoneData *GetBone(int t_id);

    QStringList GetBoneNames();

    BoneData* m_Bones[1024] = {nullptr};
    QMatrix4x4 m_BoneMatrices[1024];
};

#endif // NEO_ARMATURE_H
