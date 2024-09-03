#include "neo_animator.h"

NeoAnimator::NeoAnimator(NeoArmature *t_target)
{
    p_TargetArmature = t_target;

}

void NeoAnimator::RunAnimation()
{
    m_CurrentFrame += 1;
    if(m_CurrentFrame > m_Length) m_CurrentFrame = 0;

    UpdateCurrentKeyframe();

}

void NeoAnimator::SetKeyframes(QMap<int, KeyframeData *> l_Keyframes)
{
    m_CurrentKeyframes = l_Keyframes;
    m_Times = m_CurrentKeyframes.keys().toVector();
    std::sort(m_Times.begin(), m_Times.end());


    m_Length = m_Times.isEmpty() ? 0 : m_Times.last();
}

void NeoAnimator::UpdateCurrentKeyframe()
{

    int l_KeyPrevious = -1;
    int l_KeyUpcoming = -1;

    for (int i = 0; i < m_Times.size(); ++i)
    {
        if (m_Times[i] < m_CurrentFrame)
        {
            l_KeyPrevious = m_Times[i];
        }
        else if (m_Times[i] > m_CurrentFrame)
        {
            l_KeyUpcoming = m_Times[i];
            break;
        }
    }

    KeyframeData *l_Current = m_CurrentKeyframes[l_KeyPrevious];

    if(l_Current == nullptr) return;

    for(QString t_boneName : l_Current->GetBoneNames())
    {
        BoneData *t_bone = p_TargetArmature->GetBone(t_boneName);

        if(t_bone != nullptr)
        {
            t_bone->GetTransform()->SetPosition(l_Current->GetPosition(t_boneName));
            t_bone->GetTransform()->SetRotation(l_Current->GetRotation(t_boneName));
            t_bone->UpdateMatrix();
        }
    }


}
