//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#pragma once

#include <GL/glew.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "Log.h"

namespace fs = std::filesystem;

#define INVALIDE_TEXTURE_ID ((TextureID)(0))

typedef GLuint TextureID;

TextureID LoadTextureFromFile(std::string path);

class TextureCache
{

private:
    std::unordered_map<std::string, TextureID> m_caches;

public:
    void Add(const std::string& path)
    {
        std::string key = fs::path(path).filename().string();
        auto item = m_caches.find(key);
        if (item != m_caches.end())
        {
            assert(0);
            Err("Texture {} already in cache.", key);
        }
        else
        {
            //< check file exist here
            m_caches[key] = LoadTextureFromFile(path);
        }
    }

    TextureID Acquire(const std::string& key)
    {
        auto item = m_caches.find(key);
        if (item != m_caches.end())
        {
            return item->second;
        }
        else
        {
            return INVALIDE_TEXTURE_ID;
        }
    }

    std::vector<std::string> DiffuseMapTextureNames() const
    {
        std::vector<std::string> names;
        for_each(m_caches.begin(), m_caches.end(),
            [&names](auto pair)
            {
                if (pair.first.find("_diffuse") != std::string::npos)
                {
                    names.emplace_back(pair.first);
                }
            }
        );
        return names;
    }
    std::vector<std::string> NormalMapTextureNames() const
    {
        std::vector<std::string> names;
        for_each(m_caches.begin(), m_caches.end(),
            [&names](auto pair)
            {
                if (pair.first.find("_normal") != std::string::npos)
                {
                    names.emplace_back(pair.first);
                }
            }
        );
        return names;
    }
    std::vector<std::string> TextureNames() const
    {
        auto key_picker = [](auto pair) {return pair.first; };
        std::vector<std::string> names(m_caches.size());

        transform(m_caches.begin(), m_caches.end(), names.begin(), key_picker);
        return names;
    }
};

TextureCache& GetTextureCache();
