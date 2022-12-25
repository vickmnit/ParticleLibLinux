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


#include "RenderingAPIs/OpenGLES/SPK_GLESQuadRenderer.h"
#include "Core/SPK_Particle.h"
#include "Core/SPK_Group.h"
#include "Core/SPK_ArrayBuffer.h"
#include "stb_image.h"

namespace SPK
{
namespace GLES
{
	const std::string GLESQuadRenderer::GPU_BUFFER_NAME("SPK_GLESQuadRenderer_GPU");
	const std::string GLESQuadRenderer::TEXTURE_BUFFER_NAME("SPK_GLESQuadRenderer_Texture");

	float* GLESQuadRenderer::gpuBuffer = NULL;
	float* GLESQuadRenderer::gpuIterator = NULL;
	float* GLESQuadRenderer::textureBuffer = NULL;
	float* GLESQuadRenderer::textureIterator = NULL;

	void (GLESQuadRenderer::*GLESQuadRenderer::renderParticle)(const Particle&, const size_t) = NULL;

	GLESQuadRenderer::GLESQuadRenderer(float scaleX,float scaleY) :
		GLESRenderer(),
		QuadRendererInterface(scaleX,scaleY),
		Oriented3DRendererInterface(),
		//GLExtHandler(),
		textureIndex(0)
	{}

	void GLESQuadRenderer::setTransformations(glm::mat4 p_modelview, glm::mat4 p_projection, glm::mat4 p_mvp)
	{
		modelviewMat = p_modelview;
		projectionMat = p_projection;
		mvpMat = p_mvp;
	}

	void GLESQuadRenderer::setResourcePaths(char* p_texturePath, unsigned int p_imageFormat, const char* p_vertShaderPath, const char* p_fragShaderPath, size_t vlen, size_t flen)
	{
        texturePath = p_texturePath;
		vertShaderPath = p_vertShaderPath;
        fragShaderPath = p_fragShaderPath;
		vertQuadLen = (int)vlen;
		fragQuadLen = (int)flen;
		imageFormat = p_imageFormat;
        if(splashShader == nullptr)
		   splashShader = new Shader(vertShaderPath, fragShaderPath, &vertQuadLen, &fragQuadLen);
	}

	void GLESQuadRenderer::setQuadTex(const char* p_QuadTexPath, int p_width, int p_height, int p_len)
	{
		QuadTexPath = p_QuadTexPath;
		texBufLen = p_len;
		texWidth = p_width;
		texHeight = p_height;
	}
	
	bool GLESQuadRenderer::checkBuffers(const Group& group)
	{
		FloatBuffer* fGpuBuffer;

		if ((fGpuBuffer = dynamic_cast<FloatBuffer*>(group.getBuffer(GPU_BUFFER_NAME))) == NULL)
			return false;

		if (texturingMode != TEXTURE_NONE)
		{
			FloatBuffer* fTextureBuffer;

			if ((fTextureBuffer = dynamic_cast<FloatBuffer*>(group.getBuffer(TEXTURE_BUFFER_NAME,texturingMode))) == NULL)
				textureBuffer = createTextureBuffer(group);

			textureIterator = textureBuffer = fTextureBuffer->getData();
		}

		gpuIterator = gpuBuffer = fGpuBuffer->getData();
		return true;
	}

	void GLESQuadRenderer::createBuffers(const Group& group)
	{
		FloatBuffer* fBuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(GPU_BUFFER_NAME,FloatBufferCreator(28),0,false));
		gpuIterator = gpuBuffer = fBuffer->getData();
		if (texturingMode != TEXTURE_NONE)
			textureIterator = textureBuffer = createTextureBuffer(group);

		//std::cout << "error before generate commands: " << glGetError() << std::endl;
		splashShader->generateVBO();
		splashShader->generateTBO();
		splashShader->generateEBO();
		//std::cout << "error after generate commands: " << glGetError() << std::endl;
		splashShader->bindTBO();
		//std::cout << "error After first bind TBO commands: " << glGetError() << std::endl;
		//set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		int width, height, nrChannels;
		//unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
		unsigned char *data = stbi_load_from_memory((unsigned char*)QuadTexPath, texBufLen, &width, &height, &nrChannels, 0);
		if (data)
		{
			//std::cout << "error before set TBO data: " << glGetError() << std::endl;
			//splashShader->setTBOData(imageFormat, width, height, data);
			splashShader->setTBOData(imageFormat, width, height, data);
			//std::cout << "error after set TBO data: " << glGetError() << std::endl;
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}

	void GLESQuadRenderer::destroyBuffers(const Group& group)
	{
		group.destroyBuffer(GPU_BUFFER_NAME);
		group.destroyBuffer(TEXTURE_BUFFER_NAME);
	}

	float* GLESQuadRenderer::createTextureBuffer(const Group& group) const
	{
		FloatBuffer* fbuffer = NULL;

		switch(texturingMode)
		{
		case TEXTURE_2D :
			fbuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(TEXTURE_BUFFER_NAME,FloatBufferCreator(8),TEXTURE_2D,false));
			if (!group.getModel()->isEnabled(PARAM_TEXTURE_INDEX))
			{
				float t[8] = {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f};
				for (size_t i = 0; i < group.getParticles().getNbReserved() << 3; ++i)
					fbuffer->getData()[i] = t[i & 7];
			}
			break;

		case TEXTURE_3D :
			fbuffer = dynamic_cast<FloatBuffer*>(group.createBuffer(TEXTURE_BUFFER_NAME,FloatBufferCreator(12),TEXTURE_3D,false));
			float t[12] =  {1.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,1.0f,1.0f,0.0f};
			for (size_t i = 0; i < group.getParticles().getNbReserved() * 12; ++i)
				fbuffer->getData()[i] = t[i % 12];
			break;
		}

		return fbuffer->getData();
	}

	bool GLESQuadRenderer::setTexturingMode(TexturingMode mode)
	{
		if (mode == TEXTURE_3D)
			return false;

		texturingMode = mode;
		return true;
	}

	void GLESQuadRenderer::render(const Group& group)
	{
		if (!prepareBuffers(group))
			return;
		
		float oldModelView[16];
		for (int i = 0; i < 16; ++i)
			oldModelView[i] = modelView[i];
		//glGetFloatv(GL_MODELVIEW_MATRIX,modelView);
		const float *pSource = (const float*)(glm::value_ptr(modelviewMat));
		for (int i = 0; i < 16; ++i)
			modelView[i] = pSource[i];

		for (int i = 0; i < 16; ++i)
			if (oldModelView[i] != modelView[i])
			{
				invertModelView();
				break;
			}

		initBlending();
		initRenderingHints();

		//glShadeModel(GL_FLAT);

		switch(texturingMode)
		{
		case TEXTURE_2D :
			// if (getTexture3DGLExt() == SUPPORTED)
			// 	glDisable(GL_TEXTURE_3D);
			//std::cout << "error before enabling texture 2D: " << glGetError() << std::endl;
			//glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			//std::cout << "error before bind TBO commands: " << glGetError() << std::endl;
			splashShader->bindTBO();
			//std::cout << "error after bind TBO commands: " << glGetError() << std::endl;
			//glBindTexture(GL_TEXTURE_2D,textureIndex);
			//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,getTextureBlending());

			if (!group.getModel()->isEnabled(PARAM_TEXTURE_INDEX))
			{
				if (!group.getModel()->isEnabled(PARAM_ANGLE))
					renderParticle = &GLESQuadRenderer::render2D;
				else
					renderParticle = &GLESQuadRenderer::render2DRot;
			}
			else
			{
				if (!group.getModel()->isEnabled(PARAM_ANGLE))
					renderParticle = &GLESQuadRenderer::render2DAtlas;
				else
					renderParticle = &GLESQuadRenderer::render2DAtlasRot;
			}
			break;

		case TEXTURE_3D :
			glDisable(GL_TEXTURE_2D);
			//glEnable(GL_TEXTURE_3D);
			//glBindTexture(GL_TEXTURE_3D,textureIndex);
			//glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,getTextureBlending());

			if (!group.getModel()->isEnabled(PARAM_ANGLE))
				renderParticle = &GLESQuadRenderer::render3D;
			else
				renderParticle = &GLESQuadRenderer::render3DRot;
			break;

		case TEXTURE_NONE :
			glDisable(GL_TEXTURE_2D);
			// if (getTexture3DGLExt() == SUPPORTED)
			// 	glDisable(GL_TEXTURE_3D);
			if (!group.getModel()->isEnabled(PARAM_ANGLE))
				renderParticle = &GLESQuadRenderer::render2D;
			else
				renderParticle = &GLESQuadRenderer::render2DRot;
			break;
		}

		bool globalOrientation = precomputeOrientation3D(
			group,
			Vector3D(-invModelView[8],-invModelView[9],-invModelView[10]),
			Vector3D(invModelView[4],invModelView[5],invModelView[6]),
			Vector3D(invModelView[12],invModelView[13],invModelView[14]));

		totalNbParticles = group.getNbParticles();
		vertices = new float[totalNbParticles * 4 * 9]();
		//vertices = new float[totalNbParticles * 9]();
		indices = new unsigned int[totalNbParticles * 6]();
		//indices = new unsigned int[totalNbParticles]();

		if (globalOrientation)
		{
			computeGlobalOrientation3D();
			for (size_t i = 0; i < totalNbParticles; ++i)
				(this->*renderParticle)(group.getParticle(i), i);
		}
		else
		{
			for (size_t i = 0; i < totalNbParticles; ++i)
			{
				const Particle& particle = group.getParticle(i);
				computeSingleOrientation3D(particle);
				(this->*renderParticle)(particle, i);
			}
		}

		float SampTriVerts[] = {
         	-0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // top right
        	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom right
        	0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f // bottom left
    	};

		//std::cout << "error before set data commands: " << glGetError() << std::endl;
		splashShader->bindVBO();
		splashShader->setVBOData((sizeof(float) * totalNbParticles * 4 * 9), vertices);
		//splashShader->setVBOData((sizeof(float) * totalNbParticles * 9), vertices);
		//splashShader->setVBOData(sizeof(SampTriVerts), SampTriVerts);
		splashShader->bindEBO();
		splashShader->setEBOData((sizeof(unsigned int) * totalNbParticles * 6), indices);
		//splashShader->setEBOData((sizeof(unsigned int) * totalNbParticles), indices);

		//std::cout << "error before all set commands: " << glGetError() << std::endl;
		splashShader->use();
		//std::cout << "error after shader use cmd: " << glGetError() << std::endl;
		splashShader->setAttribute("aPos", 3, 9, 0);
		//splashShader->setAttribute("aPos", 3, 9, 0);
		//std::cout << "error after aPos set: " << glGetError() << std::endl;
		splashShader->setAttribute("aCol", 4, 9, 3);
		//splashShader->setAttribute("aCol", 4, 9, 3);
		//std::cout << "error after aCol set: " << glGetError() << std::endl;
		splashShader->setAttribute("aTexCoord", 2, 9, 7);
		//splashShader->setAttribute("aTexCoord", 2, 9, 7);
		//std::cout << "error after aTexCoord set: " << glGetError() << std::endl;
		splashShader->setInt("ourTexture", 0);
		//std::cout << "error after ourTexture set: " << glGetError() << std::endl;
		splashShader->setMatrix("modelview", modelviewMat);
		//std::cout << "error after modelview set: " << glGetError() << std::endl;
		splashShader->setMatrix("projection", projectionMat);
		//std::cout << "error after projection set: " << glGetError() << std::endl;
		splashShader->setMatrix("mvp", mvpMat);
		
		splashShader->bindTBO();
		splashShader->bindVBO();
		splashShader->bindEBO();

		//std::cout << "error before draw command: " << glGetError() << std::endl;
		glDrawElements(GL_TRIANGLES, (totalNbParticles*6), GL_UNSIGNED_INT, 0);
		//glDrawElements(GL_POINTS, totalNbParticles, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//std::cout << "error after draw command: " << glGetError() << std::endl;

		splashShader->unbindTBO();
		splashShader->unbindVBO();
		splashShader->unbindEBO();

		delete[] vertices;
		vertices = nullptr;
		delete[] indices;
		indices = nullptr;
		// glEnableClientState(GL_VERTEX_ARRAY);
		// glEnableClientState(GL_COLOR_ARRAY);

		// if (texturingMode == TEXTURE_2D)
		// {
		// 	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// 	glTexCoordPointer(2,GL_FLOAT,0,textureBuffer);
		// }
		// else if (texturingMode == TEXTURE_3D)
		// {
		// 	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		// 	glTexCoordPointer(3,GL_FLOAT,0,textureBuffer);
		// }

		// // interleaves vertex and color data
		// glVertexPointer(3,GL_FLOAT,7 * sizeof(float),gpuBuffer);
		// glColorPointer(4,GL_FLOAT,7 * sizeof(float),gpuBuffer + 3);

		// glDrawArrays(GL_QUADS,0,group.getNbParticles() << 2);

		// glDisableClientState(GL_VERTEX_ARRAY);
		// glDisableClientState(GL_COLOR_ARRAY);

		// if (texturingMode != TEXTURE_NONE)
		// 	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	void GLESQuadRenderer::render2D(const Particle& particle, const size_t iter)
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
	}

	void GLESQuadRenderer::render2DRot(const Particle& particle, const size_t iter)
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
	}

	void GLESQuadRenderer::render3D(const Particle& particle, const size_t iter)
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
		GLCallTexture3D(particle);
	}

	void GLESQuadRenderer::render3DRot(const Particle& particle, const size_t iter)
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
		GLCallTexture3D(particle);
	}

	void GLESQuadRenderer::render2DAtlas(const Particle& particle, const size_t iter)
	{
		scaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
		GLCallTexture2DAtlas(particle);
	}

	void GLESQuadRenderer::render2DAtlasRot(const Particle& particle, const size_t iter)
	{
		rotateAndScaleQuadVectors(particle,scaleX,scaleY);
		GLCallColorAndVertex(particle, iter);
		GLCallTexture2DAtlas(particle);
	}

	void GLESQuadRenderer::GLCallColorAndVertex(const Particle& particle, const size_t iter)
	{
		float x = particle.position().x;
		float y = particle.position().y;
		float z = particle.position().z;

		computeAtlasCoordinates(particle);

		// vertices[0 + 9 * iter] = x;
		// vertices[1 + 9 * iter] = y;
		// vertices[2 + 9 * iter] = z;
		// vertices[3 + 9 * iter] = particle.getR();
		// vertices[4 + 9 * iter] = particle.getG();
		// vertices[5 + 9 * iter] = particle.getB();
		// vertices[6 + 9 * iter] = particle.getParamCurrentValue(PARAM_ALPHA);
		// vertices[7 + 9 * iter] = textureAtlasU1();
		// vertices[8 + 9 * iter] = textureAtlasV0();

		// indices[iter] = (unsigned int)(iter);

		size_t shiftStep = 36 * iter;
		// quads are drawn in a counter clockwise order :
		// top right vertex

		vertices[0 + shiftStep] = x + quadSide().x + quadUp().x;
		vertices[1 + shiftStep] = y + quadSide().y + quadUp().y;
		vertices[2 + shiftStep] = z + quadSide().z + quadUp().z;
		vertices[3 + shiftStep] = particle.getR();
		vertices[4 + shiftStep] = particle.getG();
		vertices[5 + shiftStep] = particle.getB();
		vertices[6 + shiftStep] = particle.getParamCurrentValue(PARAM_ALPHA);
		vertices[7 + shiftStep] = textureAtlasU1();
		vertices[8 + shiftStep] = textureAtlasV0();
		// top left vertex
		vertices[9 + shiftStep] = x - quadSide().x + quadUp().x;
		vertices[10 + shiftStep] = y - quadSide().y + quadUp().y;
		vertices[11 + shiftStep] = z - quadSide().z + quadUp().z;
		vertices[12 + shiftStep] = particle.getR();
		vertices[13 + shiftStep] = particle.getG();
		vertices[14 + shiftStep] = particle.getB();
		vertices[15 + shiftStep] = particle.getParamCurrentValue(PARAM_ALPHA);
		vertices[16 + shiftStep] = textureAtlasU0();
		vertices[17 + shiftStep] = textureAtlasV0();
		// bottom left
		vertices[18 + shiftStep] = x - quadSide().x - quadUp().x;
		vertices[19 + shiftStep] = y - quadSide().y - quadUp().y;
		vertices[20 + shiftStep] = z - quadSide().z - quadUp().z;
		vertices[21 + shiftStep] = particle.getR();
		vertices[22 + shiftStep] = particle.getG();
		vertices[23 + shiftStep] = particle.getB();
		vertices[24 + shiftStep] = particle.getParamCurrentValue(PARAM_ALPHA);
		vertices[25 + shiftStep] = textureAtlasU0();
		vertices[26 + shiftStep] = textureAtlasV1();
		// bottom right
		vertices[27 + shiftStep] = x + quadSide().x - quadUp().x;
		vertices[28 + shiftStep] = y + quadSide().y - quadUp().y;
		vertices[29 + shiftStep] = z + quadSide().z - quadUp().z;
		vertices[30 + shiftStep] = particle.getR();
		vertices[31 + shiftStep] = particle.getG();
		vertices[32 + shiftStep] = particle.getB();
		vertices[33 + shiftStep] = particle.getParamCurrentValue(PARAM_ALPHA);
		vertices[34 + shiftStep] = textureAtlasU1();
		vertices[35 + shiftStep] = textureAtlasV1();

		//populate indices
		size_t indexJump = 6 * iter;
		indices[0 + indexJump] = (unsigned int)(4*iter+0);
		indices[1 + indexJump] = (unsigned int)(4*iter+1);
		indices[2 + indexJump] = (unsigned int)(4*iter+3);
		indices[3 + indexJump] = (unsigned int)(4*iter+1);
		indices[4 + indexJump] = (unsigned int)(4*iter+2);
		indices[5 + indexJump] = (unsigned int)(4*iter+3);
	}
}}
