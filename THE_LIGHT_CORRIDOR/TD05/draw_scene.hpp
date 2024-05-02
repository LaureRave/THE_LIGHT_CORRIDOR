#pragma once

#include "elements.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>

extern Game game;

void drawFrame();

void drawBall(Ball ball);

void drawPlayer(Player player);

void drawCorridor(Corridor corridor);


