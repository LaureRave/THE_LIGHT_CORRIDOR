#define GLFW_INCLUDE_NONE
#define _USE_MATH_DEFINES

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <iostream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "3D_tools.hpp"
#include "draw_scene.hpp"

/* Window properties */
static const unsigned int WINDOW_WIDTH = 1500;
static const unsigned int WINDOW_HEIGHT = 800;
static const char WINDOW_TITLE[] = "THE LIGHT CORRIDOR";
static float aspectRatio = 1.0;
static const int scalingFactor = 4;

Game game = Game();

static const float _viewSize = CORRIDOR_HEIGHT;

/* Minimal time wanted between two images */
static const double FRAMERATE_IN_SECONDS = 1. / 60.;

/* Error handling function */
void onError(int error, const char *description)
{
	std::cout << "GLFW Error: " << description << std::endl;
}

void onWindowResized(GLFWwindow *window, int width, int height)
{
	aspectRatio = width / (float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setPerspective(60.0f, aspectRatio, Z_NEAR, Z_FAR);
	glMatrixMode(GL_MODELVIEW);
	setCamera();
}

/* MOUSE BUTTON CALLBACK : right click = throw ball / left click = move racket forward */
void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (!game.ball.isThrown)
		{
			game.ball.isThrown = true;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (game.ball.isThrown)
		{
			game.moveForward(1);
			std::cout << "CURRENT SCORE: " << game.score << std::endl;
		}
	}
}

// update Position pos in relation with mouse position (posX, posY)
void updateMousePosition(Position *pos, int posX, int posY, int width, int height, double _viewSize, double aspectRatio, double xLimit, double zLimit)
{
	pos->x = std::max(-xLimit / 2, std::min(xLimit / 2, ((_viewSize * aspectRatio) / width * posX - (_viewSize * aspectRatio) / 2.0)));
	pos->z = std::max(-zLimit / 2, std::min(zLimit / 2, (-_viewSize / height * posY + _viewSize / 2.0)));
}

/* CURSOR CALLBAK: Move racket followed by cursor position */
void cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
	// move racket
	updateMousePosition(&game.player.pos, xpos, ypos, WINDOW_WIDTH, WINDOW_HEIGHT, _viewSize, aspectRatio, CORRIDOR_WIDTH - game.player.size, CORRIDOR_HEIGHT - game.player.size);

	// Move ball followed by racket position if not thrown
	if (!game.ball.isThrown)
	{
		updateMousePosition(&game.ball.pos, xpos, ypos, WINDOW_WIDTH, WINDOW_HEIGHT, _viewSize, aspectRatio, CORRIDOR_WIDTH - game.player.size, CORRIDOR_HEIGHT - game.player.size);
	}
}

void onKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_A:
			std::cout << "QUIT" << std::endl;
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;

		case GLFW_KEY_S: // start game
			std::cout << "START" << std::endl;
			game.loadGame();
			break;

		default:
			std::cout << "Touche non gérée (" << key << ")" << std::endl;
			break;
		}
	}
}

void draw()
{
	switch (game.gameState)
	{
	case ONGOING:
		glPushMatrix();
		glTranslatef(0, -game.currentPos, 0);
		drawBall(game.ball);
		drawCorridor(game.corridor);
		glPopMatrix();
		drawPlayer(game.player);
		break;

	// Game Over menu
	case LOSE:
		std::cout << "YOU LOSE !" << std::endl;
		glfwTerminate();
		exit(0);

	// Victory menu
	case WIN:
		std::cout << "YOU WIN !" << std::endl;
		glfwTerminate();
		exit(0);
	}
}

int main(int /* argc */, char ** /* argv */)
{
	/* GLFW initialisation */
	GLFWwindow *window;
	if (!glfwInit())
		return -1;

	/* Callback to a function if an error is rised by GLFW */
	glfwSetErrorCallback(onError);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	if (!window)
	{
		// If no context created : exit !
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Intialize glad (loads the OpenGL functions)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}

	srand(time(NULL));

	glfwSetWindowSizeCallback(window, onWindowResized);
	glfwSetKeyCallback(window, onKey);
	onWindowResized(window, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwSetMouseButtonCallback(window, mouse_callback); // mouse click
	glfwSetCursorPosCallback(window, cursor_callback);	// cursor position

	glPointSize(5.0);
	glEnable(GL_DEPTH_TEST);

	// transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	game.loadGame(); // load the game

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Get time (in second) at loop beginning */
		double startTime = glfwGetTime();

		/* Cleaning buffers and setting Matrix Mode */
		glClearColor(0.2, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Scene rendering */
		if (game.gameState == ONGOING)
		{
			if (game.ball.isThrown)
			{
				game.ball.pos.x += game.ball.speed.x;
				game.ball.pos.y += game.ball.speed.y;
				game.ball.pos.z += game.ball.speed.z;
			}
			game.ball.checkCollisions(game.corridor, game.player, game.currentPos);
			game.playerState();
		}
		draw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		/* Elapsed time computation from loop begining */
		double elapsedTime = glfwGetTime() - startTime;
		/* If to few time is spend vs our wanted FPS, we wait */
		if (elapsedTime < FRAMERATE_IN_SECONDS)
		{
			glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS - elapsedTime);
		}
	}

	glfwTerminate();
	return 0;
}