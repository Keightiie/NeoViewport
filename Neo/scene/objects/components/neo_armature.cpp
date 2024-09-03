#include "neo_armature.h"

NeoArmature::NeoArmature() {}

void NeoArmature::AddBone(int t_Id, QString m_name, QVector3D t_PosAbso, QVector3D t_rotation, int m_Parent)
{
    if(t_Id > 1023 || t_Id < 0) return;

    if(m_Bones[t_Id] != nullptr) delete m_Bones[t_Id];
    m_Bones[t_Id] = new BoneData(t_Id, t_PosAbso, t_rotation, &m_BoneMatrices[t_Id]);
    m_Bones[t_Id]->SetName(m_name);

    if(m_Parent != -1)
    {
        if(m_Bones[m_Parent] != nullptr)
        {
            m_Bones[m_Parent]->AppendChild(m_Bones[t_Id]);
        }
    }
}

BoneData *NeoArmature::GetBone(QString m_name)
{
    for(int i = 0; i < 511; i++)
    {
        if(m_Bones[i] != nullptr)
        {
            if(m_Bones[i]->GetName() == m_name) return m_Bones[i];
        }
    }
    return nullptr;
}

BoneData *NeoArmature::GetBone(int t_id)
{
    return m_Bones[t_id];
}

QStringList NeoArmature::GetBoneNames()
{
    QStringList l_BoneNames = {};
    for(int i = 0; i < 511; i++)
    {
        if(m_Bones[i] != nullptr)
        {
            l_BoneNames.append(m_Bones[i]->GetName());
        }
    }
    return l_BoneNames;
}
