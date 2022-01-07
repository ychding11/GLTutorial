//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//

#ifndef Log_MY_H_
#define Log_MY_H_

//
// https://github.com/gabime/spdlog
//
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h" // must be included for user type format


namespace Logging
{
    spdlog::logger* Logger();
}

#define Log(...)  Logging::Logger()->info(__VA_ARGS__); 
#define Warn(...) Logging::Logger()->warn(__VA_ARGS__); 
#define Err(...)  Logging::Logger()->error(__VA_ARGS__); __debugbreak(); 

#include <GL/glew.h>

inline void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	while (err != GL_NO_ERROR)
	{
		Warn("OpenGL error: {}, at {}:{} - {}", gluErrorString(err), fname, line, stmt);
		err = glGetError();
	}
}

#ifdef _DEBUG
#define GL_API_CHECK(stmt) do {                          \
            stmt;                                        \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
			__debugbreak();                              \
        } while (0)
#else
#define GL_API_CHECK(stmt) stmt
#endif

#endif
