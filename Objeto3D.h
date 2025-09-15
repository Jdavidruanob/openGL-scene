#pragma once // Evita que el header se incluya m�ltiples veces

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <string>
#include "glm.h"       // Para GLMmodel
#include <FreeImage.h> // Para cargar texturas
#include "glsl.h"      // <--- A�ADE ESTA L�NEA. Ahora s� reconocer� cwc::glShader

class Objeto3D {
public:
    // Miembros de transformaci�n
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float escalaX, escalaY, escalaZ;

private:
    GLMmodel* malla;
    std::vector<GLuint> idsTexturas; // Un vector para manejar m�ltiples texturas
    cwc::glShader* shader; // Cada objeto puede tener su propio shader

public:
    // Constructor
    Objeto3D(cwc::glShader* shaderUtilizado);

    // M�todos principales
    bool cargarMalla(const std::string& rutaArchivo);
    bool agregarTextura(const std::string& rutaArchivo);
    void dibujar();

private:
    // M�todo privado para la carga de texturas, para no repetir c�digo
    GLuint cargarTextura(const char* nombreArchivo);
};