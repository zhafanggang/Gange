#ifndef _GG_UNIFORM_BUFFER_HANDLE_H_
#define _GG_UNIFORM_BUFFER_HANDLE_H_
#include "GGBufferHandle.h"
#include "GGCameraController.h"

namespace Gange {

class GGUniformBufferHandle : public GGBufferHandle {
public:
    GGUniformBufferHandle();

    virtual ~GGUniformBufferHandle();

    void generate() override;

    void updateUniformBuffers();

    void setCameraController(GGCameraController *cameraController);

    struct {
        Matrix4 projection;
        Matrix4 modelView;
        // Vector4 lightPos = Vector4(5.0f, 5.0f, 5.0f, 1.0f);
    } uboModel;

private:
    GGCameraController *mCameraController = nullptr;
};

}  // namespace Gange

#endif  // !GG_TEXTURE_H_
