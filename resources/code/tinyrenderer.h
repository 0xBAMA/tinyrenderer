#ifndef TINYRENDERER
#define TINYRENDERER

#include "includes.h"

class tinyrenderer
{
public:

	tinyrenderer();
	~tinyrenderer();

private:

	SDL_Window * window;
	SDL_GLContext GLcontext;

	ImVec4 clear_color;

	GLuint display_shader;
	GLuint display_vao;
	GLuint display_vbo;
	GLuint display_texture;

    std::vector<unsigned char> image_data;

	void create_window();
	void gl_setup();
	void draw_everything();

	void quit();

	bool pquit;

};

#endif
