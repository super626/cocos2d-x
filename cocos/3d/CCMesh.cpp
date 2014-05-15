
#include "CCMesh.h"

#include <list>
#include <fstream>
#include <iostream>
#include <sstream>

#include "base/ccMacros.h"
#include "renderer/ccGLStateCache.h"
#include "CCMeshPart.h"
#include "CCObjLoader.h"
#include "CCSprite3DDataCache.h"

using namespace std;

NS_CC_BEGIN

bool RenderMeshData::hasVertexAttrib(int attrib)
{
    for (auto itr = _vertexAttribs.begin(); itr != _vertexAttribs.end(); itr++) {
        if ((*itr).vertexAttrib == attrib)
            return true; //already has
    }
    return false;
}

bool RenderMeshData::initFrom(std::vector<float>& posions, std::vector<float>& normals, std::vector<float>& texs, const std::vector<unsigned short>& indices)
{
    CC_ASSERT(posions.size()<65536 * 3 && "index may out of bound");
    
    _vertexAttribs.clear();
    vertexsizeBytes = 0;
    
    vertexNum = posions.size() / 3; //number of vertex
    if (vertexNum == 0)
        return false;
    
    if ((normals.size() != 0 && vertexNum * 3 != normals.size()) || (texs.size() != 0 && vertexNum * 2 != texs.size()))
        return false;
    
    vertexsizeBytes += 3;
    MeshVertexAttrib meshvertexattrib;
    meshvertexattrib.size = 3;
    meshvertexattrib.type = GL_FLOAT;
    meshvertexattrib.attribSizeBytes = meshvertexattrib.size * sizeof(float);
    meshvertexattrib.vertexAttrib = GLProgram::VERTEX_ATTRIB_POSITION;
    _vertexAttribs.push_back(meshvertexattrib);
    
    //normal
    if (normals.size())
    {
        //add normal flag
        vertexsizeBytes += 3;
        meshvertexattrib.vertexAttrib = GLProgram::VERTEX_ATTRIB_NORMAL;
        _vertexAttribs.push_back(meshvertexattrib);
    }
    //
    if (texs.size())
    {
        vertexsizeBytes += 2;
        meshvertexattrib.size = 2;
        meshvertexattrib.vertexAttrib = GLProgram::VERTEX_ATTRIB_TEX_COORD;
        meshvertexattrib.attribSizeBytes = meshvertexattrib.size * sizeof(float);
        _vertexAttribs.push_back(meshvertexattrib);
    }
    
    _vertexs.clear();
    _vertexs.reserve(vertexNum * vertexsizeBytes);
    vertexsizeBytes *= sizeof(float);
    
    //position, normal, texCoordinate into _vertexs
    for(int i = 0; i < vertexNum; i++)
    {
        _vertexs.push_back(posions[i * 3]);
        _vertexs.push_back(posions[i * 3 + 1]);
        _vertexs.push_back(posions[i * 3 + 2]);
        
        if (hasVertexAttrib(GLProgram::VERTEX_ATTRIB_NORMAL))
        {
            _vertexs.push_back(normals[i * 3]);
            _vertexs.push_back(normals[i * 3 + 1]);
            _vertexs.push_back(normals[i * 3 + 2]);
        }
        
        if (hasVertexAttrib(GLProgram::VERTEX_ATTRIB_TEX_COORD))
        {
            _vertexs.push_back(texs[i * 2]);
            _vertexs.push_back(texs[i * 2 + 1]);
        }
    }
    _indices = indices;
    
    return true;
}


Mesh::Mesh()
:_vertexBuffer(0)
, _parts(nullptr)
, _partCount(0)
{
    

}

Mesh::~Mesh()
{
    releaseMeshPart();
    
    freeBuffers();
}

void Mesh::releaseMeshPart()
{
//    for (unsigned int i = 0; i < _partCount; ++i)
//    {
//        delete _parts[i];
//    }
//    delete _parts;
}

Mesh* Mesh::create(std::vector<float>& posions, std::vector<float>& normals, std::vector<float>& texs, const std::vector<unsigned short>& indices)
{
    auto mesh = new Mesh();
    if(mesh && mesh->init(posions, normals, texs, indices))
    {
        mesh->autorelease();
        return mesh;
    }
    CC_SAFE_DELETE(mesh);
    return nullptr;
}

bool Mesh::init(std::vector<float>& posions, std::vector<float>& normals, std::vector<float>& texs, const std::vector<unsigned short>& indices)
{
    
    bool bRet = _renderdata.initFrom(posions, normals, texs, indices);
    if (!bRet)
        return false;
    
    restore();
    
    return true;
}

void Mesh::freeBuffers()
{
    if(glIsBuffer(_vertexBuffer))
    {
        glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
    }
}

void Mesh::buildBuffer()
{
    freeBuffers();

    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    
    glBufferData(GL_ARRAY_BUFFER,
                 _renderdata._vertexs.size() * sizeof(_renderdata._vertexs[0]),
                 &_renderdata._vertexs[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//void Mesh::addMeshPart(PrimitiveType primitiveType, IndexFormat indexformat,  void* indexData, unsigned int indexCount)
//{
//    MeshPart* part = MeshPart::create(this, _partCount, primitiveType, indexformat, indexData, indexCount);
//    if (part)
//    {
//        // Increase size of part array and copy old subets into it.
//        MeshPart** oldParts = _parts;
//        _parts = new MeshPart*[_partCount + 1];
//        for (unsigned int i = 0; i < _partCount; ++i)
//        {
//            _parts[i] = oldParts[i];
//        }
//        
//        // Add new part to array.
//        _parts[_partCount++] = part;
//        
//        // Delete old part array.
//        delete[] (oldParts);
//    }
//}

void Mesh::restore()
{
    releaseMeshPart();
    
    buildBuffer();
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    
    unsigned int indexSize = 2;
    IndexFormat indexformat = IndexFormat_INDEX16;
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * _renderdata._indices.size(), &_renderdata._indices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    _primitiveType = PrimitiveType_TRIANGLES;
    _indexFormat = indexformat;
    _indexCount = _renderdata._indices.size();
    _indexBuffer = vbo;
    
}

NS_CC_END
