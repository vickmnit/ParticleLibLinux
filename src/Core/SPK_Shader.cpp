#include "Core/SPK_Shader.h"
//#include <GLES2/gl2.h>
#include <string>
#include <fstream>
#include <sstream>

namespace SPK
{
    namespace GLES
    {
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader::Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
       /*  std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str(); */
        const char* vShaderCode = vertexPath;
        const char * fShaderCode = fragmentPath;
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    Shader::~Shader()
    {
        //glDeleteProgram(ID);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void Shader::use() 
    { 
        glUseProgram(ID); 
    }

    void Shader::Delete() 
    { 
        glDeleteProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void Shader::setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void Shader::setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void Shader::checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void Shader::setAttribute(const std::string &attrname, int count, int stride, int offset)
    {
        GLint Loc = glGetAttribLocation(ID, attrname.c_str());
		glVertexAttribPointer(Loc, count, GL_FLOAT, GL_TRUE, stride * sizeof(float), (void*)(offset * sizeof(float)));
		glEnableVertexAttribArray(Loc);
    }

    void Shader::setMatrix(const std::string &attrname, glm::mat4 matrix)
    {
        unsigned int matLoc = glGetUniformLocation(ID, attrname.c_str());
	    glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void Shader::setVBOData(size_t size, float data[])
    {
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void Shader::setEBOData(size_t size, unsigned int data[])
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    }

    void Shader::setTBOData(unsigned int type, int width, int height, unsigned char *data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
    }

    void Shader::generateVAO()
    {
        //glGenVertexArrays(1, &VAO);
    }

    void Shader::bindVAO()
    {
        //glBindVertexArray(VAO);
    }

    void Shader::unbindVAO()
    {
        //glBindVertexArray(0);
    }

    void Shader::generateTBO()
    {
        glGenTextures(1, &TBO);
    }

    void Shader::generateVBO()
    {
        glGenBuffers(1, &VBO);
    }

    void Shader::generateEBO()
    {
        glGenBuffers(1, &EBO);
    }

    void Shader::genMipMap()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void Shader::bindTBO()
    {
        glBindTexture(GL_TEXTURE_2D, TBO);
    }

    void Shader::bindVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    void Shader::bindEBO()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    }

    void Shader::unbindVBO()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Shader::unbindEBO()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Shader::unbindTBO()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Shader::deleteVBO()
    {
        glDeleteBuffers(1, &VBO);
    }

    void Shader::deleteEBO()
    {
        glDeleteBuffers(1, &EBO);
    }
    }
}
