#include <stdlib.h>
#include "header.h"

struct AttribBinding {
    const char * name;
    GLuint ID;
};

bool compileShader(GLuint *shader, GLenum type, const char *file) {
    GLint status;
    GLchar *source;
    
    FILE *f = fopen(file, "rb");
    if ( !f ) {
		std::cerr << "Cannot open file: " << file << std::endl;
        return false;
    }
	
    fseek(f, 0, SEEK_END);
    const size_t sz = ftell(f) + 1;
    source = (GLchar*)calloc(sz, 1);
	fseek(f, 0, SEEK_SET);
    fread(source, sz, 1, f);
    fclose(f);
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, (const GLchar **)&source, NULL);
    glCompileShader(*shader);
	free(source);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        std::cerr << "Shader compile log:\n" << log << std::endl;
        free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(*shader);
        return false;
    }
    
    return true;
}

bool linkProgram(GLuint prog) {
    GLint status;
    glLinkProgram(prog);
    
#if defined(DEBUG)
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        std::cerr << "Program link log:\n" << log << std::endl;
        free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0) {
        return false;
    }
    
    return true;
}

bool validateProgram(GLuint prog) {
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        std::cerr << "Program validate log:\n" << log << std::endl;
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == 0) {
        return false;
    }
    
    return true;
}

bool loadShaders(GLuint &program, const char *vertShaderPath, const char *fragShaderPath, const AttribBinding *bindings, const size_t nBindings) {
	GLuint vertShader, fragShader;
    program = glCreateProgram();
    
    if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertShaderPath)) {
        std::cerr << "Failed to compile vertex shader" << std::endl;
        return false;
    }
    
    if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderPath)) {
        std::cerr << "Failed to compile fragment shader" << std::endl;
        return false;
    }
    
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    
    for ( size_t i = 0; i < nBindings; ++i )
        glBindAttribLocation(program, bindings[i].ID, bindings[i].name);

    // Link program.
    if (!linkProgram(program)) {
        std::cerr << "Failed to link program: " << program << std::endl;
        
        if (vertShader) {
            glDeleteShader(vertShader);
            vertShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (program) {
            glDeleteProgram(program);
            program = 0;
        }
        
        return false;
    }

    // Release vertex and fragment shaders.
    if (vertShader) {
        glDetachShader(program, vertShader);
        glDeleteShader(vertShader);
    }
    if (fragShader) {
        glDetachShader(program, fragShader);
        glDeleteShader(fragShader);
    }
    
    return true;
}