#ifndef _GG_UNIFORM_BUFFER_HANDLE_H_
#define _GG_UNIFORM_BUFFER_HANDLE_H_
#include "GGBufferHandle.h"
#include "GGCameraController.h"

namespace Gange {
	enum UniformType
	{
		scene,
		light
	};

class GGUniformBufferHandle : public GGBufferHandle {
public:
    GGUniformBufferHandle(UniformType uniformType = UniformType::scene, bool protagonist = false);

    virtual ~GGUniformBufferHandle();

    void generate() override;

    void updateUniformBuffers();

    bool getIsSkyBoxFlag();

    bool addPushConstant = false;

    struct {
        Matrix4 projection;
        Matrix4 view;
        Matrix4 model = Matrix4::IDENTITY;
    } mUboModel;

	struct {
		Vector3 lightPos;
		Vector3 viewPos;
		bool blinn = true;
	}mUboLight;

private:
	UniformType mUniformType = UniformType::scene;

    bool mProtagonist = false;

    GGCameraController *mCameraController = nullptr;
};

}  // namespace Gange

#endif  // !GG_TEXTURE_H_
