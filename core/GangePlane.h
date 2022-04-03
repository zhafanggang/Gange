#ifndef _GG_PLANE_H_
#define _GG_PLANE_H_

#include "GGRenderable.h"

namespace Gange {
class Plane : public Renderable {
public:
    virtual void initialize();

    void draw(VkCommandBuffer commandBuffer);

protected:
private:
};
}  // namespace Gange

#endif