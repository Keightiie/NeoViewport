#include "smd_model_reader.h"

#include <QFile>
#include <QDebug>
#include <QQueue>

#include <Neo/OpenGL/keyframe_data.h>

SMDModelReader::SMDModelReader(QString l_filePath, QString l_fileName)
{
    m_FilePath = l_filePath;

    QFile file(l_filePath + "/" + l_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open file:" << l_filePath + "/" + l_fileName;
        return;
    }

    SMDReadState m_CurrentState = StateSMDNone;

    SMDTriangle m_CurrentTriangle;

    int m_CurrentTime = -1;
    KeyframeData *l_Keyframe = nullptr;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString l_CurrentLine = in.readLine().trimmed();

        if(l_CurrentLine == "end")
        {
            if(m_CurrentTime != -1 && l_Keyframe != nullptr)
            {
                m_Keyframes[m_CurrentTime] = l_Keyframe;
                m_TimeSkeletonManagers[m_CurrentTime].ConvertToObjectSpace(-1);
                l_Keyframe = nullptr;
                m_CurrentTime = -1;
            }
            m_CurrentState = StateSMDNone;
        }
        else if(m_CurrentState == StateSMDNodes)
        {
            QStringList l_NodesPart = l_CurrentLine.split(' ');
            int l_NodeID = l_NodesPart.at(0).toInt();
            int l_NodeParent = l_NodesPart.last().toInt();

            l_NodesPart.removeLast();
            l_NodesPart.removeFirst();

            QString m_Mat = l_NodesPart.join(' ');
            QString l_NodeName = m_Mat.split('"').at(1);

            SMDNodeData l_NewNode;
            l_NewNode.m_BoneID = l_NodeID;
            l_NewNode.m_BoneParent = l_NodeParent;
            l_NewNode.m_BoneName = l_NodeName;
            m_ModelNodes.append(l_NewNode);
        }
        else if(m_CurrentState == StateSMDSkeleton)
        {
            QStringList l_NodesPart = l_CurrentLine.split(' ');
            l_NodesPart.removeAll("");
            if(l_NodesPart.at(0) == "time")
            {
                if(m_CurrentTime != -1 && l_Keyframe != nullptr)
                {
                    m_Keyframes[m_CurrentTime] = l_Keyframe;
                    m_TimeSkeletonManagers[m_CurrentTime].ConvertToObjectSpace(-1);
                }
                m_CurrentTime = l_NodesPart.at(1).toInt();
                l_Keyframe = new KeyframeData();
            }
            else
            {
                if(l_NodesPart.count() == 7)
                {
                    int l_Bone = l_NodesPart.at(0).toInt();

                    for(SMDNodeData l_node : m_ModelNodes)
                    {
                        if(l_node.m_BoneID == l_Bone)
                        {
                            l_node.m_Position = QVector3D(l_NodesPart.at(1).toDouble(), l_NodesPart.at(2).toDouble(), l_NodesPart.at(3).toDouble());
                            l_node.m_Rotation = QVector3D(qRadiansToDegrees(l_NodesPart.at(4).toDouble()), qRadiansToDegrees(l_NodesPart.at(5).toDouble()), qRadiansToDegrees(l_NodesPart.at(6).toDouble()));
                            m_TimeSkeletonManagers[m_CurrentTime].AddBone(l_Bone, l_node.m_BoneName, l_node.m_BoneParent, l_node.m_Position, l_node.m_Rotation);
                            l_Keyframe->AddBoneTransform(l_node.m_BoneName, l_node.m_Position, l_node.m_Rotation);
                        }
                    }
                }
            }
        }
        else if(m_CurrentState == StateSMDTriangles)
        {
            m_CurrentTriangle.m_MaterialName = l_CurrentLine;

            if(!m_MaterialList.contains(l_CurrentLine)) m_MaterialList.append(l_CurrentLine);
            m_CurrentState = StateSMDTrianglesVert1;
        }
        else if(m_CurrentState == StateSMDTrianglesVert1)
        {
            QStringList l_NodesPart = l_CurrentLine.split(' ');
            l_NodesPart.removeAll("");
            m_CurrentTriangle.m_Vert1 = ParseVertLine(l_NodesPart);
            m_CurrentState = StateSMDTrianglesVert2;
        }
        else if(m_CurrentState == StateSMDTrianglesVert2)
        {
            QStringList l_NodesPart = l_CurrentLine.split(' ');
            l_NodesPart.removeAll("");
            m_CurrentTriangle.m_Vert2 = ParseVertLine(l_NodesPart);
            m_CurrentState = StateSMDTrianglesVert3;
        }
        else if(m_CurrentState == StateSMDTrianglesVert3)
        {
            QStringList l_NodesPart = l_CurrentLine.split(' ');
            l_NodesPart.removeAll("");
            m_CurrentTriangle.m_Vert3 = ParseVertLine(l_NodesPart);
            m_MaterialTriangles[m_CurrentTriangle.m_MaterialName].append(m_CurrentTriangle);
            m_CurrentState = StateSMDTriangles;
        }
        else if(m_CurrentState == StateSMDNone)
        {
            if(l_CurrentLine == "nodes") m_CurrentState = StateSMDNodes;
            if(l_CurrentLine == "skeleton") m_CurrentState = StateSMDSkeleton;
            if(l_CurrentLine == "triangles") m_CurrentState = StateSMDTriangles;
        }
    }

}

SMDVert SMDModelReader::ParseVertLine(QStringList l_data)
{
    SMDVert l_ReturnData;
    if(l_data.count() > 8)
    {
        l_ReturnData.m_Position = QVector3D(l_data.at(1).toDouble(), l_data.at(2).toDouble(), l_data.at(3).toDouble());
        l_ReturnData.m_Normals = QVector3D(l_data.at(4).toDouble(), l_data.at(5).toDouble(), l_data.at(6).toDouble());
        l_ReturnData.m_UVs = QVector2D(l_data.at(7).toDouble(), l_data.at(8).toDouble());

        int l_LinksCount = l_data.at(9).toInt();

        for(int i = 0; i < l_LinksCount; i++)
        {
            int l_BoneId = l_data.at(10 + (i * 2)).toInt();
            double l_Weight = l_data.at(11 + (i * 2)).toDouble();
            l_ReturnData.m_Weights.append({l_BoneId, l_Weight});
        }
    }
    return l_ReturnData;
}

void SMDModelReader::ReaderStateSkeleton(QString l_State)
{

}

SceneObject *SMDModelReader::GenerateSceneObject()
{
    SceneObject *l_ReturnData = new SceneObject();


    QVector<GLVertexData> m_VertexList = {};

    for(QString l_MatName : m_MaterialList)
    {
        MeshData *l_Mesh = new MeshData();
        for(SMDTriangle r_tri : m_MaterialTriangles[l_MatName])
        {
            GLVertexData l_vert;
            GLVertexData l_vert2;
            GLVertexData l_vert3;

            l_vert.m_Position = r_tri.m_Vert1.m_Position;
            l_vert2.m_Position = r_tri.m_Vert2.m_Position;
            l_vert3.m_Position = r_tri.m_Vert3.m_Position;

            l_vert.m_Normals = r_tri.m_Vert1.m_Normals;
            l_vert2.m_Normals = r_tri.m_Vert2.m_Normals;
            l_vert3.m_Normals = r_tri.m_Vert3.m_Normals;

            l_vert.m_TexCoord = r_tri.m_Vert1.m_UVs;
            l_vert2.m_TexCoord = r_tri.m_Vert2.m_UVs;
            l_vert3.m_TexCoord = r_tri.m_Vert3.m_UVs;

            l_vert.m_JointIndices = QVector3D(0, -1, -1);
            l_vert.m_JointWeights = QVector3D(1, 0, 0);


            //Vert 1
            if(r_tri.m_Vert1.m_Weights.length() > 0)
            {
                if(r_tri.m_Vert1.m_Weights.at(0).m_Weight != 0)
                {
                    l_vert.m_JointIndices.setX(r_tri.m_Vert1.m_Weights.at(0).m_BoneId);
                    l_vert.m_JointWeights.setX(r_tri.m_Vert1.m_Weights.at(0).m_Weight);
                }
            }
            if(r_tri.m_Vert1.m_Weights.length() > 1)
            {
                if(r_tri.m_Vert1.m_Weights.at(1).m_Weight != 0)
                {
                    l_vert.m_JointIndices.setY(r_tri.m_Vert1.m_Weights.at(1).m_BoneId);
                    l_vert.m_JointWeights.setY(r_tri.m_Vert1.m_Weights.at(1).m_Weight);
                }
            }
            if(r_tri.m_Vert1.m_Weights.length() > 2)
            {
                if(r_tri.m_Vert1.m_Weights.at(2).m_Weight != 0)
                {
                    l_vert.m_JointIndices.setZ(r_tri.m_Vert1.m_Weights.at(2).m_BoneId);
                    l_vert.m_JointWeights.setZ(r_tri.m_Vert1.m_Weights.at(2).m_Weight);
                }
            }


            if(r_tri.m_Vert2.m_Weights.length() > 0)
            {
                if(r_tri.m_Vert2.m_Weights.at(0).m_Weight != 0)
                {
                    l_vert2.m_JointIndices.setX(r_tri.m_Vert2.m_Weights.at(0).m_BoneId);
                    l_vert2.m_JointWeights.setX(r_tri.m_Vert2.m_Weights.at(0).m_Weight);
                }
            }
            if(r_tri.m_Vert2.m_Weights.length() > 1)
            {
                if(r_tri.m_Vert2.m_Weights.at(1).m_Weight != 0)
                {
                    l_vert2.m_JointIndices.setY(r_tri.m_Vert2.m_Weights.at(1).m_BoneId);
                    l_vert2.m_JointWeights.setY(r_tri.m_Vert2.m_Weights.at(1).m_Weight);
                }
            }
            if(r_tri.m_Vert2.m_Weights.length() > 2)
            {
                if(r_tri.m_Vert2.m_Weights.at(2).m_Weight != 0)
                {
                    l_vert2.m_JointIndices.setZ(r_tri.m_Vert2.m_Weights.at(2).m_BoneId);
                    l_vert2.m_JointWeights.setZ(r_tri.m_Vert2.m_Weights.at(2).m_Weight);
                }
            }


            if(r_tri.m_Vert3.m_Weights.length() > 0)
            {
                if(r_tri.m_Vert3.m_Weights.at(0).m_Weight != 0)
                {
                    l_vert3.m_JointIndices.setX(r_tri.m_Vert3.m_Weights.at(0).m_BoneId);
                    l_vert3.m_JointWeights.setX(r_tri.m_Vert3.m_Weights.at(0).m_Weight);
                }
            }
            if(r_tri.m_Vert3.m_Weights.length() > 1)
            {
                if(r_tri.m_Vert3.m_Weights.at(1).m_Weight != 0)
                {
                    l_vert3.m_JointIndices.setY(r_tri.m_Vert3.m_Weights.at(1).m_BoneId);
                    l_vert3.m_JointWeights.setY(r_tri.m_Vert3.m_Weights.at(1).m_Weight);
                }
            }
            if(r_tri.m_Vert3.m_Weights.length() > 2)
            {
                if(r_tri.m_Vert3.m_Weights.at(2).m_Weight != 0)
                {
                    l_vert3.m_JointIndices.setZ(r_tri.m_Vert3.m_Weights.at(2).m_BoneId);
                    l_vert3.m_JointWeights.setZ(r_tri.m_Vert3.m_Weights.at(2).m_Weight);
                }
            }



            m_VertexList.append(l_vert);
            m_VertexList.append(l_vert2);
            m_VertexList.append(l_vert3);
        }
        QString l_TexturePath = m_FilePath + "/" + l_MatName + ".png";
        l_Mesh->ProcessVertexList(m_VertexList, l_TexturePath);
        l_Mesh->Initialize();
        l_ReturnData->m_Mesh.append(l_Mesh);
    }

    m_Arm = l_ReturnData->GetArmature();
    m_TimeSkeletonManagers[0].ApplyToArmature(-1, m_Arm);

    return l_ReturnData;
}

QMap<int, KeyframeData *> SMDModelReader::GetKeyframes(KeyframeData *l_baseModel)
{
    QMap<int, KeyframeData *> l_Updated = {};
    QVector<int> m_Times = m_TimeSkeletonManagers.keys().toVector();

    for(int r_time : m_Times)
    {
        KeyframeData *l_NewTime = new KeyframeData();
        KeyframeData *l_CurrentTime = m_TimeSkeletonManagers[r_time].GetKeyframes();

        for(QString r_name : l_CurrentTime->GetBoneNames())
        {
            l_NewTime->AddBoneTransform(r_name, l_CurrentTime->GetPosition(r_name) - l_baseModel->GetPosition(r_name), l_CurrentTime->GetRotation(r_name) - l_baseModel->GetRotation(r_name));
        }
        l_Updated[r_time] = l_NewTime;
    }



    return l_Updated;
}

KeyframeData *SMDModelReader::GetFirstKeyframe()
{
    if(m_TimeSkeletonManagers.contains(0)) return m_TimeSkeletonManagers[0].GetKeyframes();
    return nullptr;
}

QMap<int, KeyframeData *> SMDModelReader::GetKeyframes()
{
    return m_Keyframes;
}

void SMDSkeletonManager::AddBone(int t_id, QString t_name, int t_BoneParent, QVector3D t_Position, QVector3D t_rotation)
{
    SMDNodeData l_newBone;
    l_newBone.m_BoneID = t_id;
    l_newBone.m_BoneName = t_name;
    l_newBone.m_BoneParent = t_BoneParent;
    l_newBone.m_Position = t_Position;
    l_newBone.m_Rotation = t_rotation;

    m_RelativeBones[t_BoneParent].append(l_newBone);
}

void SMDSkeletonManager::ConvertToObjectSpace(int t_bone)
{
    QMatrix4x4 l_CurrentBonePosition;
    QVector3D l_CurrentRotation;

    for(SMDNodeData r_node : m_RelativeBones[t_bone])
    {
        if(r_node.m_BoneParent != -1)
        {
            if(m_CurrentMatrix.contains(r_node.m_BoneParent))
            {
                l_CurrentBonePosition = m_CurrentMatrix[r_node.m_BoneParent];
            }
            if(m_ParentRotations.contains(r_node.m_BoneParent))
            {
                l_CurrentRotation = m_ParentRotations[r_node.m_BoneParent];
            }
        }
        l_CurrentBonePosition.rotate(l_CurrentRotation.z(), 0.0f, 0.0f, 1.0f);
        l_CurrentBonePosition.rotate(l_CurrentRotation.x(), 1.0f, 0.0f, 0.0f);
        l_CurrentBonePosition.rotate(l_CurrentRotation.y(), 0.0f, 1.0f, 0.0f);
        l_CurrentBonePosition.translate(r_node.m_Position);

        m_CurrentMatrix[r_node.m_BoneID] = l_CurrentBonePosition;
        m_ParentRotations[r_node.m_BoneID] = r_node.m_Rotation;

        if(m_ModelSpaceRotations.contains(r_node.m_BoneParent))
        {
            m_ModelSpaceRotations[r_node.m_BoneID] = m_ModelSpaceRotations[r_node.m_BoneParent] + r_node.m_Rotation;
        }
        else
        {
            m_ModelSpaceRotations[r_node.m_BoneID] = r_node.m_Rotation;
        }

        QVector3D translation = l_CurrentBonePosition.column(3).toVector3D();

        //Create the node in Global Space
        SMDNodeData l_GlobalNode = r_node;
        l_GlobalNode.m_Position = translation;
        l_GlobalNode.m_Rotation = m_ModelSpaceRotations[r_node.m_BoneID];

        m_ObjectSpaceBones[t_bone].append(l_GlobalNode);

        if(m_RelativeBones.contains(r_node.m_BoneID)) ConvertToObjectSpace(r_node.m_BoneID);

    }

}

KeyframeData *SMDSkeletonManager::GetKeyframes()
{
    KeyframeData *l_NewKeyframe = new KeyframeData();

    QVector<int> l_BoneParents = m_RelativeBones.keys().toVector();

    for(int r_BoneParent : l_BoneParents)
    {
        for(SMDNodeData r_node : m_RelativeBones[r_BoneParent])
        {
            l_NewKeyframe->AddBoneTransform(r_node.m_BoneName, r_node.m_Position, r_node.m_Rotation);
        }
    }

    return l_NewKeyframe;
}

void SMDSkeletonManager::ApplyToArmature(int t_bone, NeoArmature *t_armPointer)
{
    for(SMDNodeData r_node : m_ObjectSpaceBones[t_bone])
    {
        t_armPointer->AddBone(r_node.m_BoneID, r_node.m_BoneName, r_node.m_Position, r_node.m_Rotation, r_node.m_BoneParent);
        if(m_ObjectSpaceBones.contains(r_node.m_BoneID)) ApplyToArmature(r_node.m_BoneID, t_armPointer);
    }
}

QMap<int, QVector<SMDNodeData> > SMDSkeletonManager::GetGlobalNodes()
{
    return m_ObjectSpaceBones;
}
