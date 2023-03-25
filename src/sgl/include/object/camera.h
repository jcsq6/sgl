#pragma once
#include "math/mat.h"

SGL_BEG

class camera
{
public:
    inline camera(vec3 _pos, vec3 target) : pos{_pos}
    {
        look_at(target);
    }

    inline mat4 view() const
    {
        return orient(m_right, m_up, m_dir) * translate(-pos);
    }

    inline void look_at(vec3 loc)
    {
        if (loc != pos)
            m_dir = pos - loc;
        else
            m_dir = {1, 0, 0};
        get_axes();
    }

    inline void rotate_right(float amount)
    {
        vec4 d{m_dir, 1};
        d = rot(-amount, m_up) * d;
        m_dir = d;
        get_axes();
    }

    inline void rotate_up(float amount)
    {
        vec4 d{m_dir, 1};
        d = rot(amount, m_right) * d;
        vec3 dir_vec{d};
        float theta = angle(dir_vec, vec3{dir_vec.x, 0, dir_vec.z});
        // pi / 2.15 is slightly less than 90 degrees, to prevent spiraling horizontal rotation
        if (std::abs(theta) >= pi<float>() / 2.15f)
            return;
        m_dir = dir_vec;
        get_axes();
    }

    inline void move_forward(float distance)
    {
        pos -= distance * normalize(vec3{m_dir.x, 0, m_dir.z});
    }

    inline void move_right(float distance)
    {
        pos += distance * normalize(vec3{m_right.x, 0, m_right.z});
    }

    inline void move_up(float distance)
    {
        pos.y += distance;
    }

    inline vec3 get_dir() const
    {
        return m_dir;
    }
    
    vec3 pos;
private:
    vec3 m_dir;
    vec3 m_right;
    vec3 m_up;

    inline void get_axes()
    {
        if (m_dir.x || m_dir.z)
            m_right = normalize(cross(sgl::vec3{0, 1, 0}, m_dir));
        else if (m_dir.y > 0)
            m_right = {0, 0, -1};
        else
            m_right = {0, 0, 1};
        
        m_up = normalize(cross(m_dir, m_right));
    }
};

SGL_END