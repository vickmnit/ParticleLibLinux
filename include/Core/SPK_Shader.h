#ifndef H_SPK_SHADER
#define H_SPK_SHADER

#include <iostream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <GLES2/gl2.h>
using namespace std;

namespace SPK
{
    namespace GLES
    {
    class Shader
    {
       public:
    unsigned int ID;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int VAO;
    unsigned int TBO;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, int *vlen, int *flen);
    ~Shader();
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    void Delete();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const string &name, bool value) const;
    // ------------------------------------------------------------------------
    void setInt(const string &name, int value) const;
    // ------------------------------------------------------------------------
    void setFloat(const string &name, float value) const;

    void setVBOData(size_t size, float data[]);
    void setEBOData(size_t size, unsigned int data[]);
    void setTBOData(unsigned int type, int width, int height, unsigned char *data);
    void setAttribute(const std::string &attrname, int count, int stride, int offset);
    void setMatrix(const std::string &attrname, glm::mat4 matrix);

    void generateVAO();
    void generateVBO();
    void generateEBO();
    void generateTBO();
    void genMipMap();

    void bindTBO();
    void bindVBO();
    void bindEBO();
    void bindVAO();

    void unbindVAO();
    void unbindVBO();
    void unbindEBO();
    void unbindTBO();


    void deleteVBO();
    void deleteEBO();

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
    };
    }
} // namespace SPK
#endif

