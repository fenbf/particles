#include "glParticleRenderer.h"
#include "particles.h"

#include <assert.h>
#include "gl_includes.h"

namespace
{
	void destroyBuffer(GLuint & buf)
	{
		if (buf != 0)
		{
			glDeleteBuffers(1, &buf);
			buf = 0;
		}
	}

	void genSingleBuffer(GLuint & buf, GLuint count, float ** mappedBuffer)
	{
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);

		if (mappedBuffer && GLEW_ARB_buffer_storage)
		{
			const GLbitfield creationFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_DYNAMIC_STORAGE_BIT;
			const GLbitfield mapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
			const unsigned int BUFFERING_COUNT = 3;
			const GLsizeiptr neededSize = sizeof(float) * 4 * count * BUFFERING_COUNT;

			glBufferStorage(GL_ARRAY_BUFFER, neededSize, nullptr, creationFlags);
			
			*mappedBuffer = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, neededSize, mapFlags);
		}
		else
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * count, nullptr, GL_STREAM_DRAW);
	}

	void setVertexAttrib(GLuint & bufPos, GLuint attribID, GLuint elements)
	{
		glEnableVertexAttribArray(attribID);

		if (GLEW_ARB_vertex_attrib_binding)
		{
			glBindVertexBuffer(attribID, bufPos, 0, sizeof(float) * elements);
			glVertexAttribFormat(attribID, elements, GL_FLOAT, GL_FALSE, 0);
			glVertexAttribBinding(attribID, attribID);
		}
		else
			glVertexAttribPointer(attribID, elements, GL_FLOAT, GL_FALSE, (elements)*sizeof(float), (void *)((0)*sizeof(float)));
	}

	void generateBuffers(GLuint &vao, GLuint &bufPos, GLuint &bufCol, GLuint count, float **mappedBuffer1, float **mappedBuffer2)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		genSingleBuffer(bufPos, count, mappedBuffer1);
		setVertexAttrib(bufPos, 0, 4);

		genSingleBuffer(bufCol, count, mappedBuffer2);
		setVertexAttrib(bufCol, 1, 4);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

namespace particles
{
	void GLParticleRenderer::generate(ParticleSystem *sys, bool)
	{
		assert(sys != nullptr);

		m_system = sys;

		const size_t count = sys->numAllParticles();

		generateBuffers(m_vao, m_bufPos, m_bufCol, count, nullptr, nullptr);
	}

	void GLParticleRenderer::destroy()
	{
		destroyBuffer(m_bufPos);
		destroyBuffer(m_bufCol);
	}

	void GLParticleRenderer::update()
	{
		assert(m_system != nullptr);
		assert(m_bufPos > 0 && m_bufCol > 0);

		const size_t count = m_system->numAliveParticles();
		if (count > 0)
		{
			float *posPtr = (float *)(m_system->finalData()->m_pos);
			float *colPtr = (float *)(m_system->finalData()->m_col);

			glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, posPtr);

			glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, colPtr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void GLParticleRenderer::render()
	{
		glBindVertexArray(m_vao);
		const size_t count = m_system->numAliveParticles();

		if (count > 0)
			glDrawArrays(GL_POINTS, 0, count);

		glBindVertexArray(0);
	}

	void GLParticleRendererUseMap::update()
	{
		assert(m_system != nullptr);
		assert(m_bufPos > 0 && m_bufCol > 0);

		const size_t count = m_system->numAliveParticles();
		if (count > 0)
		{
			float *posPtr = (float *)(m_system->finalData()->m_pos);
			float *colPtr = (float *)(m_system->finalData()->m_col);

			glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
			//glInvalidateBufferData(GL_ARRAY_BUFFER);
			float *mem = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, count*sizeof(float) * 4, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
			memcpy(mem, posPtr, count*sizeof(float) * 4);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
			//glInvalidateBufferData(GL_ARRAY_BUFFER);
			mem = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, count*sizeof(float) * 4, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
			memcpy(mem, colPtr, count*sizeof(float) * 4);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void GLParticleRendererDoubleVao::generate(ParticleSystem *sys, bool)
	{
		assert(sys != nullptr);

		m_system = sys;

		const size_t count = sys->numAllParticles();

		generateBuffers(m_doubleVao[0], m_doubleBufPos[0], m_doubleBufCol[0], count, nullptr, nullptr);
		generateBuffers(m_doubleVao[1], m_doubleBufPos[1], m_doubleBufCol[1], count, nullptr, nullptr);
		m_id = 0;
	}

	void GLParticleRendererDoubleVao::destroy()
	{
		destroyBuffer(m_doubleBufPos[0]);
		destroyBuffer(m_doubleBufPos[1]);
		destroyBuffer(m_doubleBufCol[0]);
		destroyBuffer(m_doubleBufCol[1]);
	}

	void GLParticleRendererDoubleVao::update()
	{
		assert(m_system != nullptr);
		assert(m_doubleBufPos[m_id] > 0 && m_doubleBufCol[m_id] > 0);

		const size_t count = m_system->numAliveParticles();
		if (count > 0)
		{
			float *posPtr = (float *)(m_system->finalData()->m_pos);
			float *colPtr = (float *)(m_system->finalData()->m_col);

			glBindBuffer(GL_ARRAY_BUFFER, m_doubleBufPos[m_id]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float) * 4, posPtr);

			glBindBuffer(GL_ARRAY_BUFFER, m_doubleBufCol[m_id]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float) * 4, colPtr);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void GLParticleRendererDoubleVao::render()
	{
		glBindVertexArray(m_doubleVao[1 - m_id]);

		const size_t count = m_system->numAliveParticles();
		if (count > 0)
			glDrawArrays(GL_POINTS, 0, count);

		glBindVertexArray(0);

		m_id = 1 - m_id;
	}

	void GLParticleRendererPersistent::generate(ParticleSystem *sys, bool)
	{
		assert(sys != nullptr);

		m_system = sys;

		const size_t count = sys->numAllParticles();

		generateBuffers(m_vao, m_bufPos, m_bufCol, count, &m_mappedPosBuf, &m_mappedColBuf);
		m_id = 0;
	}

	void GLParticleRendererPersistent::update()
	{
		assert(m_system != nullptr);
		assert(m_bufPos > 0 && m_bufCol > 0);

		const size_t count = m_system->numAliveParticles();
		if (count > 0)
		{
			float *posPtr = (float *)(m_system->finalData()->m_pos);
			float *colPtr = (float *)(m_system->finalData()->m_col);
			const size_t maxCount = m_system->numAllParticles();
			
			float *mem = (float *)m_mappedPosBuf + m_id*maxCount * 4;
			memcpy(mem, posPtr, count*sizeof(float) * 4);
			mem = (float *)m_mappedColBuf + m_id*maxCount * 4;
			memcpy(mem, colPtr, count*sizeof(float) * 4);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}

	void GLParticleRendererPersistent::render()
	{
		glBindVertexArray(m_vao);

		const size_t count = m_system->numAliveParticles();
		const size_t maxCount = m_system->numAllParticles();
		if (count > 0)
			glDrawArrays(GL_POINTS, m_id*maxCount, count);

		glBindVertexArray(0);

		m_id = (m_id + 1) % 3;
	}
}



