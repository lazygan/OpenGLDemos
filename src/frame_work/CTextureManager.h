/*
* File:   textureManager.h
* Desc:		Trida pro spravu textur
*
*/
#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H


using namespace std;

class CTextureManager {
public:
	CTextureManager();
	~CTextureManager();
	GLuint operator[](const string& texture);
	void add(const string& texture);
	void createTexture(const string& texture, const string filePath, unsigned w, unsigned h, GLuint filter, GLuint type, GLuint type_2, bool depth);
	void createRGBA16F3DTexture(const string& texture, glm::vec3 dim, GLuint filter, GLuint wrap);
	void createRGBA3DTexture(const string& texture, glm::vec3 dim, GLuint filter, GLuint wrap);
	void clear3Dtexture(GLuint texture);
	void setClearTextureExtension(bool v) {
		clearTextureExtension = v;
	}
private:
	map<string,GLuint> textures;
	bool clearTextureExtension;
};

#endif