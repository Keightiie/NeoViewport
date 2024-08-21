#include "texture_manager.h"

TextureManager::TextureManager() {}

void TextureManager::LoadTexture(QString l_Path)
{
    QImage img(l_Path);
    if (img.isNull())
    {
        qWarning("Failed to load texture image");
        return;
    }

    QOpenGLTexture *l_NewTexture = new QOpenGLTexture(img.mirrored());
    if (!l_NewTexture->isCreated())
    {
        qWarning("Failed to create texture");
        return;
    }

    l_NewTexture->setMinificationFilter(QOpenGLTexture::Linear);
    l_NewTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    l_NewTexture->setWrapMode(QOpenGLTexture::Repeat);

    m_LoadedTextures[l_Path] = l_NewTexture;
}

QOpenGLTexture *TextureManager::GetTexture(QString l_path)
{
    QOpenGLTexture * l_Texture = (m_LoadedTextures.contains(l_path)) ? m_LoadedTextures[l_path] : nullptr;

    if(l_Texture == nullptr)
    {
        LoadTexture(l_path);
    }

    l_Texture = (m_LoadedTextures.contains(l_path)) ? m_LoadedTextures[l_path] : nullptr;

    return l_Texture;
}

