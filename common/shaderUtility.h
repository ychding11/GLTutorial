//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//


#pragma once

#include <GL/glew.h>

//<
//< It provides 
//<

GLuint loadStandardShaders(const char *vert_file_path, const char *frag_file_path);

GLuint loadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path, const char *tess_gs_file_path);

GLuint loadExplodeShaders(const char *vert_file_path, const char *gs_file_path, const char *frag_file_path);

GLuint LoadShaders(const char *vs_file_path, const char *fs_file_path, const char *tcs_file_path = nullptr, const char *tes_file_path = nullptr,
    const char *gs_file_path = nullptr );
