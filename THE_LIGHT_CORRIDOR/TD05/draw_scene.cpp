#include "draw_scene.hpp"
#include "3D_tools.hpp"
#include <vector>

// Corridor colors

Color corridorColor(int c)
{
	switch (c)
	{
	case 0: // blue
		return Color(25. / 255., 25. / 255., 130. / 255.);
	case 1: // red
		return Color(130. / 255., 25. / 255., 25. / 255.);
	default: // green
		return Color(25. / 255., 130. / 255., 25. / 255.);
	}
}

Color color_left_right = corridorColor(0);
Color color_up_down = Color(color_left_right.r * 2, color_left_right.g * 2, color_left_right.b * 2);
Color color_obstacle = color_left_right;

void drawFrame()
{
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
}

// Draw Ball (= sphere)
void drawBall(Ball ball)
{
	glPushMatrix();
	glColor3f(60. / 255., 60. / 255., 60. / 255.); // dark grey
	glTranslatef(ball.pos.x, ball.pos.y, ball.pos.z);
	glScalef(ball.radius, ball.radius, ball.radius);
	drawSphere();
	glPopMatrix();
}

// Draw the Racket (= square)
void drawPlayer(Player player)
{
	glPushMatrix();
	glTranslatef(0, player.pos.y, 0);
	glTranslatef(player.pos.x, 0, player.pos.z);
	glScalef(player.size, 1, player.size);
	glRotatef(90, 1, 0, 0);

	// DRAW BORDER OF RACKET
	glColor3f(1., 1., 1.);
	glLineWidth(2.0);
	drawEmptySquare();

	// DRAW INSIDE OF RACKET (TRANSPARENCY)
	glColor4f(1., 1., 1., .25);
	drawSquare();

	glPopMatrix();
}

// Draw all obstacles in corridor
void drawObstacles(std::vector<Obstacle> obstacles, Color color)
{
	for (Obstacle obstacle : obstacles)
	{
		glPushMatrix();
		glTranslatef(0, obstacle.pos.y, 0);
		glTranslatef(obstacle.pos.x, 0, obstacle.pos.z);
		glTranslatef((obstacle.width) / 2, 0, -(obstacle.height) / 2);
		glScalef(obstacle.width, 1, obstacle.height);
		glRotatef(90, 1, 0, 0);
		glColor4f(color.r, color.g, color.b, 0.5);
		drawSquare();
		glPopMatrix();
	}
}

// Draw the corridor with all sections
void drawSections(Corridor corridor)
{
	for (int i = 0; i < corridor.sections; i++)
	{
		double y = i * corridor.sections + corridor.sections;
		float posX = 0;
		float posY1 = y - corridor.sections / 2;
		float posY2 = y;
		float posZ1 = corridor.height / 2;
		float posZ2 = -corridor.height / 2;

		// Draw UP wall
		glPushMatrix();
		glTranslatef(posX, posY1, posZ1);
		glScalef(corridor.width, corridor.sections, corridor.height);
		glColor3f(color_up_down.r, color_up_down.g, color_up_down.b);
		drawSquare();
		glPopMatrix();

		// Draw DOWN wall
		glPushMatrix();
		glTranslatef(posX, posY1, posZ2);
		glScalef(corridor.width, corridor.sections, corridor.height);
		glColor3f(color_up_down.r, color_up_down.g, color_up_down.b);
		drawSquare();
		glPopMatrix();

		// Draw LEFT wall
		posX = -corridor.width / 2;
		float posZ3 = 0;
		glPushMatrix();
		glTranslatef(posX, posY1, posZ3);
		glRotatef(90, 0, 1, 0);
		glScalef(corridor.height, corridor.sections, corridor.width);
		glColor3f(color_left_right.r, color_left_right.g, color_left_right.b);
		drawSquare();
		glPopMatrix();

		// Draw RIGHT wall
		posX = corridor.width / 2;
		glPushMatrix();
		glTranslatef(posX, posY1, posZ3);
		glRotatef(90, 0, 1, 0);
		glScalef(corridor.height, corridor.sections, corridor.width);
		glColor3f(color_left_right.r, color_left_right.g, color_left_right.b);
		drawSquare();
		glPopMatrix();

		// Draw SECTIONS
		glPushMatrix();
		glTranslatef(0, posY2, 0);
		glScalef(corridor.width, 1, corridor.height);
		glRotatef(90, 1, 0, 0);
		glColor4f(255., 255., 255., 1.);
		drawEmptySquare();
		glPopMatrix();
	}
}

// draw the corridor : walls, sections, obstacles
void drawCorridor(Corridor corridor)
{
	glPushMatrix();
	glTranslatef(0, corridor.sections + corridor.sections * (SECTIONS - 1) - 1. / 100, 0);
	glScalef(corridor.width, 1, corridor.height);
	glRotatef(90., 1., 0., 0.);
	glPopMatrix();
	drawObstacles(corridor.obstacles, color_obstacle);
	drawSections(corridor);
}
