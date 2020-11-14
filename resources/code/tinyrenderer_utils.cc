#include "tinyrenderer.h"
// This contains the lower level code

//TinyOBJLoader implementation define - This has to be included in a .cc file, so it's here for right now
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// tinyobj callbacks
//  user_data is passed in as void, then cast as 'tinyrenderer' class to push vertices, normals, texcoords, index, material info
void vertex_cb(void *user_data, float x, float y, float z, float w)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data);

    t->vertices.push_back(glm::vec4(x,y,z,w));
}

void normal_cb(void *user_data, float x, float y, float z) 
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 

    t->normals.push_back(glm::vec3(x,y,z));
}

void texcoord_cb(void *user_data, float x, float y, float z)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 

    t->texcoords.push_back(glm::vec3(x,y,z));
}

void index_cb(void *user_data, tinyobj::index_t *indices, int num_indices)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 

    if(num_indices == 3) // this is a triangle
    {
        // OBJ uses 1-indexing, convert to 0-indexing
        t->triangle_indices.push_back(glm::ivec3(indices[0].vertex_index-1,   indices[1].vertex_index-1,   indices[2].vertex_index-1));
        t->normal_indices.push_back(  glm::ivec3(indices[0].normal_index-1,   indices[1].normal_index-1,   indices[2].normal_index-1));
        t->texcoord_indices.push_back(glm::ivec3(indices[0].texcoord_index-1, indices[1].texcoord_index-1, indices[2].texcoord_index-1));
    }
    
    // lines, points have a different number of indicies
    //  might want to handle these
}

void usemtl_cb(void *user_data, const char *name, int material_idx)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 
}

void mtllib_cb(void *user_data, const tinyobj::material_t *materials, int num_materials)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 
}

void group_cb(void *user_data, const char **names, int num_names)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 
}

void object_cb(void *user_data, const char *name)
{
    tinyrenderer *t = reinterpret_cast<tinyrenderer *>(user_data); 
}

// this is where the callbacks are used
void tinyrenderer::load_OBJ(std::string filename)
{
    tinyobj::callback_t cb;
    cb.vertex_cb = vertex_cb;
    cb.normal_cb = normal_cb;
    cb.texcoord_cb = texcoord_cb;
    cb.index_cb = index_cb;
    cb.usemtl_cb = usemtl_cb;
    cb.mtllib_cb = mtllib_cb;
    cb.group_cb = group_cb;
    cb.object_cb = object_cb;

    std::string warn;
    std::string err;

    std::ifstream ifs(filename.c_str());
    tinyobj::MaterialFileReader mtlReader(".");

    bool ret = tinyobj::LoadObjWithCallback(ifs, cb, this, &mtlReader, &warn, &err);

    if (!warn.empty())
    {
        std::cout << "WARN: " << warn << std::endl;
    }

    if (!err.empty())
    {
        std::cerr << err << std::endl;
    }

    if (!ret)
    {
        std::cerr << "Failed to parse .obj" << std::endl;
    }

    cout << "vertex list length: " << vertices.size() << endl;
    cout << "normal list length: " << normals.size() << endl;
    cout << "texcoord list length: " << texcoords.size() << endl;

    cout << "vertex index list length: " << triangle_indices.size() << endl;
    cout << "normal index length: " << normal_indices.size() << endl;
    cout << "texcoord index length: " << texcoord_indices.size() << endl;
}





void tinyrenderer::create_window()
{
    if(SDL_Init( SDL_INIT_EVERYTHING ) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
    }

    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 8);

// this is how you query the screen resolution
    SDL_DisplayMode dm;
    SDL_GetDesktopDisplayMode(0, &dm);

// pulling these out because I'm going to try to span the whole screen with
// the window, in a way that's flexible on different resolution screens
    int total_screen_width = dm.w;
    int total_screen_height = dm.h;

    cout << "creating window...";

// window = SDL_CreateWindow( "OpenGL Window", 0, 0, total_screen_width, total_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS );
    window = SDL_CreateWindow( "OpenGL Window", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE );
    SDL_ShowWindow(window);
    // SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    
    cout << "done." << endl;


    cout << "setting up OpenGL context...";
// OpenGL 4.3 + GLSL version 430
    const char* glsl_version = "#version 430";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    GLcontext = SDL_GL_CreateContext( window );

    SDL_GL_MakeCurrent(window, GLcontext);
    SDL_GL_SetSwapInterval(1); // Enable vsync
// SDL_GL_SetSwapInterval(0); // explicitly disable vsync

    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);

    glPointSize(3.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning

    ImGui::StyleColorsDark();

// Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, GLcontext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    clear_color = ImVec4(75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f); // initial value for clear color

// really excited by the fact imgui has an hsv picker to set this
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow( window );

    cout << "done." << endl;

    ImVec4* colors = ImGui::GetStyle().Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.67f, 0.50f, 0.16f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.33f, 0.27f, 0.16f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.05f, 0.00f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.23f, 0.17f, 0.02f, 0.05f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.12f, 0.07f, 0.01f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.18f, 0.09f, 0.33f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.33f, 0.15f, 0.02f, 0.17f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.33f, 0.15f, 0.02f, 0.17f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.09f, 0.02f, 0.17f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.25f, 0.12f, 0.01f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.25f, 0.12f, 0.01f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.07f, 0.02f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.13f, 0.10f, 0.08f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.69f, 0.45f, 0.11f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.28f, 0.18f, 0.06f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.36f, 0.22f, 0.06f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.18f, 0.06f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.42f, 0.18f, 0.06f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.10f, 0.05f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.25f, 0.12f, 0.01f, 0.78f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.34f, 0.14f, 0.01f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.33f, 0.15f, 0.02f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.42f, 0.18f, 0.06f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.06f, 0.03f, 0.01f, 0.78f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.64f, 0.42f, 0.09f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.64f, 0.42f, 0.09f, 0.90f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void tinyrenderer::gl_setup()
{
// some info on your current platform
    const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
    const GLubyte *version = glGetString( GL_VERSION );// OpenGL version as a string
    printf( "Renderer: %s\n", renderer );
    printf( "OpenGL version supported %s\n\n\n", version );



    // create the shader for the triangles to cover the screen
    display_shader = Shader("resources/code/shaders/blit.vs.glsl", "resources/code/shaders/blit.fs.glsl").Program;

    // set up the points for the display
    //  A---------------B
    //  |          .    |
    //  |       .       |
    //  |    .          |
    //  |               |
    //  C---------------D

    // diagonal runs from C to B
    //  A is -1, 1
    //  B is  1, 1
    //  C is -1,-1
    //  D is  1,-1
    std::vector<glm::vec3> points;

    points.clear();
    points.push_back(glm::vec3(-1, 1, 0.5));  //A
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1, 1, 0.5));  //B

    points.push_back(glm::vec3( 1, 1, 0.5));  //B
    points.push_back(glm::vec3(-1,-1, 0.5));  //C
    points.push_back(glm::vec3( 1,-1, 0.5));  //D

    // vao, vbo
    cout << "  setting up vao, vbo for display geometry...........";
    glGenVertexArrays( 1, &display_vao );
    glBindVertexArray( display_vao );

    glGenBuffers( 1, &display_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, display_vbo );
    cout << "done." << endl;

    // buffer the data
    cout << "  buffering vertex data..............................";
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * points.size(), &points[0]);
    cout << "done." << endl;

    // set up attributes
    cout << "  setting up attributes in display shader............";
    GLuint points_attrib = glGetAttribLocation(display_shader, "vPosition");
    glEnableVertexAttribArray(points_attrib);
    glVertexAttribPointer(points_attrib, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (static_cast<const char*>(0) + (0)));
    cout << "done." << endl;


    ///////////////////////
    
    auto t1 = std::chrono::high_resolution_clock::now();

    //obj loading
    load_OBJ(std::string("resources/obj/african_head/african_head.obj"));
    
    auto t2 = std::chrono::high_resolution_clock::now();
    obj_load_time = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    ///////////////////////
    
    t1 = std::chrono::high_resolution_clock::now();

    clear_buffers(); // zero out color, depth
    
    unsigned error = lodepng::decode(texture_diffuse, diffuse_width, diffuse_height, std::string("resources/obj/african_head/african_head_diffuse.png").c_str());
    // unsigned error = lodepng::decode(texture_diffuse, diffuse_width, diffuse_height, std::string("resources/obj/grid.png").c_str());

    //report any errors
    if(error) cout << "decode error (diffuse texture) " << error << ": " << lodepng_error_text(error) << endl; 
    
    draw_triangles();
    
    t2 = std::chrono::high_resolution_clock::now();
    software_renderer_time = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    ///////////////////////
    
    t1 = std::chrono::high_resolution_clock::now();

    //png output
    output_frame(std::string("test.png"));
    
    t2 = std::chrono::high_resolution_clock::now();
    png_output_time = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    ///////////////////////
    
    t1 = std::chrono::high_resolution_clock::now();

    // create the image textures
    glGenTextures(1, &display_texture);
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, display_texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // buffer the averaged data to the GPU
    buffer_GPU_texture();
    
    t2 = std::chrono::high_resolution_clock::now();
    texture_buffer_time = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

}


static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void tinyrenderer::draw_everything()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io; // void cast prevents unused variable warning
    //i.e. to get the screen dimensions and pass in as uniforms


    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);   // from hsv picker
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                     // clear the background


    static int index = 0; // indexed output

    // start frame timing
    auto t1 = std::chrono::high_resolution_clock::now();

    if(index < 400)
    {
        // update transforms, light position
        transform = glm::mat4(0.95f); // initialize with identity, scaled down slightly
        // transform = rotation(glm::vec3(0,1,0), 0.69+(6.28 * (index/400.0))) * rotation(glm::vec3(1,0,0), 0.1*sin(6.28*2*(index/400.0))) * transform; // compose transforms into one
        transform = rotation(glm::vec3(1,0,0), -0.5-0.15*sin(6.28*2*(index/400.0))) * rotation(glm::vec3(0,1,0), 0.69+(6.28*(index/400.0))) * transform; // compose transforms into one

        ntransform = glm::transpose(glm::inverse(transform)); // transpose of the inverse of the model transform is used for the normals

        transform = glm::lookAt(glm::vec3(2,2,-4), glm::vec3(0,0,0), glm::vec3(0,1,0)) * transform;   // view matrix
        transform = glm::perspective(glm::radians(90.0f), float(WIDTH)/float(HEIGHT), 0.25f, 10.0f) * transform; // perspective transform 
    

        light_position = glm::normalize(glm::vec3(1+4.0*sin(6.28*2*(index/400.0)),8+3.0*sin(6.28*(index/400.0)),9));
    
        // clear the color and depth buffers
        clear_buffers();
    
        // draw the model
        draw_triangles();
    
        // output PNG frame to frames/frameXXX.png
        std::stringstream ss;
        ss << "frames/frame" << std::setfill('0') << std::setw(3) << index << ".png";
        output_frame(ss.str());
    
        // buffer texture
        buffer_GPU_texture();
    }

    index++;
    
    // end frame timing
    auto t2 = std::chrono::high_resolution_clock::now();
    
    float frame_time = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    
    // texture display
    glUseProgram(display_shader);
    glBindVertexArray( display_vao );
    glBindBuffer( GL_ARRAY_BUFFER, display_vbo );

    glDrawArrays( GL_TRIANGLES, 0, 6 );


    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();


    // info window
    ImGui::SetNextWindowPos(ImVec2(10,10));
    ImGui::SetNextWindowSize(ImVec2(260,117));
    ImGui::Begin("Info", NULL, 0);

    //do the other widgets
    // HelpMarker("shut up, compiler");

    //show timing info for the various operations in the setup, note unicode (UTF-8) escape sequence for micro 
    // ImGui::Text(" OBJ Loading:        %10.2f \xC2\xB5s", obj_load_time);
    // ImGui::Text(" Render:             %10.2f \xC2\xB5s", software_renderer_time);
    // ImGui::Text(" PNG Output:         %10.2f \xC2\xB5s", png_output_time);
    // ImGui::Text(" Texture Buffering:  %10.2f \xC2\xB5s", texture_buffer_time);

    ImGui::Text(" OBJ Loading:        %7.2f ms", obj_load_time/1000.0);
    ImGui::Text(" Render:             %7.2f ms", software_renderer_time/1000.0);
    ImGui::Text(" PNG Output:         %7.2f ms", png_output_time/1000.0);
    ImGui::Text(" Texture Buffering:  %7.2f ms", texture_buffer_time/1000.0);
    ImGui::Text(" Frame Time:         %7.2f ms", frame_time/1000.0);

    
    ImGui::End();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());   // put imgui data into the framebuffer

    SDL_GL_SwapWindow(window); // swap the double buffers

    // handle events

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT)
            pquit = true;

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            pquit = true;

        if ((event.type == SDL_KEYUP  && event.key.keysym.sym == SDLK_ESCAPE) || (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_X1)) //x1 is browser back on the mouse
            pquit = true;
    }
}

void tinyrenderer::clear_buffers()
{
    image_data.resize(0);
    depth_data.resize(0);
    
    image_data.resize(HEIGHT*WIDTH*4, 0); // 4 channels, might do something with alpha eventually
    depth_data.resize(HEIGHT*WIDTH, -std::numeric_limits<float>::max()); // initialize z buffer at most negative value
}

void tinyrenderer::buffer_GPU_texture()
{
    glBindTexture(GL_TEXTURE_2D, display_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, WIDTH, HEIGHT, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &image_data[0]);
    glBindImageTexture(1, display_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
}

void tinyrenderer::output_frame(std::string filename)
{
    lodepng::encode(filename.c_str(), image_data, WIDTH, HEIGHT); 
}


void tinyrenderer::draw_wireframe()
{
    // render triangles as 3 lines making up their outline 
    for(size_t i = 0; i < triangle_indices.size(); i++)
    {
        glm::vec3 p0, p1, p2;

        p0 = vertices[triangle_indices[i].x];  p0.x += 1.0f;  p0.x *= (WIDTH/2.0f);  p0.y += 1.0f;  p0.y *= (HEIGHT/2.0f); 
        p1 = vertices[triangle_indices[i].y];  p1.x += 1.0f;  p1.x *= (WIDTH/2.0f);  p1.y += 1.0f;  p1.y *= (HEIGHT/2.0f);
        p2 = vertices[triangle_indices[i].z];  p2.x += 1.0f;  p2.x *= (WIDTH/2.0f);  p2.y += 1.0f;  p2.y *= (HEIGHT/2.0f);
        
        draw_line(glm::ivec2(p0.xy()), glm::ivec2(p1.xy()), glm::vec4( 0.5, 0.2, 0.1, 1.0));
        draw_line(glm::ivec2(p1.xy()), glm::ivec2(p2.xy()), glm::vec4( 0.5, 0.2, 0.1, 1.0));
        draw_line(glm::ivec2(p0.xy()), glm::ivec2(p2.xy()), glm::vec4( 0.5, 0.2, 0.1, 1.0));
    }
}

glm::vec3 tinyrenderer::world2screen(glm::vec3 v)
{
    return glm::vec3(int((v.x+1.0f)*(WIDTH/2.0f)), int((v.y+1.0f)*(HEIGHT/2.0f)), v.z);
}

glm::vec4 tinyrenderer::diffuse_texture_ref(glm::vec3 sample)
{
    int sample_x = int(sample.x * diffuse_width);
    int sample_y = int((1.0-sample.y) * diffuse_height);

    // cout << "referencing image at " << sample_x << " " << sample_y << endl;
    
    int base = (sample_x + (diffuse_width * sample_y)) * 4;

    if((base+3) < texture_diffuse.size())
        return glm::vec4(texture_diffuse[base]/255.0, texture_diffuse[base+1]/255.0, texture_diffuse[base+2]/255.0, texture_diffuse[base+3]/255.0);
    else
        return glm::vec4(0,0,0,1);
}


glm::mat4 tinyrenderer::rotation(glm::vec3 a, float angle)
{//thanks to Neil Mendoza via http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
    a = glm::normalize(a); //a is the axis
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return glm::mat4(oc * a.x * a.x + c,         oc * a.x * a.y - a.z * s,  oc * a.z * a.x + a.y * s,  0.0,
                     oc * a.x * a.y + a.z * s,   oc * a.y * a.y + c,        oc * a.y * a.z - a.x * s,  0.0,
                     oc * a.z * a.x - a.y * s,   oc * a.y * a.z + a.x * s,  oc * a.z * a.z + c,        0.0,
                     0.0,                        0.0,                       0.0,                       1.0);
}

void tinyrenderer::draw_triangles()
{
    std::default_random_engine gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> cdist(0.0, 1.0); 


    
    // render triangles as triangles
    for(size_t i = 0; i < triangle_indices.size(); i++)
    {
        glm::vec3 p0, p1, p2;
        glm::vec3 n0, n1, n2;
        glm::vec3 t0, t1, t2;

        p0 = (transform*vertices[triangle_indices[i].x]).xyz();  
        p1 = (transform*vertices[triangle_indices[i].y]).xyz();
        p2 = (transform*vertices[triangle_indices[i].z]).xyz();

        n0 = (ntransform*glm::vec4(normals[normal_indices[i].x], 1.0f)).xyz();
        n1 = (ntransform*glm::vec4(normals[normal_indices[i].y], 1.0f)).xyz();
        n2 = (ntransform*glm::vec4(normals[normal_indices[i].z], 1.0f)).xyz();

        t0 = texcoords[texcoord_indices[i].x];
        t1 = texcoords[texcoord_indices[i].y];
        t2 = texcoords[texcoord_indices[i].z];

        glm::vec4 color = glm::vec4(0.5, 0.2*cdist(gen), cdist(gen), 1.0);
        // glm::vec4 color = glm::vec4(0.5, 0.3, 0.2, 1.0);
    
        glm::vec3 pts[3];
        pts[0] = world2screen(p0);
        pts[1] = world2screen(p1);
        pts[2] = world2screen(p2);

        glm::vec3 normals[3];
        normals[0] = n0;
        normals[1] = n1;
        normals[2] = n2;

        glm::vec3 texcoords[3];
        texcoords[0] = t0;
        texcoords[1] = t1;
        texcoords[2] = t2;
        
        draw_triangle(pts, normals, texcoords, color);
    } 
}

void tinyrenderer::draw_line(glm::ivec2 p0, glm::ivec2 p1, glm::vec4 color)
{ 
    int x0 = p0.x;
    int y0 = p0.y;
    int x1 = p1.x;
    int y1 = p1.y;

    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1))
    { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 

    if (x0>x1)
    { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 

    int dx = x1-x0; 
    int dy = y1-y0; 

    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = y0; 

    for (int x=x0; x<=x1; x++)
    { 
        if (steep)
        { 
            set_pixel(glm::ivec2(y, x), color); 
        }
        else
        { 
            set_pixel(glm::ivec2(x, y), color); 
        } 
        error2 += derror2; 
        if (error2 > dx)
        { 
            y += (y1>y0?1:-1); 
            error2 -= dx*2; 
        } 
    } 
}

glm::vec3 tinyrenderer::barycentric(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 P)
{
    glm::vec3 s[2];

    for(int i = 2; i--; )
    {
        s[i][0] = p2[i]-p0[i];
        s[i][1] = p1[i]-p0[i];
        s[i][2] = p0[i]-P[i];
    }

    glm::vec3 u = glm::cross(s[0], s[1]);
    
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return glm::vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return glm::vec3(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator 
}

void tinyrenderer::draw_triangle(glm::vec3 *pts, glm::vec3 *normals, glm::vec3 *texcoords, glm::vec4 color)
{
    glm::vec2 bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    glm::vec2 bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    glm::vec2 clamp(WIDTH-1, HEIGHT-1);
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<2; j++)
        {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    glm::vec3 P, N(0), T(0);
    float intensity;
    
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            glm::vec3 bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;

            for (int i=0; i<3; i++)
            {
                P.z += pts[i][2]*bc_screen[i];
                N += normals[i]*bc_screen[i];
            }

            N = glm::normalize(N);
            
            if (depth_data[int(P.x+P.y*WIDTH)] < P.z) {
                depth_data[int(P.x+P.y*WIDTH)] = P.z;

                float light_min = 0.25;
                float light_max = 2.0;
                
                // shading
                switch(current_mode)
                {
                    case FLAT:
                        // N = glm::normalize(glm::cross(pts[0]-pts[2], pts[0]-pts[1])); // might revisit this, not currently working correctly
                        N = (normals[0] + normals[1] + normals[2])/3.0f; // going to just use the averaged vertex normals for the time being 
                        intensity = std::min(std::max(glm::dot(N, light_position), light_min), light_max);  
                        set_pixel(glm::ivec2(P.x, P.y), glm::vec4(glm::vec3(intensity*color.rgb()), color.a));
                        break;
                        
                    case SMOOTH:
                        intensity = std::min(std::max(glm::dot(N, light_position), light_min), light_max);  
                        set_pixel(glm::ivec2(P.x, P.y), glm::vec4(glm::vec3(intensity*color.rgb()), color.a));
                        break;
                        
                    case TEXTURED:
                        intensity = std::min(std::max(glm::dot(N, light_position), light_min), light_max);  
                        T = texcoords[0]*bc_screen[0] + texcoords[1]*bc_screen[1] + texcoords[2]*bc_screen[2]; 
                        set_pixel(glm::ivec2(P.x, P.y), glm::vec4(intensity*diffuse_texture_ref(T).rgb(), 1.0));
                        break;
                        
                    default:

                        break;
                }
            }
        }
    } 
}

void tinyrenderer::set_pixel(glm::ivec2 p, glm::vec4 color)
{
    // base is where red is located, followed by g, b, a
    int base = (p.x + (WIDTH * (WIDTH-p.y))) * 4;
    if(base >= 0 && base <= WIDTH*HEIGHT*4)
    {
        // float gamma = 0.9f;
        // image_data[base]   = static_cast<unsigned char>(255.999 * std::pow(color.r, gamma)); 
        // image_data[base+1] = static_cast<unsigned char>(255.999 * std::pow(color.g, gamma)); 
        // image_data[base+2] = static_cast<unsigned char>(255.999 * std::pow(color.b, gamma)); 
        // image_data[base+3] = static_cast<unsigned char>(255.999 * color.a); 

        image_data[base]   = static_cast<unsigned char>(255.999 * color.r); 
        image_data[base+1] = static_cast<unsigned char>(255.999 * color.g); 
        image_data[base+2] = static_cast<unsigned char>(255.999 * color.b); 
        image_data[base+3] = static_cast<unsigned char>(255.999 * color.a); 
    }
}





void tinyrenderer::quit()
{
    //shutdown everything
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    //destroy window
    SDL_GL_DeleteContext(GLcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    cout << "goodbye." << endl;
}
