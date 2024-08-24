#ifndef NEO_TRANSFORM_H
#define NEO_TRANSFORM_H

#include <QVector3D>
#include <QMatrix4x4>


class NeoTransform
{
public:
    NeoTransform();

    void TranslatePosition(QVector3D t_translation);
    void TranslateRotation(QVector3D t_translation);
    void SetPosition(QVector3D t_translation);
    void SetRotation(QVector3D t_translation);

    QVector3D GetPosition();
    QVector3D GetRotation();

    QMatrix4x4 GetMatrix();

private:
    QVector3D m_Position;
    QVector3D m_Rotation;
};

#endif // NEO_TRANSFORM_H
