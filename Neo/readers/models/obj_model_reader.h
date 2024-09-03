#ifndef OBJ_MODEL_READER_H
#define OBJ_MODEL_READER_H

#include <QMap>
#include <QString>

#include <Neo/OpenGL/mesh_data.h>
#include <Neo/scene/scene_object.h>

class MtlModelReader
{
public:
    MtlModelReader(QString l_filePath, QString l_fileName);
    MaterialData *GetMaterial(QString m_MatName);

private:
    QMap<QString, MaterialData*> m_Materials = {};
};

class ObjModelReader //: public ModelReader
{
public:
    ObjModelReader(QString l_fileName);
    ObjModelReader(QString l_filePath, QString l_fileName);

    QList<MeshData*> GetMeshData();

    SceneObject* GenerateSceneObject();

private:
    void ParseFile();

    MeshData *ParseMeshData(int l_StartingVert);

    QString m_FilePath = "";
    QString m_FileName = "";

    QList<MeshData*> m_ParsedMeshData = {};

    QList<MtlModelReader *> m_MtlReaders = {};
    QMap<int, QString> m_FaceMaterialIndex = {};

    QStringList m_LoadedLinesVertex = {};
    QStringList m_LoadedLinesVertexTexCords = {};
    QStringList m_LoadedLinesVertexNorms = {};
    QStringList m_LoadedLinesFaces = {};
};

#endif // OBJ_MODEL_READER_H
