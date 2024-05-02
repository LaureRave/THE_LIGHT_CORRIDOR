#pragma once

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <cstdlib>
#include <cmath>

static const double CORRIDOR_WIDTH = 25.;
static const double CORRIDOR_HEIGHT = 15.;
static const int SECTIONS = 10.;

enum GAME_STATES
{
    ONGOING,
    LOSE,
    WIN
};

class Position
{
public:
    double x, y, z;

    Position() = default;

    Position(double _x, double _y, double _z)
        : x{_x}, y{_y}, z{_z}
    {
    }
};

class Color
{
public:
    float r, g, b;

    Color(float _r, float _g, float _b)
    {
        r = _r;
        g = _g;
        b = _b;
    }
};

class Player
{
public:
    double size; // racket is a square
    Position pos;
    bool bonusStick = false;  // stick
    bool bonusLife = false;  // ++ life

    Player() = default;

    Player(float _size)
        : size(_size), pos{0, 0, 0}
    {
    }
};

class Obstacle
{
public:
    float width;
    float height;
    Position pos;

    Obstacle() = default;

    Obstacle(float _width, float _height, Position _pos)
        : width{_width}, height{_height}, pos{_pos}
    {
    }
};

class Bonus
{
    float size;
    int type;
    Position pos;

    Bonus() = default;

    Bonus(float _size, int _type, Position _pos)
        : size{_size}, type{_type}, pos{_pos} {}
};

class Corridor
{
public:
    double width;
    double height;
    int sections;
    std::vector<Obstacle> obstacles;

    Corridor() = default;

    Corridor(double _width, double _height, int _sections)
        : width{_width}, height{_height}, sections{_sections}
    {
    }

    void generateCorridor()
    {
        for (int i = 0; i < sections / 2; i++)
        {
            double y = i * sections + sections; // profondeur
            int randomObstacleType = rand() % 6;

            switch (randomObstacleType)
            {
            case 0: // BIG OBSTACLE LEFT
            {
                Position position = Position(-width / 2, y * 2, height / 2);
                Obstacle obstacle = Obstacle(width / 2, height, position);
                obstacles.push_back(obstacle);
                break;
            }

            case 1: // BIG OBSTACLE RIGHT
            {
                Position position = Position(0, y * 2, height / 2);
                Obstacle obstacle = Obstacle(width / 2, height, position);
                obstacles.push_back(obstacle);
                break;
            }

            case 2: // SMALL OBSTACLE LEFT
            {
                Position position = Position(-width / 2 * 2, y, height / 2);
                Obstacle obstacle = Obstacle(width / 4, height, position);
                obstacles.push_back(obstacle);
                break;
            }

            case 3: // SMALL OBSTACLE RIGHT
            {
                Position position = Position(width / 4, y * 2, height / 2);
                Obstacle obstacle = Obstacle(width / 4, height, position);
                obstacles.push_back(obstacle);
                break;
            }

            default: // TWO LITTLE OBSTACLES LEFT-RIGHT
            {
                Position position = Position(width / 4, y * 2, height / 2);
                Obstacle obstacle = Obstacle(width / 4, height, position);
                obstacles.push_back(obstacle);
                Position position2 = Position(-width / 2, y * 2, height / 2);
                Obstacle obstacle2 = Obstacle(width / 4, height, position2);
                obstacles.push_back(obstacle2);
                break;
            }
            }
        }
    }
};

class Ball
{
public:
    float radius;
    Position pos;
    float defaultSpeed;
    Position speed;
    bool isThrown;

    Ball() {}

    Ball(float rad, float speed)
        : radius{rad}, pos{Position(0, rad + 1, 0)}, defaultSpeed{speed}, speed{Position(0, speed, 0)}, isThrown{false}
    {
    }

    // Check all possible collisions of the ball
    void checkCollisions(Corridor corridor, Player player, double currentPos)
    {
        obstacleCollision(corridor.obstacles);
        racketCollision(player, currentPos);
        wallCollision(corridor);
    }

private:
    void obstacleCollision(std::vector<Obstacle> obstacles)
    {
        for (Obstacle obstacle : obstacles)
        {
            // NEW BALL POSITION
            float nextX = pos.x + speed.x;
            float nextY = pos.y + speed.y;
            float nextZ = pos.z + speed.z;

            // REBOUND
            float ballMinX = nextX - radius;
            float ballMaxX = nextX + radius;
            float ballMinY = nextY - radius;
            float ballMaxY = nextY + radius;
            float ballMinZ = nextZ - radius;
            float ballMaxZ = nextZ + radius;

            // SIDE COLLISION
            if (pos.y + radius >= obstacle.pos.y && pos.y - radius <= obstacle.pos.y &&
                ballMaxX >= obstacle.pos.x && ballMinX <= obstacle.pos.x + obstacle.width)
            {
                speed.x = -speed.x;
            }

            // OBSTACLES COLLISION
            if (ballMaxX >= obstacle.pos.x && ballMinX <= obstacle.pos.x + obstacle.width &&
                ballMaxY >= obstacle.pos.y && ballMinY <= obstacle.pos.y &&
                ballMinZ >= obstacle.pos.z - obstacle.height && ballMaxZ <= obstacle.pos.z)
            {
                speed.y = -speed.y;
            }
        }
    }

    void racketCollision(Player player, double currentPos)
    {
        // BALL POSITION
        float nextX = pos.x + speed.x;
        float nextY = pos.y + speed.y;
        float nextZ = pos.z + speed.z;

        // BALL BORDER
        float ballMinX = nextX - radius;
        float ballMaxX = nextX + radius;
        float ballMinY = nextY - radius;
        float ballMaxY = nextY + radius;
        float ballMinZ = nextZ - radius;
        float ballMaxZ = nextZ + radius;

        // CHECK RACKET COLLISION
        if (ballMaxX >= player.pos.x - player.size / 2 && ballMinX <= player.pos.x + player.size / 2 &&
            ballMaxY >= player.pos.y + currentPos && ballMinY <= player.pos.y + currentPos &&
            ballMaxZ >= player.pos.z - player.size / 2 && ballMinZ <= player.pos.z + player.size / 2)
        {
            if (player.bonusStick) {
                // THE BALL STICK TO THE RACKET
                pos = Position(player.pos.x, radius + currentPos + 1, player.pos.z);
                speed = Position(0, defaultSpeed, 0);
                isThrown = false;
            }
            else {
                // REBOND
                const double MAX_SPEED = std::abs(speed.y);

                // DISTANCE CENTER RACKET - BALL
                float distanceFromCenterX = pos.x - player.pos.x;
                float distanceFromCenterZ = pos.z - player.pos.z;

                // REBOUND DIRECTION
                float reboundDirectionX = distanceFromCenterX / (player.size / 2);
                float reboundDirectionZ = distanceFromCenterZ / (player.size / 2);

                // UPDATE SPEED
                speed.x = reboundDirectionX * MAX_SPEED;
                speed.y = -speed.y;
                speed.z = reboundDirectionZ * MAX_SPEED;
            }
        }
    }

    void bonusCollision()
    { 
        // TODO
    }

    void wallCollision(Corridor corridor)
    {
        // BALL POSITION
        float nextX = pos.x + speed.x;
        float nextY = pos.y + speed.y;
        float nextZ = pos.z + speed.z;

        // BALL BORDER
        float ballMinX = nextX - radius;
        float ballMaxX = nextX + radius;
        float ballMinZ = nextZ - radius;
        float ballMaxZ = nextZ + radius;

        // BALL COLLISION WITH CORRIDOR
        if (ballMaxX >= corridor.width / 2)
        {
            pos.x = corridor.width / 2 - radius;
            speed.x = -speed.x;
        }
        else if (ballMinX <= -corridor.width / 2)
        {
            pos.x = -corridor.width / 2 + radius;
            speed.x = -speed.x;
        }
        if (ballMaxZ >= corridor.height / 2)
        {
            pos.z = corridor.height / 2 - radius;
            speed.z = -speed.z;
        }
        else if (ballMinZ <= -corridor.height / 2)
        {
            pos.z = -corridor.height / 2 + radius;
            speed.z = -speed.z;
        }
    }
};

class Game
{
public:
    Player player;
    Corridor corridor;
    Ball ball;
    int life;
    int score;
    GAME_STATES gameState;
    double currentPos = 0.;

    Game() = default;

    void loadGame()
    {
        corridor = Corridor(CORRIDOR_WIDTH, CORRIDOR_HEIGHT, SECTIONS);
        player = Player(CORRIDOR_WIDTH / 6);
        ball = Ball(CORRIDOR_WIDTH / 12, .2);
        life = 5;
        gameState = ONGOING;
        currentPos = 0;
        score = 0;
        corridor.generateCorridor();
    }

    // PLAYER MOVE FORWARD INSIDE THE CORRIDOR
    void moveForward(int distance)
    {
        if (racketCanMoveForward(distance, currentPos, corridor.obstacles)) // check if no obstacle in front of the player
        {
            if ((ball.pos.y - currentPos - distance - 2) > 0)
            {
                currentPos += distance;
                if (!ball.isThrown)
                {
                    ball.pos.y += distance;
                }
                score += distance * 25;
            }
        }
    }

    // PLAYER STATE: LOSE LIFE, WIN GAME, LOSE GAME
    void playerState()
    {
        double corridorEnd = corridor.sections * corridor.sections;
        if (ball.isThrown)
        {
            // BALL BEHIND RACKET
            if (ball.pos.y - ball.radius - currentPos < player.pos.y)
            {
                life--;
                std::cout << "CURRENT LIFE: " << life << std::endl;
                if (life == 0)
                {
                    gameState = LOSE;
                }
                else
                {
                    ball.pos = Position(player.pos.x, ball.radius + currentPos + 1, player.pos.z);
                    ball.speed = Position(0, ball.defaultSpeed, 0);
                    ball.isThrown = false;
                }
            }
            // CHECK IF END OF THE CORRIDOR
            else if (ball.pos.y + ball.radius > corridorEnd)
            {
                gameState = WIN;
            }
        }
    }

private:
    // Check if player can move forward (no obstacle in front of him)
    bool racketCanMoveForward(int distance, int currentPos, std::vector<Obstacle> obstacles)
    {
        for (Obstacle obstacle : obstacles)
        {
            // NEW PLAYER POSITION in y-axis
            float nextY = player.pos.y + distance;

            // RACKET BORDER
            float playerMinX = player.pos.x - player.size / 2;
            float playerMaxX = player.pos.x + player.size / 2;
            float playerMinZ = player.pos.z - player.size / 2;
            float playerMaxZ = player.pos.z + player.size / 2;

            // OBSTACLES COLLISION
            if (playerMaxX >= obstacle.pos.x && playerMinX <= obstacle.pos.x + obstacle.width &&
                nextY + currentPos >= obstacle.pos.y && player.pos.y + currentPos <= obstacle.pos.y &&
                playerMinZ >= obstacle.pos.z - obstacle.height && playerMaxZ <= obstacle.pos.z)
            {
                return false;
            }
        }
        return true;
    }
};
