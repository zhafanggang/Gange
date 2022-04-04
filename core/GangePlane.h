#ifndef _GG_PLANE_H_
#define _GG_PLANE_H_

#include "GGRenderable.h"

namespace Gange {
class Plane : public Renderable {
public:
    Plane() {
        initialize();
    };
    ~Plane(){};
    void initialize() override;
};
}  // namespace Gange

#endif