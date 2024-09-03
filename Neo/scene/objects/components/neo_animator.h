#ifndef NEO_ANIMATOR_H
#define NEO_ANIMATOR_H

#include "neo_armature.h"

#include <Neo/OpenGL/keyframe_data.h>


class NeoAnimator
{
public:
    NeoAnimator(NeoArmature *t_target);

    void RunAnimation();
    void SetKeyframes(QMap<int, KeyframeData*> l_Keyframes);

    void UpdateCurrentKeyframe();


private:
    bool m_IsRunning = false;

    NeoArmature *p_TargetArmature = nullptr;

    QMap<int, KeyframeData*> m_CurrentKeyframes = {};

    QVector<int> m_Times = {};
    int m_CurrentFrame = 0;
    int m_Length = 0;

};

#endif // NEO_ANIMATOR_H
