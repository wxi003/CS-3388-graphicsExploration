// .hpp file for loading shaders
#ifndef SHADER_HPP
#define SHADER_HPP

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <GL/glew.h>

#include <stdlib.h>
#include <string.h>


GLuint LoadShaders(const char * vertex_file_path, const char * tessellation_control_file_path, const char * tessellation_evaluation_file_path, const char * geo_file_path, const char * fragment_file_path){

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    
    GLuint TessellationControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);

    GLuint TessellationEvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    std::ifstream TessellationControlShaderStream(tessellation_control_file_path, std::ios::in);
    std::ifstream TessellationEvaluationShaderStream(tessellation_evaluation_file_path, std::ios::in);
    std::ifstream GeometryShaderStream(geo_file_path, std::ios::in);
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    std::string VertexShaderCode, TessellationControlShaderCode, TessellationEvaluationShaderCode, GeometryShaderCode, FragmentShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
   

    auto readShaderCode = [](std::ifstream &shaderStream, std::string &shaderCode, const char *file_path) {
        if (shaderStream.is_open()) {
            std::stringstream sstr;
            sstr << shaderStream.rdbuf();
            shaderCode = sstr.str();
            shaderStream.close();
        } else {
            printf("Impossible to open %s. Are you in the right directory? Don't forget to read the FAQ!\n", file_path);
            getchar();
            return 0;
        }
        return 1;
    };

    readShaderCode(TessellationEvaluationShaderStream, TessellationEvaluationShaderCode, tessellation_evaluation_file_path);
    readShaderCode(GeometryShaderStream, GeometryShaderCode, geo_file_path);
    readShaderCode(FragmentShaderStream, FragmentShaderCode, fragment_file_path);
    readShaderCode(VertexShaderStream, VertexShaderCode, vertex_file_path);
    readShaderCode(TessellationControlShaderStream, TessellationControlShaderCode, tessellation_control_file_path);
    

    auto compileShader = [](GLuint shaderID, const char *source, const char *file_path) {
        printf("Compiling shader: %s\n", file_path);
        glShaderSource(shaderID, 1, &source, NULL);
        glCompileShader(shaderID);
    };

    auto checkShader = [](GLuint shaderID) {
        GLint Result, InfoLogLength;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(shaderID, InfoLogLength, NULL, &ErrorMessage[0]);
            printf("%s\n", &ErrorMessage[0]);
        }
    };


    compileShader(TessellationControlShaderID, TessellationControlShaderCode.c_str(), tessellation_control_file_path);
    checkShader(TessellationControlShaderID);

    compileShader(TessellationEvaluationShaderID, TessellationEvaluationShaderCode.c_str(), tessellation_evaluation_file_path);
    checkShader(TessellationEvaluationShaderID);

    compileShader(VertexShaderID, VertexShaderCode.c_str(), vertex_file_path);
    checkShader(VertexShaderID);

    compileShader(FragmentShaderID, FragmentShaderCode.c_str(), fragment_file_path);
    checkShader(FragmentShaderID);

    compileShader(GeometryShaderID, GeometryShaderCode.c_str(), geo_file_path);
    checkShader(GeometryShaderID);

    

    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, TessellationEvaluationShaderID);
    glAttachShader(ProgramID, GeometryShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, TessellationControlShaderID);
    

    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    // Detach shaders
    glDetachShader(ProgramID, TessellationEvaluationShaderID);
    glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, TessellationControlShaderID);


    // Delete shaders
    glDeleteShader(TessellationEvaluationShaderID);
    glDeleteShader(GeometryShaderID);
    glDeleteShader(FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(TessellationControlShaderID);
    

    // Return completed shader program
    return ProgramID;

}

#endif