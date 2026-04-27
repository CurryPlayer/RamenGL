#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#include "../ramen/rgl_math.h"
#include <vector>

class MatrixStack {

private:
    std::vector<Mat4f> stack;

public:
    MatrixStack() {
        stack.push_back(Mat4f::Identity());
    }

    /**
     * Speichert den aktuellen Zustand
     */
    void push() {
        stack.push_back(stack.back());
    }

    /**
     * Kehrt zum letzten Zustand zurück
     */
    void pop() {
        if (stack.size() > 1) {
            stack.pop_back();
        }
    }

    /**
     * Gibt die aktuell oberste Matrix zurück
     */
    Mat4f& top() {
        return stack.back();
    }

    void multiply(const Mat4f& mat) {
        stack.back() = stack.back() * mat;
    }

    void translate(const Vec3f& vec)
    {
        stack.back() = stack.back() * Translate(vec);
    }

    void rotate(const Vec3f& axis, const float angle)
    {
        stack.back() = stack.back() * Rotate(axis, angle);
    }

    void scale(const Vec3f& vec)
    {
        stack.back() = stack.back() * Scale(vec);
    }

};

#endif
