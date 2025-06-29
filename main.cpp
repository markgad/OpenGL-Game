#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//Mark Gad 212001260
//Yehia Zakria 211010031
//eng Merihan Ezzeldine


struct Vertex {
    float x, y, z;
};

std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

const int stacks = 15;
const int slices = 15;
const float radius = 11.0f;

float globeRotation = 0.0f;
float waveTime = 0.0f;

bool inMenu = true;
bool gameOver = false;
bool showIntroMessage = false;
bool gameStarted = false;
bool wonGame = false;

int score = 0;
int timeLeft = 60; // countdown from 60 seconds
bool timerRunning = false;

// ---------------- Obstacles ----------------
struct Obstacle {
    float x, y;
    bool active;
};

std::vector<Obstacle> redObstacles;
std::vector<Obstacle> cyanObstacles;

const float OBSTACLE_MIN_Y = 0.2f;
const float OBSTACLE_MAX_Y = 10.0f;

// Plane position controlled by keyboard
float planeX = 0.0f, planeY = 0.0f;
float planeTargetX = 0.0f, planeTargetY = 0.0f;

// Exhaust particle struct and container
struct ExhaustParticle {
    float x, y, z;
    float size;
    float flicker; // brightness factor (0..1)
};
std::vector<ExhaustParticle> exhaustParticles;

void spawnRedObstacle();
void spawnCyanObstacle();
void spawnRedObstaclesTimer(int);
void spawnCyanObstaclesTimer(int);
void updateCountdown(int);
void startGame(int);

void startGame(int) {
    showIntroMessage = false;
    gameStarted = true;
    timerRunning = true;
    wonGame = false;
    glutTimerFunc(1000, updateCountdown, 0);
    glutTimerFunc(1000, spawnRedObstaclesTimer, 0);
    glutTimerFunc(3000, spawnCyanObstaclesTimer, 0);
}

void initObstacles() {
    redObstacles.clear();
    cyanObstacles.clear();
    planeX = 0.0f;
    planeY = 0.0f;
    planeTargetX = 0.0f;
    planeTargetY = 0.0f;
    gameOver = false;
    inMenu = false;
    score = 0;
    timeLeft = 60;
    timerRunning = false;
    wonGame = false;
    exhaustParticles.clear();
}

bool isValidY(float new_y, float min_spacing, const std::vector<Obstacle>& obsVec) {
    for (const auto& obs : obsVec) {
        if (obs.active && fabs(obs.y - new_y) < min_spacing) {
            return false;
        }
    }
    return true;
}

void spawnRedObstacle() {
    float new_y;
    int tries = 0;
    float min_spacing = 0.1f + static_cast<float>(rand()) / RAND_MAX * 1.0f;
    do {
        new_y = OBSTACLE_MIN_Y + static_cast<float>(rand()) / RAND_MAX * (OBSTACLE_MAX_Y - OBSTACLE_MIN_Y);
        tries++;
    } while (!isValidY(new_y, min_spacing, redObstacles) && tries < 15);
    if (tries >= 15) return;
    redObstacles.push_back({-7.5f, new_y, true});
}

void spawnCyanObstacle() {
    float new_y;
    int tries = 0;
    float min_spacing = 1.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
    do {
        new_y = OBSTACLE_MIN_Y + static_cast<float>(rand()) / RAND_MAX * (OBSTACLE_MAX_Y - OBSTACLE_MIN_Y);
        tries++;
    } while (!isValidY(new_y, min_spacing, cyanObstacles) && tries < 15);
    if (tries >= 15) return;
    cyanObstacles.push_back({-7.5f, new_y, true});
}

void spawnRedObstaclesTimer(int) {
    if (!gameOver) {
        spawnRedObstacle();
        glutTimerFunc(1000, spawnRedObstaclesTimer, 0);
    }
}

void spawnCyanObstaclesTimer(int) {
    if (!gameOver) {
        spawnCyanObstacle();
        glutTimerFunc(3000, spawnCyanObstaclesTimer, 0);
    }
}

void updateCountdown(int) {
    if (timerRunning && !gameOver && timeLeft > 0) {
        timeLeft--;
        if (timeLeft <= 0) {
            gameOver = true;
            timerRunning = false;
            if (score < 20)
                wonGame = false;
        } else {
            glutTimerFunc(1000, updateCountdown, 0);
        }
    }
}

void updateObstacles() {
    for (auto& obs : redObstacles) {
        if (obs.active) {
            obs.x += 0.01f;
            if (obs.x > 8.0f) obs.active = false;
        }
    }
    for (auto& obs : cyanObstacles) {
        if (obs.active) {
            obs.x += 0.004f;
            if (obs.x > 8.0f) obs.active = false;
        }
    }
}

void generateSphere() {
    vertices.clear();
    indices.clear();

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * M_PI * j / slices;
            float x = sin(phi) * cos(theta);
            float y = cos(phi);
            float z = sin(phi) * sin(theta);
            vertices.push_back({x, y, z});
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;
            indices.insert(indices.end(), {first, second, first + 1, second, second + 1, first + 1});
        }
    }
}

// Text drawing utility
void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

void drawMenu() {
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, 0.2f);
    glVertex2f(0.3f, 0.2f);
    glVertex2f(0.3f, 0.0f);
    glVertex2f(-0.1f, 0.0f);
    glEnd();

    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.1f, -0.1f);
    glVertex2f(0.3f, -0.1f);
    glVertex2f(0.3f, -0.3f);
    glVertex2f(-0.1f, -0.3f);
    glEnd();

    glColor3f(1, 1, 1);
    drawText("Start", -0.7f, 0.07f);
    drawText("Exit", -0.7f, -0.23f);
}

void drawWavySphere() {
    glPushMatrix();
    glTranslatef(0.0f, -13.0f, -6.0f);
    glRotatef(globeRotation, 1.0f, 1.0f, 0.0f);
    glColor3f(0.2f, 0.6f, 0.7f);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); ++i) {
        Vertex v = vertices[indices[i]];
        float wave = 0.3f * sin(10.0f * v.x + waveTime) * sin(10.0f * v.y + waveTime);
        glVertex3f((radius + wave) * v.x, (radius + wave) * v.y, (radius + wave) * v.z);
    }
    glEnd();
    glPopMatrix();
}

// --- Exhaust update & draw ---
void updateExhaust() {
    if (exhaustParticles.size() > 20) {
        exhaustParticles.erase(exhaustParticles.begin(), exhaustParticles.begin() + 5);
    }

    float px = planeX + 0.8f; // behind plane since facing left
    float py = planeY + ((rand() % 100) / 500.0f - 0.1f);
    float pz = -1.5f + ((rand() % 100) / 1000.0f - 0.05f);
    float psize = 0.05f + (rand() % 10) / 100.0f;
    float flicker = 0.7f + (rand() % 30) / 100.0f;
    exhaustParticles.push_back({px, py, pz, psize, flicker});
}

void drawExhaust() {
    for (const auto& p : exhaustParticles) {
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glColor3f(p.flicker, p.flicker * 0.4f, 0.0f);
        glutSolidSphere(p.size, 8, 8);
        glPopMatrix();
    }
}

// --- Draw the 3D plane ---
void drawPlane() {
    glPushMatrix();
    glTranslatef(planeX, planeY, -1.5f);

    // Rotate plane 180 degrees around Y to face left
    glRotatef(180.0f, 0, 1, 0);

    glColor3f(1.0f, 1.0f, 0.0f);

    // Body (elongated cube)
    glPushMatrix();
    glScalef(0.7f, 0.2f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Nose cone
    glPushMatrix();
    glTranslatef(0.45f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    glutSolidCone(0.15, 0.3, 20, 20);
    glPopMatrix();

    // Vertical tail fin
    glPushMatrix();
    glTranslatef(-0.4f, 0.15f, 0.0f);
    glScalef(0.15f, 0.35f, 0.05f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left wing
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.35f);
    glScalef(0.4f, 0.05f, 0.7f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right wing
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.35f);
    glScalef(0.4f, 0.05f, 0.7f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Left horizontal stabilizer
    glPushMatrix();
    glTranslatef(-0.6f, 0.0f, 0.2f);
    glScalef(0.3f, 0.05f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right horizontal stabilizer
    glPushMatrix();
    glTranslatef(-0.6f, 0.0f, -0.2f);
    glScalef(0.3f, 0.05f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}

void drawPlayer() {
    drawExhaust();
    drawPlane();
}

void drawObstacles() {
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const auto& obs : redObstacles) {
        if (obs.active) {
            glPushMatrix();
            glTranslatef(obs.x, obs.y, -1.5f);
            glutSolidSphere(0.09, 10, 10);
            glPopMatrix();
        }
    }
    glColor3f(0.0f, 1.0f, 1.0f);
    for (const auto& obs : cyanObstacles) {
        if (obs.active) {
            glPushMatrix();
            glTranslatef(obs.x, obs.y, -1.5f);
            glutSolidSphere(0.18, 10, 10);
            glPopMatrix();
        }
    }
}

float dist2D(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

bool checkCollisionWithSphere() {
    return sqrt(planeX * planeX + (planeY + 13.0f)*(planeY + 13.0f)) < radius + 0.2f;
}

bool checkCollisionWithObstacles() {
    for (const auto& obs : redObstacles) {
        if (obs.active && dist2D(planeX, planeY, obs.x, obs.y) < 0.29f) return true;
    }
    return false;
}

void checkCollisionWithCyan() {
    for (auto& obs : cyanObstacles) {
        if (obs.active && dist2D(planeX, planeY, obs.x, obs.y) < 0.38f) {
            obs.active = false;
            score++;
            timeLeft += 2; // Add 2 seconds per point

            if (score >= 20) {  // Win condition
                wonGame = true;
                gameOver = true;
                timerRunning = false;
            }
        }
    }
}

void drawGameOver() {
    // Save state
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);          // Disable depth test so overlay draws on top
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Save matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);           // 2D orthographic projection

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw semi-transparent black background quad
    glColor4f(0, 0, 0, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-1, -1); glVertex2f(1, -1);
    glVertex2f(1, 1); glVertex2f(-1, 1);
    glEnd();

    // Draw text
    glColor3f(1, 1, 1);
    if (wonGame) {
        drawText("YOU ARE THE CHAMP!", -0.3f, 0.3f);
    } else {
        drawText("GAME OVER!", -0.2f, 0.3f);
    }
    char scoreStr[50];
    sprintf(scoreStr, "Score: %d", score);
    drawText(scoreStr, -0.2f, 0.15f);

    // Draw Play Again button
    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.2f, -0.1f);
    glVertex2f(0.2f, -0.1f);
    glVertex2f(0.2f, -0.3f);
    glVertex2f(-0.2f, -0.3f);
    glEnd();

    glColor3f(0, 0, 0);
    drawText("Play Again", -0.15f, -0.25f);

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // Restore attributes
    glPopAttrib();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (inMenu) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Draw menu buttons (green Start, red Exit)
        glColor3f(0.2f, 0.8f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, 0.2f);
        glVertex2f(0.3f, 0.2f);
        glVertex2f(0.3f, 0.0f);
        glVertex2f(-0.1f, 0.0f);
        glEnd();

        glColor3f(0.8f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.1f);
        glVertex2f(0.3f, -0.1f);
        glVertex2f(0.3f, -0.3f);
        glVertex2f(-0.1f, -0.3f);
        glEnd();

        glColor3f(1, 1, 1);
        drawText("Start", -0.7f, 0.07f);
        drawText("Exit", -0.7f, -0.23f);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    } else {
        gluLookAt(0, 0, 8, 0, 0, 0, 0, 1, 0);
        drawWavySphere();
        drawObstacles();
        drawPlayer();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        if (showIntroMessage) {
            glColor3f(1, 1, 0);
            drawText("The World is crashing...", -0.45f, 0.1f);
            drawText("Try to survive to save the world", -0.55f, -0.05f);
        } else {
            // Timer color fades white->red as time decreases
            char timerStr[20];
            int minutes = timeLeft / 60;
            int seconds = timeLeft % 60;
            sprintf(timerStr, "Time: %d:%02d", minutes, seconds);

            float factorTime = (float)timeLeft / 60.0f;
            float greenBlueTime = factorTime;
            glColor3f(1.0f, greenBlueTime, greenBlueTime);
            drawText(timerStr, -0.1f, 0.95f);

            // Score color fades white->gold as points increase
            float factorScore = std::min(score, 20) / 20.0f;
            float greenScore = 1.0f - factorScore * 0.16f; // 1 to 0.84
            float blueScore = 1.0f - factorScore;           // 1 to 0
            glColor3f(1.0f, greenScore, blueScore);

            char scoreStr[50];
            sprintf(scoreStr, "Score: %d", score);
            drawText(scoreStr, -0.1f, 0.90f);
        }

        if (gameOver) drawGameOver();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }

    glutSwapBuffers();
}

void update(int) {
    if (!inMenu && !gameOver && gameStarted) {
        waveTime += 0.05f;
        globeRotation += 0.1f;
        if (globeRotation > 360.0f)
            globeRotation -= 360.0f;

        updateObstacles();

        // Smooth movement of plane toward target position
        planeX += (planeTargetX - planeX) * 0.2f;
        planeY += (planeTargetY - planeY) * 0.2f;

        if (checkCollisionWithSphere() || checkCollisionWithObstacles()) {
            gameOver = true;
            timerRunning = false;
            wonGame = false;
        }

        checkCollisionWithCyan();

        updateExhaust();
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Keyboard input for arrow keys to move plane target position
void specialKeyDown(int key, int x, int y) {
    const float step = 0.5f;
    const float maxX = 7.0f;
    const float maxY = 10.0f;
    const float minX = -7.0f;
    const float minY = -7.0f;

    switch (key) {
    case GLUT_KEY_UP:
        planeTargetY += step;
        if (planeTargetY > maxY)
            planeTargetY = maxY;
        break;
    case GLUT_KEY_DOWN:
        planeTargetY -= step;
        if (planeTargetY < minY)
            planeTargetY = minY;
        break;
    case GLUT_KEY_LEFT:
        planeTargetX -= step;
        if (planeTargetX < minX)
            planeTargetX = minX;
        break;
    case GLUT_KEY_RIGHT:
        planeTargetX += step;
        if (planeTargetX > maxX)
            planeTargetX = maxX;
        break;
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float normX = (x / 800.0f) * 2.0f - 1.0f;
        float normY = 1.0f - (y / 600.0f) * 2.0f;

        if (inMenu) {
            // Check Start button
            if (normX > -0.1f && normX < 0.3f && normY > 0 && normY < 0.2f) {
                inMenu = false;
                showIntroMessage = true;
                gameStarted = false;
                initObstacles();
                glutTimerFunc(5000, startGame, 0);
            }
            // Check Exit button
            else if (normX > -0.1f && normX < 0.3f && normY > -0.3f && normY < -0.1f) {
                exit(0);
            }
        } else if (gameOver) {
            // Play Again button
            if (normX > -0.2f && normX < 0.2f && normY > -0.3f && normY < -0.1f) {
                gameOver = false;
                showIntroMessage = false;
                initObstacles();
                gameStarted = true;
                timerRunning = true;
                glutTimerFunc(1000, updateCountdown, 0);
                glutTimerFunc(1000, spawnRedObstaclesTimer, 0);
                glutTimerFunc(3000, spawnCyanObstaclesTimer, 0);
            }
        }
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand((unsigned int)time(nullptr));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("The Aviator");

    glEnable(GL_DEPTH_TEST);
    generateSphere();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutSpecialFunc(specialKeyDown);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
