#include "3D_tools.hpp"
#include "draw_scene.hpp"

/* Camera parameters and functions */
float theta = 0.;       // Angle between x axis and viewpoint
float phy = 90.;        // Angle between z axis and viewpoint
float dist_zoom = 3.0f; // Distance between origin and viewpoint

void setCamera()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0., 0., -10.);
    glRotatef(-90, 1., 0., 0.);
}

void setPerspective(float fovy, float a_ratio, float z_near, float z_far)
{
    float mat[16] = {0.0f};

    double f = 1.0 / tan(fovy * M_PI / 360.0); // 1/tan(fovy/2)
    mat[0] = f / a_ratio;
    mat[5] = f;
    mat[10] = -(z_far + z_near) / (z_far - z_near);
    mat[11] = -1.0f;
    mat[14] = -2.0f * z_far * z_near / (z_far - z_near);
    mat[15] = 0.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(mat);
}

/* Convert degree to radians */
float toRad(float deg)
{
    return deg * M_PI / 180.0f;
}

void drawSquare()
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(-0.5, -0.5, 0.0);
    glVertex3f(0.5, -0.5, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glVertex3f(-0.5, 0.5, 0.0);
    glEnd();
}

void drawEmptySquare()
{
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glVertex2f(-0.5f, -0.5f);
    glEnd();
}

void drawCircle()
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0, 0.0);
    float step_rad = 2 * M_PI / (float)NB_SEG_CIRCLE;
    for (int i = 0; i <= NB_SEG_CIRCLE; i++)
    {
        glVertex3f(cos(i * step_rad), sin(i * step_rad), 0.0f);
    }
    glEnd();
}

void drawCone()
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0, 1.0);
    float step_rad = 2 * M_PI / (float)NB_SEG_CIRCLE;
    for (int i = 0; i <= NB_SEG_CIRCLE; i++)
    {
        glVertex3f(cos(i * step_rad), sin(i * step_rad), 0.0f);
    }
    glEnd();
}

void drawSphere()
{
    float angle_theta{0.0};
    float angle_alpha{0.0};
    float pas_angle_theta{M_PI / NB_SEG_CIRCLE};
    float pas_angle_alpha{2 * M_PI / NB_SEG_CIRCLE};
    for (int band{0}; band < NB_SEG_CIRCLE; band++)
    {
        angle_alpha = 0.0;
        glBegin(GL_TRIANGLE_STRIP);
        for (int count{0}; count <= NB_SEG_CIRCLE; count++)
        {
            glVertex3f(cosf(angle_alpha) * sinf(angle_theta),
                       sinf(angle_alpha) * sinf(angle_theta),
                       cosf(angle_theta));
            glVertex3f(cosf(angle_alpha) * sinf(angle_theta + pas_angle_theta),
                       sinf(angle_alpha) * sinf(angle_theta + pas_angle_theta),
                       cosf(angle_theta + pas_angle_theta));
            angle_alpha += pas_angle_alpha;
        }
        glEnd();
        angle_theta += pas_angle_theta;
    }
}
