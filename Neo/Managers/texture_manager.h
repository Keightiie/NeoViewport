#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <QMap>
#include <QObject>
#include <QOpenGLTexture>

class TextureManager
{
public:
    TextureManager();

    void LoadTexture(QString l_Path);
    QOpenGLTexture *GetTexture(QString l_path);

private:
    QMap<QString, QOpenGLTexture*> m_LoadedTextures = {};
};

#endif // TEXTURE_MANAGER_H
