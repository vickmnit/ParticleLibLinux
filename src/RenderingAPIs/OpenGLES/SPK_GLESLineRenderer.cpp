//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////


#include "RenderingAPIs/OpenGLES/SPK_GLESLineRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_ArrayBuffer.h"


namespace SPK
{
namespace GLES
{
	const std::string GLESLineRenderer::GPU_BUFFER_NAME("SPK_GLLineRenderer_GPU");

	float* GLESLineRenderer::gpuBuffer = NULL;
	float* GLESLineRenderer::gpuIterator = NULL;

	GLESLineRenderer::GLESLineRenderer(float length,float width) :
		GLESRenderer(),
		LineRendererInterface(length,width)
		//RainShader("res/Shaders/Particles.vert.glsl", "res/Shaders/Particles.frag.glsl")
	{}

	bool GLESLineRenderer::checkBuffers(const Group& group)
	{
		FloatBuffer* fBuffer;
		if ((fBuffer = dynamic_cast<FloatBuffer*>(group.getBuffer(GPU_BUFFER_NAME))) == NULL)
			return false;

		gpuIterator = gpuBuffer = fBuffer->getData();
		return true;
	}

	void GLESLineRenderer::createBuffers(const Group& group)
	{	
		FloatBuffer* fBuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(GPU_BUFFER_NAME,FloatBufferCreator(14),0,false));
		gpuIterator = gpuBuffer = fBuffer->getData();

		RainShader->generateVBO();
	}

	void GLESLineRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(GPU_BUFFER_NAME);
		RainShader->deleteVBO();
	}

	void GLESLineRenderer::setTransformations(glm::mat4 p_modelview, glm::mat4 p_projection, glm::mat4 p_mvp)
	{
		modelviewMat = p_modelview;
		projectionMat = p_projection;
		mvpMat = p_mvp;
	}

	void GLESLineRenderer::setResourcePaths(char* p_texturePath, unsigned int p_imageFormat, const char* p_vertShaderPath, const char* p_fragShaderPath, size_t vlen, size_t flen)
	{
        texturePath = p_texturePath;
		vertShaderPath = p_vertShaderPath;
        fragShaderPath = p_fragShaderPath;
		vertLineLen = (int)vlen;
		fragLineLen = (int)flen;
		imageFormat = p_imageFormat;
        if(RainShader == nullptr)
		   RainShader = new Shader(vertShaderPath, fragShaderPath, &vertLineLen, &fragLineLen);
	}

	void GLESLineRenderer::render(const Group& group)
	{
		if (!prepareBuffers(group))
		 	return;

		initBlending();
		initRenderingHints();

		glLineWidth(width);
		glDisable(GL_TEXTURE_2D);
		//glShadeModel(GL_FLAT);

		size_t nbPartices = group.getNbParticles();
		size_t verticesSize = 14 * nbPartices;
		float vertices[verticesSize];

		for (size_t i = 0; i < nbPartices; ++i)
		{
			const Particle& particle = group.getParticle(i);

			vertices[(0+14*i)] = particle.position().x;
			vertices[(1+14*i)] = particle.position().y;
			vertices[(2+14*i)] = particle.position().z;

			vertices[(3+14*i)] = particle.getR();
			vertices[(4+14*i)] = particle.getG();
			vertices[(5+14*i)] = particle.getB();
			vertices[(6+14*i)] = particle.getParamCurrentValue(PARAM_ALPHA);	

			vertices[(7+14*i)] = particle.position().x + particle.velocity().x * length;
			vertices[(8+14*i)] = particle.position().y + particle.velocity().y * length;
			vertices[(9+14*i)] = particle.position().z + particle.velocity().z * length;

			vertices[(10+14*i)] = particle.getR();
			vertices[(11+14*i)] = particle.getG();
			vertices[(12+14*i)] = particle.getB();
			vertices[(13+14*i)] = particle.getParamCurrentValue(PARAM_ALPHA);	
		}

		// float SampTriVerts[] = {
        //  	-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top right
        // 	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom right
        // 	0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f // bottom left
    	// };

		// create transformations
		// modelviewMat = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		// modelviewMat = glm::rotate(modelviewMat, glm::radians(12.0f), glm::vec3(1.0f,0.0f,0.0f));
		// modelviewMat = glm::rotate(modelviewMat, glm::radians(0.0f), glm::vec3(0.0f,1.0f,0.0f));
		// modelviewMat = glm::translate(modelviewMat, glm::vec3(-0.0f,-1.0f,-0.0f));

		// projectionMat = glm::perspective(glm::radians(45.0f), 256.0f/256.0f, 0.01f, 20.0f);
		RainShader->bindVBO();
		RainShader->setVBOData(sizeof(vertices), vertices);
		RainShader->use();
		RainShader->setAttribute("aPos", 3, 7, 0);
		RainShader->setAttribute("aCol", 4, 7, 3);
		RainShader->setMatrix("modelview", modelviewMat);
		RainShader->setMatrix("projection", projectionMat);
		RainShader->setMatrix("mvp", mvpMat);
		
		glDrawArrays(GL_LINES,0,nbPartices * 2);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}
}
