//=================================================================================//
// Copyright (c) 2021 Yaochuang Ding 
//=================================================================================//
#pragma once

#include <GL/glew.h>
#include "shader.h"

#define kInvalidGLId 0xffffffffu

#define QUERY_GL_Limit(name)                                        \
do {                                                                \
    int value = 0;                                                  \
    glGetIntegerv(name, &value);                                    \
    Log(#name"={}", value);                                         \
} while (0);

class AABB;
class Camera;

class BoundingBoxDrawer
{
public :
    BoundingBoxDrawer() = default;

    void Init();
    void DrawBoundingBox(const AABB& aabb, const Camera& camera);

private:
    GLuint m_vao_id;
    GLuint vbo_id;
    GLuint ibo_id;
    Shader m_shader{"BoxDrawer"};
};

//< suppose all shader parameter is updated
void PostProcessing(GLuint srcTex, GLuint dstTex, const Shader& shader, std::string label = "PostProcessing");

void PresentTex(const Shader& shader, std::string label = "PresentTexture");


//
inline bool IsValidGLId(GLuint id)
{
    return id != kInvalidGLId;
}

// busy wait for GPU job done
inline static void SyncGPU()
{
    GLsync sync_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLenum wait_return = GL_UNSIGNALED;
    while (wait_return != GL_ALREADY_SIGNALED && wait_return != GL_CONDITION_SATISFIED)
        wait_return = glClientWaitSync(sync_fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
    glDeleteSync(sync_fence);
}

//< Buffer object wiki : https://www.khronos.org/opengl/wiki/Buffer_Object
class Buffer
{
private:
	GLuint m_id{ kInvalidGLId };

public:
	Buffer() = default;

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& buffer) noexcept
		: m_id(buffer.m_id)
	{
		buffer.m_id = kInvalidGLId;
	}

	Buffer& operator= (const Buffer&) = delete;

	/*Buffer& operator= (Buffer&& buffer) noexcept
	{
		m_id = buffer.m_id; buffer.m_id = kInvalidOglId;
		return *this;
	}*/

	~Buffer()
	{
		if (IsValidGLId(m_id))
			glDeleteBuffers(1, &m_id);
	}

	void Initialize()
	{
		glCreateBuffers(1, &m_id);
	}

	GLuint Get() const { return m_id; }

	void MutableStorage(GLsizei bytes, GLbitfield flags)
	{
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferData.xhtml
		// since GL 4.5
		// mutable buffer storage, reallocate storage each call
		GL_API_CHECK( glNamedBufferData(m_id, bytes, nullptr, flags) );
	}

	void Storage(GLsizei bytes, GLbitfield flags)
	{
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferStorage.xhtml
		// since GL 4.5
		// immutable buffer storage
		GL_API_CHECK( glNamedBufferStorage(m_id, bytes, nullptr, flags) );
	}

	template<class T>
	void Storage(T* begin, T* end, GLbitfield flags)
	{
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferStorage.xhtml
		// since GL 4.5
		glNamedBufferStorage(m_id, (end - begin) * sizeof(T), begin, flags);
	}

	//never use Data() with Storage()
	template<class T>
	void Data(T* begin, T* end, GLenum usage)
	{
		glNamedBufferData(m_id, (end - begin) * sizeof(T), begin, usage);
	}

	template<class T>
	void SubData(GLsizei byte_offset, const T* begin, const T* end)
	{
		glNamedBufferSubData(m_id, byte_offset, (end - begin) * sizeof(T), begin);
	}

	GLint GetByteCount() const
	{
		GLint c = 0;
		glGetNamedBufferParameteriv(m_id, GL_BUFFER_SIZE, &c);
		return c;
	}

	void Bind(GLenum target) const { glBindBuffer(target, m_id); }

	void BindBase(GLenum target, GLuint index) const
	{
		glBindBufferBase(target, index, m_id);
	}
};

class AtomicCounter
{
private:
	Buffer m_buffer;
	GLuint* m_mapped{ nullptr };

public:
	~AtomicCounter()
	{
		if (m_mapped) glUnmapNamedBuffer(m_buffer.Get());
	}

	void Initialize()
	{
		m_buffer.Initialize();
		GLbitfield flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
		m_buffer.Storage(sizeof(GLuint), flags);

		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glMapBufferRange.xhtml
		// since GL4.5
		m_mapped = (GLuint*)glMapNamedBufferRange(m_buffer.Get(), 0, sizeof(GLuint), flags);
		*m_mapped = 0u; // assign an initial value 0
	}

	void BindAsAtomicCounter(GLuint index) const { glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, m_buffer.Get()); }
	void BindAsShaderStorage(GLuint index) const { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_buffer.Get()); }

	      GLuint& GetValue()       { return *m_mapped; }
	const GLuint& GetValue() const { return *m_mapped; }

	GLuint& SyncAndGetValue()
	{
		SyncGPU();
		return *m_mapped;
	}
	const GLuint& SyncAndGetValue() const
	{
		SyncGPU();
		return *m_mapped;
	}

	void Reset() { SyncGPU(); *m_mapped = 0; }
};
