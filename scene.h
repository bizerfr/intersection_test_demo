#ifndef _SCENE_H_
#define _SCENE_H_

// std
#include <fstream>
#include <algorithm>
#include <vector>
#include <iostream> // delete=====
// Qt
#include <QtOpenGL>


#include "geometric.h"


class Scene
{
public:
    double m_circle_radius;
    BBox m_rect; // bouding the circles
    BBox m_viewer_rect; // bouding the whole scene
    BBox m_viewer_rect_screen; // viewer in the screen coordinate system

    Point m_mouse_pos;

private:
    std::vector<Circle> m_circles;
    Ray m_ray;
    std::vector<Circle> m_intersected_circles;
  
private:
    std::unique_ptr<Algorithm> m_alg_ptr 
        = std::make_unique<Algorithm>();

public:
    Scene() {}

    ~Scene()
    {
        clear_all();
    }

    void set_mouse_pos(const Point &pos) { m_mouse_pos = pos; }

    void set_circle_radius(const double radius) { m_circle_radius = radius; }
    void set_rect(const BBox& rect) { m_rect = rect; }
    void set_viewer_rect(const BBox& rect) { m_viewer_rect = rect; }
    void set_screen_viewer_rect(const BBox& rect) { m_viewer_rect_screen = rect; }
    Ray& get_random_ray() { return m_ray; }
    const std::vector<Circle>& get_intersected_circles() { return m_intersected_circles; }

    void clear_all()
    {
       m_circles.clear();
       m_intersected_circles.clear();
       m_ray = Ray();
       m_alg_ptr->clear();
    }

    void clear_circles()
    {
       m_circles.clear();
       m_intersected_circles.clear();
       m_alg_ptr->clear();
    }

    void clear_ray()
    {
       m_ray = Ray();
       m_intersected_circles.clear();
    }

    // plot
    void render()
    {
        plot_rect(); 

        // plot circles
        for(const auto &circle : m_circles)
        {
            plot_circle_boundary(circle);
        }  
    
        plot_ray();

        // plot intersected circles
        for(const auto &circle : m_intersected_circles)
        {
            plot_shaded_circle(circle);
        } 
    }

    void plot_rect()
    {
        glColor3f(0.0f, 0.0f, 0.0f); // Set color to black
        glLineWidth(2.0f);           // Set line width
        glBegin(GL_LINE_LOOP);
        glVertex2f(m_rect.bottom_left.x, m_rect.bottom_left.y);
        glVertex2f(m_rect.top_right.x, m_rect.bottom_left.y);
        glVertex2f(m_rect.top_right.x, m_rect.top_right.y);
        glVertex2f(m_rect.bottom_left.x, m_rect.top_right.y);
        glEnd();
    }

    void plot_circle_boundary(const Circle& circle)
    {
        int num_segments = 100;
        const float delta = 2.0f * M_PI / num_segments;
        
        glColor3f(0.1f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP); 
        for (int i = 0; i < num_segments; ++i)
        {
            float angle = i * delta;
            float x = circle.center.x + circle.radius * std::cos(angle);
            float y = circle.center.y + circle.radius * std::sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }

    void plot_shaded_circle(const Circle& circle)
    {
        int num_segments = 100;
        const float delta = 2.0f * M_PI / num_segments;
        
        glColor3f(0.1f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(circle.center.x, circle.center.y);
        for (int i = 0; i < num_segments; ++i)
        {
            float angle = i * delta;
            float x = circle.center.x + circle.radius * std::cos(angle);
            float y = circle.center.y + circle.radius * std::sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }

    void plot_ray()
    {
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(m_ray.plot_segment.origin.x, m_ray.plot_segment.origin.y); 
        glVertex2f(m_ray.plot_segment.destination.x, m_ray.plot_segment.destination.y); 
        glEnd();
    }

    void generate_random_circles()
    {
        m_circles.clear();

        std::uniform_int_distribution<> distri_num_circles(100, 2000);
        std::size_t num_circles = distri_num_circles(m_alg_ptr->random_generator());

        m_alg_ptr->generate_random_circles(m_circles, m_rect, m_circle_radius, num_circles);
        std::cerr << "generate circles:" << m_circles.size() << std::endl;
    }

    void generate_random_ray()
    {
        m_alg_ptr->generate_random_ray(m_ray, m_viewer_rect);
        std::cerr << "generate a random ray," 
            << "origin:" << "(" << m_ray.origin.x << "," << m_ray.origin.y << ")" << "," 
            << "direction:" << "(" << m_ray.direction.x << "," << m_ray.direction.y << ")" << ","
            << "plot_segment: " << "(" << m_ray.plot_segment.origin.x << "," << m_ray.plot_segment.origin.y << ")" 
            << "->" << "(" << m_ray.plot_segment.destination.x << "," << m_ray.plot_segment.destination.y << ")" << std::endl;
    }

    void build_kdtree()
    {
        std::size_t depth = m_alg_ptr->build_kdtree(m_circles, m_rect);
        std::cerr << "construct kdtree, depth: " << depth << std::endl;
    }

    void detect_intersection()
    {
        m_alg_ptr->detect_intersection(m_ray, m_circles, m_intersected_circles);
        std::cerr << "intersected circles: " << m_intersected_circles.size() << std::endl;
    }

}; // end of class scence

#endif // _SCENE_H_
