#ifndef GEOMETRIC_H
#define GEOMETRIC_H

#include <random>
#include <memory>

// forward declaration
struct Vector2d;
struct Point;

struct Vector2d
{
    double x, y;

    Vector2d(const double &x = 0.0, const double &y = 0.0) : x(x), y(y) {}

    Vector2d operator-(const Vector2d &other) const
    {
        return Vector2d(x - other.x, y - other.y);
    }

    Vector2d operator+(const Vector2d &other) const
    {
        return Vector2d(x + other.x, y + other.y);
    }

    Vector2d operator*(double scalar) const
    {
        return Vector2d(x * scalar, y * scalar);
    }

    Vector2d operator/(double scalar) const
    {
        return Vector2d(x / scalar, y / scalar);
    }

    double dot(const Vector2d &other) const
    {
        return x * other.x + y * other.y;
    }

    double length() const
    {
        return std::sqrt(x * x + y * y);
    }

    Vector2d normalize() const
    {
        double len = length();
        return len > 0 ? Vector2d(x / len, y / len) : Vector2d(0, 0);
    }

    // declaration
    Point operator+(const Point &point) const;
};

struct Point
{
    double x, y;
    Point(const double &x = 0.0, const double &y = 0.0) : x(x), y(y) {}

    Vector2d operator-(const Point &other) const
    {
        return Vector2d(x - other.x, y - other.y);
    }

    Point operator+(const Vector2d &v) const
    {
        return Point(x + v.x, y + v.y);
    }

    Point operator-(const Vector2d &v) const
    {
        return Point(x - v.x, y - v.y);
    }
};

inline Point Vector2d::operator+(const Point &point) const
{
    return Point(x + point.x, y + point.y);
}

struct Segment
{
    Point origin;
    Point destination;

    Segment() : origin(), destination() {}
    Segment(const Point& origin, const Point destination) : origin(origin), destination(destination) {}
};

struct Ray
{
    Point origin;
    Vector2d direction;

    // for ploting
    Segment plot_segment;

    Ray() : origin(), direction(), plot_segment() {}

    Ray(const Point &origin, const Vector2d &direction) : origin(origin), direction(direction) {}

};

struct Circle
{
    Point center;
    double radius;
    Circle() : center(), radius(0.01) {}
    Circle(const Point &center, const double &radius = 0.01) : center(center), radius(radius) {}
};

struct BBox 
{
    Point bottom_left;
    Point top_right;

    BBox() : bottom_left(Point(-1.0, -1.0)), top_right(Point(1.0, 1.0)) {}
    BBox(const Point &bottom_left, const Point &top_right) : bottom_left(bottom_left), top_right(top_right) {}
};
   

// kd-tree
struct KDNode 
{
    Circle circle;
    BBox bbox;
    std::unique_ptr<KDNode> left, right;
    
    KDNode() : circle(), bbox(), left(nullptr), right(nullptr) {}
    KDNode(const Circle& circle, const BBox& bbox) 
        : circle(circle), bbox(bbox), left(nullptr), right(nullptr) {}
    
};

class KDTree 
{
private:
    std::unique_ptr<KDNode> m_root;

public:
    KDTree() : m_root(nullptr) {}

    void clear() { m_root.reset(); }

    // construct kd tree
    std::unique_ptr<KDNode> recursive_build(std::vector<Circle>& circles, BBox bbox, std::size_t& depth) 
    {
        if (circles.empty()) {
            return nullptr;
        }

        int cd = depth % 2;

        if (cd == 0) 
        {
            std::sort(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
                if (a.center.x == b.center.x) return a.center.y < b.center.y;
                return a.center.x < b.center.x;}
                );
        } 
        else 
        {
            std::sort(circles.begin(), circles.end(), [](const Circle& a, const Circle& b) {
                if (a.center.y == b.center.y) return a.center.x < b.center.x;    
                return a.center.y < b.center.y;}
                );
        }

        size_t median_index = circles.size() / 2;
        Point bottom_left = bbox.bottom_left;
        Point top_right = bbox.top_right;

        if (cd == 0) 
        {
            top_right.x = circles[median_index].center.x;
        } else {
            top_right.y = circles[median_index].center.y;
        }

        // using radius to enlarge the bbox for safety
        const double& radius = circles[median_index].radius;
        Vector2d offset = Point(radius, radius) - Point(.0, .0);
        BBox left_bbox = BBox(bbox.bottom_left - offset, top_right + offset);
        BBox right_bbox = BBox(bottom_left - offset, bbox.top_right + offset);
        

        std::unique_ptr<KDNode> node = std::make_unique<KDNode>(circles[median_index], BBox());

        std::vector<Circle> left_circles(circles.begin(), circles.begin() + median_index);
        std::vector<Circle> right_circles(circles.begin() + median_index + 1, circles.end());

        depth++;
        node->left = recursive_build(left_circles, left_bbox, depth);
        node->right = recursive_build(right_circles, right_bbox, depth);

        return node;
    }

    std::size_t build(const std::vector<Circle>& circles, BBox bbox) 
    {
        std::vector<Circle> circles_copy = circles;
        std::size_t depth = 0;
        m_root = recursive_build(circles_copy, bbox, depth);

        return depth;
    }

    bool does_ray_intersect_bbox(const Ray &ray, const BBox &bbox) const 
    {
        double txmin = (bbox.bottom_left.x - ray.origin.x) / ray.direction.x;
        double txmax = (bbox.top_right.x - ray.origin.x) / ray.direction.x;

        if (txmin > txmax) std::swap(txmin, txmax);

        double tymin = (bbox.bottom_left.y - ray.origin.y) / ray.direction.y;
        double tymax = (bbox.top_right.y - ray.origin.y) / ray.direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((txmin > tymax) || (tymin > txmax))
            return false;

        return true;
    }

    bool does_ray_intersect_circle(const Ray &ray, const Circle &circle) const 
    {
        Vector2d origin_to_center = circle.center - ray.origin;
        assert(std::abs(ray.direction.length() - 1.0) < 1e-7);
        double proj = origin_to_center.dot(ray.direction); 
        double d_square = origin_to_center.dot(origin_to_center) - proj * proj; 
        double radius_square = circle.radius * circle.radius; 

        if (d_square > radius_square) return false;

        double t = std::sqrt(radius_square - d_square); 
        double t0 = proj - t;
        double t1 = proj + t; 

        if (t0 > t1) std::swap(t0, t1);

        if (t0 > 0) return true;

        return t1 > 0;
    }

    void detect_intersection_recursive(const std::unique_ptr<KDNode> &node, 
        const Ray &ray, std::vector<Circle> &results) const 
    {
        if (!node) return;

        // Check if ray intersects the bounding box of the current node
        if (!does_ray_intersect_bbox(ray, node->bbox)) return;

        // Check if ray intersects the circle of the current node
        if (does_ray_intersect_circle(ray, node->circle)) {
            results.push_back(node->circle);
        }

        // Recursively check the left and right children
        detect_intersection_recursive(node->left, ray, results);
        detect_intersection_recursive(node->right, ray, results);
    }

    void detect_intersection(const Ray &ray, std::vector<Circle> &results) const 
    {
        detect_intersection_recursive(m_root, ray, results);
    }

};

class Algorithm
{
private:
    std::random_device m_rd;
    std::mt19937 m_gen;

public:
    std::mt19937 random_generator() {return m_gen;}

public:
    std::unique_ptr<KDTree> m_kdtree_ptr 
        = std::make_unique<KDTree>();
    Algorithm() : m_gen(m_rd()) {}

    void clear() { m_kdtree_ptr = std::make_unique<KDTree>(); }

public:
    void generate_random_circles(std::vector<Circle> &circles, 
        const BBox& rect, double radius, std::size_t num_circles)
    {
        std::uniform_real_distribution<> distri_x(rect.bottom_left.x + radius, rect.top_right.x - radius);
        std::uniform_real_distribution<> distri_y(rect.bottom_left.y + radius, rect.top_right.y - radius);

        for (int i = 0; i < num_circles; ++i)
        {
            float x = distri_x(m_gen);
            float y = distri_y(m_gen);
            circles.emplace_back(Point(x, y), radius);
        }
    }

    void generate_random_ray(Ray &ray, const BBox& viewer_rect)
    {
        std::uniform_real_distribution<> distri_x(viewer_rect.bottom_left.x,  viewer_rect.top_right.x);
        std::uniform_real_distribution<> distri_y(viewer_rect.bottom_left.y,  viewer_rect.top_right.y);

        std::vector<Point> edge_points = {
            {distri_x(m_gen), viewer_rect.bottom_left.y}, // Bottom edge
            {distri_x(m_gen), viewer_rect.top_right.y}, // Top edge
            {viewer_rect.bottom_left.x, distri_y(m_gen)}, // Left edge
            {viewer_rect.top_right.x, distri_y(m_gen)}  // Right edge
        };

        std::shuffle(edge_points.begin(), edge_points.end(), m_gen);

        Point origin = edge_points[0];
        Point destination = edge_points[1];
        
        Vector2d direction = destination - origin;
        direction = direction.normalize();
        
        ray = Ray(origin, direction);
        ray.plot_segment = Segment(origin, destination);
    }

    Ray generate_random_ray_screen(double viewer_w, double viewer_h)
    {
        std::uniform_real_distribution<> distri_x(0,  viewer_w);
        std::uniform_real_distribution<> distri_y(0,  viewer_h);

        std::vector<Point> edge_points = {
            {distri_x(m_gen), viewer_h}, // Bottom edge
            {distri_x(m_gen), 0}, // Top edge
            {0, distri_y(m_gen)}, // Left edge
            {viewer_w, distri_y(m_gen)}  // Right edge
        };

        std::shuffle(edge_points.begin(), edge_points.end(), m_gen);

        Point origin = edge_points[0];
        Point destination = edge_points[1];
        
        Vector2d direction = destination - origin;
        direction = direction.normalize();
        
        Ray ray = Ray(origin, direction);
        ray.plot_segment = Segment(origin, destination);

        return ray;
    }


    std::size_t build_kdtree(const std::vector<Circle>& circles, BBox bbox)
    {
        return m_kdtree_ptr->build(circles, bbox);
    }

    void detect_intersection(const Ray& ray,
         const std::vector<Circle>& circles, std::vector<Circle> &results)
    {
        m_kdtree_ptr->detect_intersection(ray, results);
    }

};

#endif