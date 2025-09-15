#include "Objeto3D.h" 

// Constructor 
Objeto3D::Objeto3D(cwc::glShader* shaderUtilizado) {
    malla = nullptr;
    shader = shaderUtilizado;

    // Inicializar transformaciones
    posX = posY = posZ = 0.0f;
    rotX = rotY = rotZ = 0.0f;
    escalaX = escalaY = escalaZ = 1.0f;
}


// Carga la malla desde un archivo .obj
bool Objeto3D::cargarMalla(const std::string& rutaArchivo) {
    // La función glmReadOBJ espera un 'char*', pero c_str() devuelve un 'const char*'.
    // Creamos una copia mutable para asegurar la compatibilidad.
    std::vector<char> rutaMutable(rutaArchivo.begin(), rutaArchivo.end());
    rutaMutable.push_back('\0'); // Es crucial agregar el carácter nulo al final

    // Ahora le pasamos el puntero al inicio de nuestro vector
    malla = glmReadOBJ(rutaMutable.data());

    if (!malla) {
        std::cerr << "Error: No se pudo cargar la malla " << rutaArchivo << std::endl;
        return false;
    }

    // Normalizamos el tamaño y calculamos las normales (con el mismo codigo base que ya teniamos)
    glmUnitize(malla);
    glmFacetNormals(malla);
    glmVertexNormals(malla, 90.0);

    std::cout << "Malla " << rutaArchivo << " cargada correctamente." << std::endl;
    return true;
}

// Carga una textura y la añade a nuestro objeto
bool Objeto3D::agregarTextura(const std::string& rutaArchivo) {
    GLuint texturaID = cargarTextura(rutaArchivo.c_str());
    if (texturaID == 0) {
        std::cerr << "Error: No se pudo cargar la textura " << rutaArchivo << std::endl;
        return false;
    }

    idsTexturas.push_back(texturaID);
    std::cout << "Textura " << rutaArchivo << " agregada correctamente." << std::endl;
    return true;
}

// Dibuja el objeto en la escena
void Objeto3D::dibujar() {
    if (!malla) return; // No dibujar si no hay malla

    if (shader) shader->begin();

    glPushMatrix();
    // 1. Aplicar transformaciones: Traslación, Rotación, Escala
    glTranslatef(posX, posY, posZ);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    glScalef(escalaX, escalaY, escalaZ);

    // 2. Aplicar texturas si las hay
    if (!idsTexturas.empty()) {
        // Por ahora, solo habilitamos para una unica textura.
        glBindTexture(GL_TEXTURE_2D, idsTexturas[0]);
        glmDraw(malla, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
    }
    else {
        // Si no hay textura, lo dibujamos solo con el material
        glmDraw(malla, GLM_SMOOTH | GLM_MATERIAL);
    }

    glPopMatrix();

    if (shader) shader->end();
}

// Función auxiliar para cargar una imagen y crear una textura en OpenGL
GLuint Objeto3D::cargarTextura(const char* nombreArchivo) {
    FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(nombreArchivo, 0), nombreArchivo);
    if (!bitmap) {
        return 0;
    }

    FIBITMAP* pImage = FreeImage_ConvertTo32Bits(bitmap);
    int nWidth = FreeImage_GetWidth(pImage);
    int nHeight = FreeImage_GetHeight(pImage);
    FreeImage_Unload(bitmap);

    GLuint texturaID;
    glGenTextures(1, &texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

    FreeImage_Unload(pImage);

    return texturaID;
}