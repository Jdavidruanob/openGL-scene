//=============================================================================
// Sample Application: Lighting (Per Fragment Phong)
//=============================================================================

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glApplication.h"
#include "glutWindow.h"
#include <iostream>
#include "glsl.h"
#include <time.h>
#include "glm.h"
#include <FreeImage.h> //*** Para Textura: Incluir librer�a
#include "Objeto3D.h" // la nueva clase
#include <vector>     // vectores para manejar la escena

//-----------------------------------------------------------------------------


class myWindow : public cwc::glutWindow
{
protected:
	// --- Miembros de la Clase Refactorizados ---

	cwc::glShaderManager SM;
	cwc::glShader* shader;           // Shader para objetos sin textura (ej. con solo color o material)
	cwc::glShader* shaderTextura;    // Shader para objetos que s� usan texturas
	std::vector<Objeto3D> escena; // Un �nico vector que contiene todos los objetos de nuestra escena.
	clock_t time0, time1;
	float timer010;  // timer counting 0->1->0
	bool bUp;        // flag if counting up or down.
	GLuint ProgramObject;


public:
	myWindow() {}

	virtual void OnInit()
	{
		// --- 1. Configuraci�n Inicial de OpenGL ---
		glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D); // Habilitamos el uso de texturas en general

		// --- 2. Carga de Shaders ---
		shader = SM.loadfromFile("vertexshader.txt", "fragmentshader.txt");
		if (shader == 0)
			std::cout << "Error al cargar el shader b�sico\n";

		shaderTextura = SM.loadfromFile("vertexshaderT.txt", "fragmentshaderT.txt");
		if (shaderTextura == 0)
			std::cout << "Error al cargar el shader de texturas\n";

		// --- 3. Creaci�n de Objetos y Composici�n de la Escena ---

		// prueba con objeto natural (Palmera)
		Objeto3D planta(shaderTextura); // Le decimos que use el shader de texturas
		planta.cargarMalla("./Mallas/CordylineFREE.obj");
		planta.agregarTextura("./Mallas/CordylineFREE_BaseColor.png");
		planta.posX = -1.5f; // Lo movemos a la izquierda
		planta.escalaX = planta.escalaY = planta.escalaZ = 0.9f; 
		escena.push_back(planta); // agregamos a la escena

		
		// aqui agregar de la misma manera los objetos que se deseen. 

		// --- 4. Inicializaci�n del Timer y la Luz ---
		time0 = clock();
		timer010 = 0.0f;
		bUp = true;
		DemoLight();
	}

	virtual void OnRender(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();
		glRotatef(timer010 * 360.0f, 0.0f, 1.0f, 0.0f);

		// Se usa un bucle para dibujar laos objetos de la escena
		for (int i = 0; i < escena.size(); ++i)
		{
			escena[i].dibujar(); // Cada objeto sabe c�mo dibujarse a s� mismo
		}

		glPopMatrix();
		glutSwapBuffers();
		UpdateTimer();
		Repaint();
	}

	virtual void OnIdle() {}

	virtual void OnResize(int w, int h)
	{
		if (h == 0) h = 1;
		float ratio = 1.0f * (float)w / (float)h;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glViewport(0, 0, w, h);

		gluPerspective(45, ratio, 1, 100);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(0.0f, 0.0f, 5.0f, // aumento la distancia de 4 a 5
			0.0, 0.0, -1.0,
			0.0f, 1.0f, 0.0f);
	}

	virtual void OnClose(void) {}
	virtual void OnMouseDown(int button, int x, int y) {}
	virtual void OnMouseUp(int button, int x, int y) {}
	virtual void OnMouseWheel(int nWheelNumber, int nDirection, int x, int y) {}

	virtual void OnKeyDown(int nKey, char cAscii)
	{
		if (cAscii == 27) // 0x1b = ESC
		{
			this->Close(); // Close Window!
		}
	};

	virtual void OnKeyUp(int nKey, char cAscii)
	{
		if (cAscii == 's')      // s: Shader
			shader->enable();
		else if (cAscii == 'f') // f: Fixed Function
			shader->disable();
	}

	void UpdateTimer()
	{
		time1 = clock();
		float delta = static_cast<float>(static_cast<double>(time1 - time0) / static_cast<double>(CLOCKS_PER_SEC));
		delta = delta / 4;
		if (delta > 0.00005f)
		{
			time0 = clock();
			if (bUp)
			{
				timer010 += delta;
				if (timer010 >= 1.0f) { timer010 = 1.0f; bUp = false; }
			}
			else
			{
				timer010 -= delta;
				if (timer010 <= 0.0f) { timer010 = 0.0f; bUp = true; }
			}
		}
	}

	void DemoLight(void)
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_NORMALIZE);

		/*// Light model parameters:
		// -------------------------------------------

		GLfloat lmKa[] = { 0.0, 0.0, 0.0, 0.0 };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);

		glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

		// -------------------------------------------
		// Spotlight Attenuation

		GLfloat spot_direction[] = { 1.0, -1.0, -1.0 };
		GLint spot_exponent = 30;
		GLint spot_cutoff = 180;

		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
		glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
		glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);

		GLfloat Kc = 1.0;
		GLfloat Kl = 0.0;
		GLfloat Kq = 0.0;

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, Kc);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);


		// -------------------------------------------
		// Lighting parameters:

		GLfloat light_pos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
		GLfloat light_Ka[] = { 1.0f, 0.5f, 0.5f, 1.0f };
		GLfloat light_Kd[] = { 1.0f, 0.1f, 0.1f, 1.0f };
		GLfloat light_Ks[] = { 1.0f, 1.0f, 1.0f, 1.0f };

		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

		// -------------------------------------------
		// Material parameters:

		GLfloat material_Ka[] = { 0.5f, 0.0f, 0.0f, 1.0f };
		GLfloat material_Kd[] = { 0.4f, 0.4f, 0.5f, 1.0f };
		GLfloat material_Ks[] = { 0.8f, 0.8f, 0.0f, 1.0f };
		GLfloat material_Ke[] = { 0.1f, 0.0f, 0.0f, 0.0f };
		GLfloat material_Se = 20.0f;

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);*/
	}
};

//-----------------------------------------------------------------------------

class myApplication : public cwc::glApplication
{
public:
	virtual void OnInit() {std::cout << "Hello World!\n"; }
};

//-----------------------------------------------------------------------------

int main(void)
{
	myApplication*  pApp = new myApplication;
	myWindow* myWin = new myWindow();

	pApp->run();
	delete pApp;
	return 0;
}

//-----------------------------------------------------------------------------

