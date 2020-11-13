#ifndef TINYRENDERER
#define TINYRENDERER

#include "includes.h"

enum MODE
{
FLAT,
SMOOTH,
TEXTURED
};

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

	double obj_load_time;
	double software_renderer_time;	
	double png_output_time;
	double texture_buffer_time;
		
	bool pquit;
	void quit();


	// main tinyrenderer funcs
	void load_OBJ(std::string path);
	void draw_wireframe();
	void draw_triangles();	

	// tinyrenderer helper funcs
	glm::vec3 barycentric(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 P);
	glm::vec3 world2screen(glm::vec3 v);
	glm::mat4 rotation(glm::vec3 a, float angle);
	glm::vec4 diffuse_texture_ref(glm::vec3 sample); 
		
	// tinyrenderer drawing functions
	void draw_line(glm::ivec2 p0, glm::ivec2 p1, glm::vec4 color);
	void draw_triangle(glm::vec3 *pts, glm::vec3 *normals, glm::vec3 *texcoords, glm::vec4 color);
	void set_pixel(glm::ivec2 p, glm::vec4 color);

public:
// OBJ data	
	// this may vary in length
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> texcoords;

	// these should all be the same length, the number of triangles
	std::vector<glm::ivec3> triangle_indices;
	std::vector<glm::ivec3> normal_indices;
	std::vector<glm::ivec3> texcoord_indices;


private:
	MODE current_mode = FLAT;
	// MODE current_mode = SMOOTH;
	// MODE current_mode = TEXTURED;

	unsigned diffuse_width, diffuse_height;
	std::vector<unsigned char> texture_diffuse;	

	// tinyrenderer draw target
	std::vector<unsigned char> image_data;
	std::vector<float> depth_data;
};

#endif
