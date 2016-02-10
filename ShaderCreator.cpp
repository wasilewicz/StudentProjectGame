#include "stdafx.h"
#include "ShaderCreator.h"
using namespace std;


// Funkcja, ktora ma za zadanie utworzyc shader danego typu (GL_VERTEX_SHADER/GL_FRAGMENT_SHADER)
// na podstawie kodu zrodlowego odczytanego z pliku o zadanej sciezce. Zwraca id shadera,
// ktore pozniej jest uzywane w celu podlaczenia shadera do programu cieniujacego.
GLuint ShaderCreator::CreateShader(GLenum type, char path[])
{

	// Odczytanie kodu zrodlowego z pliku:
	ifstream shaderFile;
	string * shaderCodeStr;
	shaderFile.open(path, ifstream::in);

	if (!shaderFile)
	{
		cerr << "Error while reading shader file (" << path << ").\n";
		return -1;
	}
	shaderCodeStr = new string(istreambuf_iterator<char>(shaderFile), istreambuf_iterator<char>());
	shaderFile.close();
	*shaderCodeStr += "\0";
	const char * shaderCode = shaderCodeStr->c_str();

	// Utworzenie shadera, przekazanie sterownikowi kodu zrodlowego oraz jego kompilacja:
	GLuint shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &shaderCode, NULL);
	glCompileShader(shaderId);

	// Kod shadera juz nie jest nam potrzebny w pamieci:
	delete shaderCode;

	// Sprawdzenie czy wystapily bledy kompilacji i ich wypisanie:
	int status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		cerr << "Error while compiling shader code (" << path << ").\n";
		GLchar *buf = new GLchar[512];
		glGetShaderInfoLog(shaderId, 512, NULL, buf);
		cerr << buf << "\n";
		delete[] buf;
		glDeleteShader(shaderId);
		return -1;
	}

	// Zwrocenie id shadera:
	return shaderId;
}


// Utworzenie programu cieniujacego skladajacego sie z Vertex i Fragment shaderow o zrodlach
// odczytanych z zadanych plikow. Zwraca id programu cieniujacego, ktore pozniej jest uzywane
// do jego wlaczenia.
GLuint ShaderCreator::CreateShadingProgram(char vsPath[], char fsPath[])
{

	// Stworzenie shaderow na podstawie zrodel z zadanych plikow:
	GLuint vs = CreateShader(GL_VERTEX_SHADER, vsPath);
	GLuint fs = CreateShader(GL_FRAGMENT_SHADER, fsPath);

	// Utworzenie nowego programu cieniujacego, OpenGL zwroci jego identyfikator:
	GLuint program = glCreateProgram();

	// Podlaczenie Vertex i Fragment shaderow do naszego nowego programu, korzystajac z id shaderow (pozniej juz nie beda nam potrzebne)
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	// Linkowanie programu po podlaczeniu shaderow:
	glLinkProgram(program);

	// Sprawdzenie czy wystapily bledy linkowania i ich wypisanie:
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		cerr << "Error while linking shading program (" << program << ").\n";
		GLchar *buf = new GLchar[512];
		glGetProgramInfoLog(program, 512, NULL, buf);
		cerr << buf << "\n";
		delete[] buf;
		return -1;
	}

	// Zwrocenie id programu cieniujacego, ktory bedzie potrzebny aby uzyc tego programu:
	return program;
}