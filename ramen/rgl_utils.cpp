#include "rgl_utils.h"

#include "rgl_model.h"

/* Defines a right handed, z-up coordinates system.
 * This is the default convention when using OpenGL and Vulkan.
 * XYZ axis map to the colors RGB.
 */
static const Vertex rgl_axis_rh_zu[ 6 ]
    = { Vertex{ .position = { 0.0f }, .normal = { 0.0f }, .color = { 1.0f, 0.0f, 0.0f } },
        Vertex{ .position = { 1.0f, 0.0f, 0.0f }, .normal = { 0.0f }, .color = { 1.0f, 0.0f, 0.0f } },
        Vertex{ .position = { 0.0f }, .normal = { 0.0f }, .color = { 0.0f, 1.0f, 0.0f } },
        Vertex{ .position = { 0.0f, 1.0f, 0.0f }, .normal = { 0.0f }, .color = { 0.0f, 1.0f, 0.0f } },
        Vertex{ .position = { 0.0f }, .normal = { 0.0f }, .color = { 0.0f, 0.0f, 1.0f } },
        Vertex{ .position = { 0.0f, 0.0f, 1.0f }, .normal = { 0.0f }, .color = { 0.0f, 0.0f, 1.0f } } };

const Vertex* Utils::CoordSystemRHZU()
{
    return rgl_axis_rh_zu;
}
