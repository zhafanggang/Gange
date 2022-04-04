#ifndef _GANGE_BOX_H_
#define _GANGE_BOX_H_

#include "Math/GGVector3.h"
#include <vector>
#include "GGRenderable.h"
#include "GGVulkanSingleHandle.h"

namespace Gange {
class ModelBox : public Renderable {
public:
    ModelBox(const Vector3 &halfSize);

    ModelBox(bool protagonist = false);

    ~ModelBox();

    void initialize() override;

    void initializeSkyBox();
};
}  // namespace Gange

#endif
