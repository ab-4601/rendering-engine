#pragma once

#include "Core.h"

class Shader {
private:
	char* vertexShader;
	char* fragmentShader;
	char* geometryShader;

	std::string vertFileName, fragFileName, geomFileName;
	GLuint vertexShaderID, fragmentShaderID, geometryShaderID, programID;

public:
	Shader(std::string vertFileName = "", std::string fragFileName = "", std::string geomFileName = "");

	void readFile(std::string fileName, char*& shader);
	void compileShader(GLuint& shaderID, const char* shader, GLenum shaderType);
	void attachShader(std::vector<GLuint> shaderIDs);

	inline void useProgram() const { glUseProgram(this->programID); }
	inline void endProgram() const { glUseProgram(0); }

	inline void setInt(const char* varName, const int& value) const {
		glUniform1i(glGetUniformLocation(this->programID, varName), value);
	}

	inline void setUint(const char* varName, const uint& value) const {
		glUniform1ui(glGetUniformLocation(this->programID, varName), value);
	}

	inline void setFloat(const char* varName, const float& value) const {
		glUniform1f(glGetUniformLocation(this->programID, varName), value);
	}

	inline void setVec1(const char* varName, const glm::vec1& value) const {
		glUniform1fv(glGetUniformLocation(this->programID, varName), 1, glm::value_ptr(value));
	}

	inline void setVec2(const char* varName, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(this->programID, varName), 1, glm::value_ptr(value));
	}

	inline void setVec3(const char* varName, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(this->programID, varName), 1, glm::value_ptr(value));
	}

	inline void setVec4(const char* varName, const glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(this->programID, varName), 1, glm::value_ptr(value));
	}

	inline void setMat3(const char* varName, const glm::mat3& value) const {
		glUniformMatrix3fv(glGetUniformLocation(this->programID, varName), 1, GL_FALSE, glm::value_ptr(value));
	}

	inline void setMat4(const char* varName, const glm::mat4& value) const {
		glUniformMatrix4fv(glGetUniformLocation(this->programID, varName), 1, GL_FALSE, glm::value_ptr(value));
	}

	~Shader();
};