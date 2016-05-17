#include <format.h>
#include <cmath>
#include <iostream>

// GLAD
#include <glad/glad.h>

// SDL
#include <SDL/SDL.h>

// TGA Image
#include <tgaimage.h>

// PNG image
#include <lodepng.h>

#include <tiled.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_impl_sdl.h>

#include <stdio.h>
#include <chrono>
#include <fstream>
#include <stopwatch.hpp>
#include <string>
#include <vector>

//#include "tiled.hpp"
#include <gl_utils.hpp>

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

SDL_Window* setupSDL() {
	std::cout << "Starting SDL context, OpenGL 4.1" << std::endl;
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// Set all the required options for SDL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create a GLFWwindow object that we can use for GLFW's functions
	SDL_Window* window = SDL_CreateWindow("Kuratko Nufik", 200, 200, WIDTH,
		HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return nullptr;
	}

	return window;
}

void tile_at(std::vector<float>& vbo_data, float x, float y, float size) {
	float off = size / 2;

	// Position      Color             Texture coords
	std::vector<float> vertices{
		x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		x + off, y - off, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		x + off, y + off, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		x - off, y - off, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		x - off, y + off, 1.0f, 1.0f, 1.0,  0.0f, 0.0f,
	};

	for (float v : vertices) {
		vbo_data.push_back(v);
	}
}

int current_x = 0;
int current_y = 0;

void draw_vector_triangles(const std::vector<float>& vbo_data) {
	// sending vertices to the graphic cards and making it active
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbo_data.size(),
		vbo_data.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, vbo_data.size());
}

void game_loop(SDL_Window* window) {
	using namespace gl;
	using namespace glm;

	int storyProgress = 0;

	// Setup ImGui binding
	ImGui_ImplSdlGL3_Init(window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, WIDTH, HEIGHT);

	VAO vao;
	VBO vbo;


	SDL_Event event;

	Shader spriteShader("res/sprite");
	SpriteRenderer sprite(spriteShader);

	spriteShader.set("projection", ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f));

	auto map = load_tiles("xmlova.tmx");


	std::unordered_map<int, Texture2D> textures;

	for (size_t i = 0; i < map.tiles.size(); i++)
	{
		Texture2D t;
		t.image_format = GL_RGBA;
		t.internal_format = GL_RGBA;
		t.load_png("res/" + map.tiles[i].filename);

		textures[map.tiles[i].gid] = std::move(t);
	}

	Texture2D t1;
	t1.image_format = GL_RGBA;
	t1.internal_format = GL_RGBA;
	t1.load_png("res/kuratko_basic_klaciky.png");


	while (true) {
		ImGui_ImplSdlGL3_NewFrame(window);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT ||
				(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_ESCAPE)) {
				return;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case 'w': current_y--; break;
				case 's': current_y++; break;
				case 'a': current_x--; break;
				case 'd': current_x++; break;
				}
			}

			ImGui_ImplSdlGL3_ProcessEvent(&event);
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int tile_size = 32;



		for (size_t i = 0; i < map.N(); i++)
		{
			for (size_t j = 0; j < map.N(); j++)
			{
				auto id = map.gid(i, j) - 1;
				if (textures.count(id) > 0) {
					auto&& tex = textures[id];
					sprite.draw_sprite(tex, vec2(j * tile_size, i * tile_size));
				}
			}

		}

		sprite.draw_sprite(t1, vec2(current_x * tile_size, current_y * tile_size));

		if (storyProgress == 0) {
			ImGui::Begin("Kuratko Nufik - Kapitola 1");
			ImGui::TextWrapped("Kuratko Nufik zilo ve zviratkovem lese spolu se svou maminkou na vysokem strome. Jednoho dne, kdyz maminka odesla sbirat sisticky k papiku se kuratko naklanelo z okraje hnizda, aby videlo svet kolem a ... co se nestalo! Kuratko spadlo!");
			ImGui::TextWrapped("");
			ImGui::TextWrapped("Kuratko zuchlo na zadecek, nastesti do vysoke travy a nic se mu nestalo. Kuratko bylo z toho cele vyjukane a zacalo se rozhlizet po okoli. Udelalo par kroku sem, par tam a najednou dostalo strach.");
			ImGui::TextWrapped("");
			ImGui::TextWrapped(" 'Radsi abych se vratil hned domu, nebo se o me bude maminka bat!'");
			ImGui::TextWrapped("");
			ImGui::TextWrapped("Kuratko chtelo najit svuj strom a vysplhat nahoru do hnizda, ale ouhle. Vsechny stromy vypadaly ze zdola uplne stejne! Jak pozna ten pravy?");
			ImGui::TextWrapped("");

			if (ImGui::Button("Zacit hledat cestu domu ...")) {
				storyProgress++;
			}

			ImGui::End();
		}
		else if (storyProgress == 1) {
		}

		

		ImGui::Render();

		SDL_GL_SwapWindow(window);
	}
}

// The MAIN function, from here we start the application and run the game loop
int main(int, char**) {


	SDL_Window* window = setupSDL();
	if (!window) return -1;

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return 1;
	}

	game_loop(window);

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int clamp(int low, int current, int high) {
	if (current < low) return low;
	if (current > high) return high;
	return current;
}
