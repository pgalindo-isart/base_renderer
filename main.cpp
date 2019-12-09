
#include <SDL2/SDL.h>

#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "types.hpp"
#include "maths.hpp"

// Rasterize a triangle to screen
void rasterizeTriangle(SDL_Renderer* renderer, vec2_t* screenCoords)
{
    SDL_RenderDrawLine(renderer, screenCoords[0].x, screenCoords[0].y, screenCoords[1].x, screenCoords[1].y);
    SDL_RenderDrawLine(renderer, screenCoords[0].x, screenCoords[0].y, screenCoords[2].x, screenCoords[2].y);
    SDL_RenderDrawLine(renderer, screenCoords[1].x, screenCoords[1].y, screenCoords[2].x, screenCoords[2].y);
}

void renderTriangles(SDL_Renderer* renderer, vec3_t* triangles, int vertexCount, 
    const mat4_t& viewportMatrix,
    const mat4_t& projection,
    const mat4_t& view,
    const mat4_t& model)
{
    // Precompute matrix transformations (local space -> clip space)
    mat4_t mvp = projection * view * model;

    // Iterate through all triangles
    for (int i = 0; i < vertexCount; i += 3)
    {
        // ====================================
        // Calculate clip coords of the triangle
        vec4_t clipCoords[3];
        for (int j = 0; j < 3; ++j)
        {
            clipCoords[j] = mvp * vec4::vec4(triangles[i + j], 1.f);
        }

        // ====================================
        // Perform (very) basic clipping
        bool clip = false;
        for (int j = 0; j < 3; ++j)
        {
            // Cull triangles
            if (   clipCoords[j].x < -clipCoords[j].w || clipCoords[j].x > clipCoords[j].w
                || clipCoords[j].y < -clipCoords[j].w || clipCoords[j].y > clipCoords[j].w
                || clipCoords[j].z < -clipCoords[j].w || clipCoords[j].z > clipCoords[j].w
            )
            {
                clip = true;
                break;
            }
        }
        if (clip)
            continue;

        // ====================================
        // Caculate NDC coords
        vec3_t ndcCoords[3];
        for (int j = 0; j < 3; ++j)
        {
            ndcCoords[j] = clipCoords[j].xyz / clipCoords[j].w;
        }

        // ====================================
        // Caculate screen coords
        vec2_t screenCoords[3];
        for (int j = 0; j < 3; ++j)
        {
            screenCoords[j] = (viewportMatrix * vec4::vec4(ndcCoords[j], 1.f)).xy;
        }

        // ====================================
        // Draw triangle
        rasterizeTriangle(renderer, screenCoords);
    }
}

std::vector<vec3_t> loadMesh(const char* filename, float scale)
{
    // Load mesh
    std::vector<vec3_t> objModel;
    {
        std::string warn;
        std::string err;
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
       
        tinyobj::LoadObj(&attrib, &shapes, NULL, &warn, &err, filename, NULL, true);
        if (!err.empty())
            printf("Error loading obj: %s\n", err.c_str());
 
        for (const tinyobj::shape_t& shape : shapes)
        {
            const tinyobj::mesh_t& mesh = shape.mesh;

            int indexId = 0;
            for (int faceId = 0; faceId < mesh.num_face_vertices.size(); ++faceId)
            {
                int faceVertices = mesh.num_face_vertices[faceId];
                assert(faceVertices == 3);

                for (int j = 0; j < faceVertices; ++j)
                {
                    const tinyobj::index_t& index = mesh.indices[indexId];
                    vec3_t v;
                    v = {
                        scale * attrib.vertices[index.vertex_index * 3 + 0],
                        scale * attrib.vertices[index.vertex_index * 3 + 1],
                        scale * attrib.vertices[index.vertex_index * 3 + 2]
                    };

                    objModel.push_back(v);
                    indexId++;
                }
            }
        }
    }

    return objModel;
}

int main()
{
    // Init SDL
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("SDL failure !\nError: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

    int width  = 640;
    int height = 480;
	SDL_Window* window = SDL_CreateWindow(
              "Renderer",
              SDL_WINDOWPOS_CENTERED,
              SDL_WINDOWPOS_CENTERED,
              width, height,
              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GL_SetSwapInterval(1);
    
    std::vector<vec3_t> mesh = loadMesh("media/teapot.obj", 0.5f);
    //std::vector<vec3_t> mesh = { { -0.5f, 0, 0 }, { 0, 0.5f, 0 }, { 0.5f, 0, 0 } };

    // Compute viewport tranformation (from NDC to screen space)
    mat4_t viewportMat = mat4::viewport(0, 0, width, height);

    float time = 0.f;
    bool isRunning = true;
    while (isRunning)
    {
        time += (1.f / 60.f);

        // ====================================
        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        isRunning = false;
                    break;
                default:;
            }
        }

        // ====================================
        // Setup matrices
        mat4_t model  = mat4::translate({0.f, 0.f, 0.f}) * mat4::rotateY(time * math::pi() * 0.24f);
        mat4_t camera = mat4::translate({0.f, 1.f, 5.f});
        mat4_t view   = mat4::inverse(camera);
        mat4_t proj   = mat4::perspective(60.f * math::pi() / 180.f, (float)width / height, 1.f, 100.f);

        // ====================================
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        // ====================================
        // Rasterize triangles
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        renderTriangles(renderer, &mesh[0], mesh.size(), viewportMat, proj, view, model);

        // Swap buffers
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	SDL_Quit();

    return 0;
}