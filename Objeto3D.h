#pragma once // Evita que el header se incluya múltiples veces

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <string>
#include "glm.h"       // Para GLMmodel
#include <FreeImage.h> // Para cargar texturas
#include "glsl.h"      // <--- AÑADE ESTA LÍNEA. Ahora sí reconocerá cwc::glShader

class Objeto3D {
public:
    // Miembros de transformación
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float escalaX, escalaY, escalaZ;

private:
    GLMmodel* malla;
    std::vector<GLuint> idsTexturas; // Un vector para manejar múltiples texturas
    cwc::glShader* shader; // Cada objeto puede tener su propio shader

public:
    // Constructor
    Objeto3D(cwc::glShader* shaderUtilizado);

    // Métodos principales
    bool cargarMalla(const std::string& rutaArchivo);
    bool agregarTextura(const std::string& rutaArchivo);
    void dibujar();

private:
    // Método privado para la carga de texturas, para no repetir código
    GLuint cargarTextura(const char* nombreArchivo);
};