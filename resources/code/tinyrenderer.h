#ifndef TINYRENDERER
#define TINYRENDERER

#include "includes.h"

class tinyrenderer
{
public:
	tinyrenderer();
	~tinyrenderer();

private:
	// administrative
	SDL_Window * window;
	SDL_GLContext GLcontext;
	ImVec4 clear_color;

	GLuint display_shader;
	GLuint display_vao;
	GLuint display_vbo;
	GLuint display_texture;

	void create_window();
	void gl_setup();
	void draw_everything();

	float obj_load_time;
	float software_renderer_time;	
	float png_output_time;
	float texture_buffer_time;
		
	bool pquit;
	void quit();


	// main tinyrenderer funcs
	void draw_wireframe(std::string path);
	void draw_triangles(std::string path);	

	// tinyrenderer helper funcs
	void draw_line();
	void draw_triangle();
	void set_pixel();
		
	// tinyrenderer draw target
	std::vector<unsigned char> image_data;
};

#endif
