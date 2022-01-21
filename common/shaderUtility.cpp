//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <filesystem> //< require c++ 17

#include "shaderUtility.h"
#include "Log.h"

namespace fs = std::filesystem;

static GLuint createShaderFromFile(GLenum type, std::string path);

static std::string readFileToString(fs::path path)
{
	if (!fs::exists(path))
	{
		Err("file NOT exist : {}", path);
	}

	std::ifstream f(path, std::ios::in | std::ios::binary);
	const auto sz = fs::file_size(path);
	std::string result(sz, '\0');
	f.read(result.data(), sz);
	f.close();
	return result;
}

GLuint loadStandardShaders(const char *vert_file_path, const char *frag_file_path)
{
    GLuint vertexShaderID   = createShaderFromFile(GL_VERTEX_SHADER, vert_file_path);
    GLuint fragmentShaderID = createShaderFromFile(GL_FRAGMENT_SHADER, frag_file_path);

    Log( "Linking shader..." );
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    GLint result = GL_FALSE;
    int infoLogLength;

    glGetShaderiv(programID, GL_LINK_STATUS, &result);
    glGetShaderiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0)
    {
        std::vector<char> programErrorMessage(infoLogLength);
        glGetShaderInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
		Err("{}", programErrorMessage.data());
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

GLuint loadTessShaders(const char *tess_vert_file_path, const char *tess_ctrl_file_path, const char *tess_eval_file_path, const char *tess_frag_file_path, const char *gs_file_path)
{
	GLuint vertShaderID = createShaderFromFile(GL_VERTEX_SHADER, tess_vert_file_path);
	GLuint ctrlShaderID = createShaderFromFile(GL_TESS_CONTROL_SHADER, tess_ctrl_file_path);
	GLuint evalShaderID = createShaderFromFile(GL_TESS_EVALUATION_SHADER, tess_eval_file_path);
	GLuint fragShaderID = createShaderFromFile(GL_FRAGMENT_SHADER, tess_frag_file_path);
    GLuint geoShaderID  = createShaderFromFile(GL_GEOMETRY_SHADER, gs_file_path);

    Log("Linking Shader...");
	GLuint m_particlProgramID = glCreateProgram();
	glAttachShader(m_particlProgramID, vertShaderID);
	glAttachShader(m_particlProgramID, ctrlShaderID);
	glAttachShader(m_particlProgramID, evalShaderID);
	glAttachShader(m_particlProgramID, geoShaderID);
	glAttachShader(m_particlProgramID, fragShaderID);
	glLinkProgram(m_particlProgramID);

    GLint result = GL_FALSE;
    int infoLogLength;
	glGetProgramiv(m_particlProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(m_particlProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
    {
		std::vector<char> tessProgramErrMsg(infoLogLength );
		glGetProgramInfoLog(m_particlProgramID, infoLogLength, NULL, &tessProgramErrMsg[0]);
		Err("{}", tessProgramErrMsg.data());
	}

	glDetachShader(m_particlProgramID, vertShaderID);
	glDetachShader(m_particlProgramID, ctrlShaderID);
	glDetachShader(m_particlProgramID, evalShaderID);
	glDetachShader(m_particlProgramID, fragShaderID);
	glDetachShader(m_particlProgramID, geoShaderID);

	glDeleteShader(vertShaderID);
	glDeleteShader(ctrlShaderID);
	glDeleteShader(evalShaderID);
	glDeleteShader(fragShaderID);
	glDeleteShader(geoShaderID);

	return m_particlProgramID;
}

GLuint loadExplodeShaders(const char *vert_file_path, const char *gs_file_path, const char *frag_file_path)
{
    GLuint vertShaderID = createShaderFromFile(GL_VERTEX_SHADER, vert_file_path);
    GLuint geoShaderID  = createShaderFromFile(GL_GEOMETRY_SHADER, gs_file_path);
    GLuint fragShaderID = createShaderFromFile(GL_FRAGMENT_SHADER, frag_file_path);

	GLint result = false;
	int infoLogLength;

    Log("Linking Shader...\n");
	GLuint explodeProgramID = glCreateProgram();
	glAttachShader(explodeProgramID, vertShaderID);
	glAttachShader(explodeProgramID, geoShaderID);
	glAttachShader(explodeProgramID, fragShaderID);
	glLinkProgram(explodeProgramID);

	glGetProgramiv(explodeProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(explodeProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0)
    {
		std::vector<char> programErrMsg(infoLogLength);
		glGetProgramInfoLog(explodeProgramID, infoLogLength, NULL, &programErrMsg[0]);
		Err("{}", programErrMsg.data());
	}

	glDetachShader(explodeProgramID, vertShaderID);
	glDetachShader(explodeProgramID, geoShaderID);
	glDetachShader(explodeProgramID, fragShaderID);

	glDeleteShader(vertShaderID);
	glDeleteShader(geoShaderID);
	glDeleteShader(fragShaderID);

	return explodeProgramID;
}

static GLuint createShaderFromFile(GLenum type, std::string path)
{
    assert(
        GL_VERTEX_SHADER == type ||
        GL_TESS_CONTROL_SHADER == type ||
        GL_TESS_EVALUATION_SHADER == type ||
        GL_FRAGMENT_SHADER == type ||
        GL_GEOMETRY_SHADER == type
    );

	std::string shaderCode = readFileToString(path);
    Log("Compiling shader : {}", path);

	//< creates an empty shader object for the shader stage given by given type
	GLuint shaderID = glCreateShader(type); 
    char const* tempPtr = shaderCode.c_str();
	//< Feed shader source code into OpenGL
	//< here we feed 1 string
	//< NULL, OpenGL will assume all of the strings are NULL-terminated
    glShaderSource(shaderID, 1, &tempPtr, NULL);
	//< Compiles the given shader
    glCompileShader(shaderID);

	GLint success = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) //< the most recent compilation failed
	{
		//< query how many bytes to allocate, the length includes NULL terminator.
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		//< NULL, don't care actually written bytes
		glGetShaderInfoLog(shaderID, maxLength, NULL, &errorLog[0]);
		Err("{}", errorLog.data());
		return 0;
	}
    return shaderID;
}

GLuint LoadShaders(const char* vs_file_path, const char* fs_file_path, const char* tcs_file_path, const char* tes_file_path,
    const char* gs_file_path)
{
	GLuint programID = glCreateProgram();

	GLuint vertShaderID = 0;
	GLuint ctrlShaderID = 0;
	GLuint evalShaderID = 0;
	GLuint fragShaderID = 0;
	GLuint geoShaderID  = 0; 

	if (vs_file_path)
	{
		vertShaderID = createShaderFromFile(GL_VERTEX_SHADER, vs_file_path);
		glAttachShader(programID, vertShaderID);
	}

	if (fs_file_path)
	{
		fragShaderID = createShaderFromFile(GL_FRAGMENT_SHADER, fs_file_path);
		glAttachShader(programID, fragShaderID);
	}

	if (tcs_file_path)
	{
		ctrlShaderID = createShaderFromFile(GL_TESS_CONTROL_SHADER, tcs_file_path);
		glAttachShader(programID, ctrlShaderID);
	}

	if (tes_file_path)
	{
		evalShaderID = createShaderFromFile(GL_TESS_EVALUATION_SHADER, tes_file_path);
		glAttachShader(programID, evalShaderID);
	}

	if (gs_file_path)
	{
		GLuint geoShaderID  = createShaderFromFile(GL_GEOMETRY_SHADER, gs_file_path);
		glAttachShader(programID, geoShaderID);
	}

    Log("Linking Shader... ");
	glLinkProgram(programID);

	GLint success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) //< the most recent link failed
	{
		//< query how many bytes to allocate, the length includes NULL terminator.
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errorLog(maxLength);
		//< NULL, don't care actually written bytes
		glGetProgramInfoLog(programID, maxLength, NULL, &errorLog[0]);
		Err("{}", &errorLog[0]);
	}

	//
	// https://khronos.org/registry/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml
	// A value of 0 for shader will be silently ignored
	//

	if (vertShaderID)
	{
		glDetachShader(programID, vertShaderID);
		GL_API_CHECK( glDeleteShader(vertShaderID) );
	}
	if (ctrlShaderID)
	{
		glDetachShader(programID, ctrlShaderID);
		GL_API_CHECK( glDeleteShader(ctrlShaderID) );
	}
	if (evalShaderID)
	{
		glDetachShader(programID, evalShaderID);
		GL_API_CHECK( glDeleteShader(evalShaderID) );
	}
	if (geoShaderID)
	{
		glDetachShader(programID, geoShaderID);
		GL_API_CHECK( glDeleteShader(geoShaderID) );
	}
	if (fragShaderID)
	{
		glDetachShader(programID, fragShaderID);
		GL_API_CHECK( glDeleteShader(fragShaderID) );
	}

	return programID;
}
