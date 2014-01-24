#include "particleRenderer.h"
#include "particles.h"

#include <assert.h>
#include "gl_comp_3_3.h"

void GLParticleRenderer::generate(ParticleSystem *sys, bool)
{
	assert(sys != nullptr);

	m_system = sys;

	const size_t count = sys->numAllParticles();

	glGenBuffers(1, &m_bufPos);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);

	glGenBuffers(1, &m_bufCol);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void GLParticleRenderer::destroy()
{
	if (m_bufPos != 0)
	{
		glDeleteBuffers(1, &m_bufPos);
		m_bufPos = 0;
	}

	if (m_bufCol != 0)
	{
		glDeleteBuffers(1, &m_bufCol);
		m_bufCol = 0;
	}
}

void GLParticleRenderer::update()
{
	assert(m_system != nullptr);
	assert(m_bufPos > 0 && m_bufCol > 0);

	const size_t count = m_system->numAliveParticles();
	if (count > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, m_system->finalData()->m_pos.data());
		glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, m_system->finalData()->m_col.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GLParticleRenderer::render()
{
	glEnable(GL_POINT_SPRITE);
	glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointSize(64.0f);

	float att_param[] = { 0.0f, 0.0f, 0.01f, 0.0f };
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att_param);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);	
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
	const size_t count = m_system->numAliveParticles();
	if (count > 0)
		glDrawArrays(GL_POINTS, 0, count);

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glBindVertexArray(0);
	glDisable(GL_POINT_SPRITE);
}