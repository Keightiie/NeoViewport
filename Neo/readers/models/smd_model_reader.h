#ifndef SMD_MODEL_READER_H
#define SMD_MODEL_READER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QVector2D>
#include <QtMath>
#include <QVector3D>

#include <Neo/OpenGL/keyframe_data.h>
#include <Neo/OpenGL/mesh_data.h>

#include <Neo/scene/scene_object.h>


struct SMDWeight
{
    int m_BoneId;
    double m_Weight;
};

struct SMDVert
{
    QVector3D m_Position;
    QVector3D m_Normals;
    QVector2D m_UVs;
    QVector<SMDWeight> m_Weights = {};
};

struct SMDTriangle
{
    QString m_MaterialName;
    SMDVert m_Vert1;
    SMDVert m_Vert2;
    SMDVert m_Vert3;
};


struct SMDNodeData
{
    int m_BoneID;
    QString m_BoneName;
    int m_BoneParent;
    QVector3D m_Position;
    QVector3D m_Rotation;

};

enum SMDReadState
{
    StateSMDNone,
    StateSMDNodes,
    StateSMDSkeleton,
    StateSMDTriangles,
    StateSMDTrianglesVert1,
    StateSMDTrianglesVert2,
    StateSMDTrianglesVert3
};

class SMDSkeletonManager
{
public:
    SMDSkeletonManager(){};

    void AddBone(int t_id, QString t_name, int m_BoneParent, QVector3D t_Position, QVector3D t_rotation);
    void ConvertToObjectSpace(int t_bone);//

    KeyframeData *GetKeyframes();

    void ApplyToArmature(int t_bone, NeoArmature *t_armPointer);

    QMap<int, QVector<SMDNodeData>> GetGlobalNodes();

    int m_BoneIds = {};

    QMap<int, QMatrix4x4> m_CurrentMatrix = {};
    QMap<int, QVector3D> m_ParentRotations = {};
    QMap<int, QVector3D> m_ModelSpaceRotations = {};
    QMap<int, QVector<SMDNodeData>> m_RelativeBones = {};
    QMap<int, QVector<SMDNodeData>> m_ObjectSpaceBones = {};
};

class SMDModelReader
{
public:
    SMDModelReader(QString l_filePath, QString l_fileName);

    SMDVert ParseVertLine(QStringList l_data);

    void ReaderStateSkeleton(QString l_State);


    SceneObject *GenerateSceneObject();

    QMap<int, KeyframeData*> GetKeyframes(KeyframeData *l_baseModel);
    KeyframeData *GetFirstKeyframe();
    QMap<int, KeyframeData*> GetKeyframes();


    QMatrix4x4 m_Matrix[1024] = {};
    NeoArmature *m_Arm = nullptr;

    QStringList m_MaterialList = {};
    QMap<QString, QList<SMDTriangle>> m_MaterialTriangles ;
    QList<SMDNodeData> m_ModelNodes = {};

    QMap<int, SMDSkeletonManager> m_TimeSkeletonManagers = {};

    QMap<int, KeyframeData*> m_Keyframes = {};
    QString m_FilePath = "";
private:
};

#endif // SMD_MODEL_READER_H
