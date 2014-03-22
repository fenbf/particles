#include "particleRenderer.h"
#include "particles.h"

#include <assert.h>
#include "gl_comp_3_3.h"

namespace particles
{

	void GLParticleRenderer::generate(ParticleSystem *sys, bool)
	{
		assert(sys != nullptr);

		m_system = sys;

		const size_t count = sys->numAllParticles();

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_bufPos);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);

		if (ogl_ext_ARB_vertex_attrib_binding)
		{
			glBindVertexBuffer(0, m_bufPos, 0, sizeof(float)* 4);
			glVertexAttribFormat(0, 4, GL_FLOAT, GL_FALSE, 0);
			glVertexAttribBinding(0, 0);
		}
		else
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));
		

		glGenBuffers(1, &m_bufCol);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 4 * count, nullptr, GL_STREAM_DRAW);
		glEnableVertexAttribArray(1);

		if (ogl_ext_ARB_vertex_attrib_binding)
		{
			glBindVertexBuffer(1, m_bufCol, 0, sizeof(float)* 4);
			glVertexAttribFormat(1, 4, GL_FLOAT, GL_FALSE, 0);
			glVertexAttribBinding(1, 1);
		}
		else
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (4)*sizeof(float), (void *)((0)*sizeof(float)));
		

		//glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat)* 3);
		//glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat)* 3);

		//glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
		//glVertexAttribBinding(0, 0);
		//glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
		//glVertexAttribBinding(1, 1);

		glBindVertexArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
			float *ptr = (float *)(m_system->finalData()->m_pos.get());
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

			glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
			ptr = (float*)(m_system->finalData()->m_col.get());
			glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float)* 4, ptr);

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
}
/*
First of all, I don't really understand why you use pos.x + 1.

Next, like Nathan said, you shouldn't use the clip-space point, but the eye-space point. This means you only use the modelview-transformed point (without projection) to compute the distance.

uniform mat4 MV;       //modelview matrix

vec3 eyePos = MV * vec4(pos.x, pos.y, 0.5, 1);
Furthermore I don't completely understand your attenuation computation. At the moment a higher constAtten value means less attenuation. Why don't you just use the model that OpenGL's deprecated point parameters used:

float dist = length(eyePos);   //since the distance to (0,0,0) is just the length
float attn = inversesqrt(constAtten + linearAtten*dist + quadAtten*dist*dist);
EDIT: But in general I think this attenuation model is not a good way, because often you just want the sprite to keep its object space size, which you have quite to fiddle with the attenuation factors to achieve that I think.

A better way is to input its object space size and just compute the screen space size in pixels (which is what gl_PointSize actually is) based on that using the current view and projection setup:

uniform mat4 MV;                //modelview matrix
uniform mat4 P;                 //projection matrix
uniform float spriteWidth;      //object space width of sprite (maybe an per-vertex in)
uniform float screenWidth;      //screen width in pixels

vec4 eyePos = MV * vec4(pos.x, pos.y, 0.5, 1);
vec4 projCorner = P * vec4(0.5*spriteWidth, 0.5*spriteWidth, eyePos.z, eyePos.w);
gl_PointSize = screenWidth * projCorner.x / projCorner.w;
gl_Position = P * eyePos;
This way the sprite always gets the size it would have when rendered as a textured quad with a width of spriteWidth.

EDIT: Of course you also should keep in mind the limitations of point sprites. A point sprite is clipped based of its center position. This means when its center moves out of the screen, the whole sprite disappears. With large sprites (like in your case, I think) this might really be a problem.

Therefore I would rather suggest you to use simple textured quads. This way you circumvent this whole attenuation problem, as the quads are just transformed like every other 3d object. You only need to implement the rotation toward the viewer, which can either be done on the CPU or in the vertex shader
*/

