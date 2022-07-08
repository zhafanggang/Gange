#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#if defined(__ANDROID__)
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif
#include <tinygltf/stb_image.h>
#include <tinygltf/tiny_gltf.h>
#include "GGVulkanGlTFModel.h"
#include "GGVulkanSingleHandle.h"
#include "math/GGQuaternion.h"
#include "GGVulkanTools.h"
#include <fstream>
#include <iomanip>

namespace Gange {

GGVulkanGlTFModel::GGVulkanGlTFModel(bool protagonist) {
    mProtagonist = protagonist;
    initialize();
}

GGVulkanGlTFModel::~GGVulkanGlTFModel() {}

void GGVulkanGlTFModel::initialize() {
	if (mProtagonist)
	{
		loadglTFFile(getAssetPath() + "models/Alice/Alice.gltf");
	}
	else
	{
		loadglTFFile(getAssetPath() + "models/samplebuilding.gltf");
	}
}

void GGVulkanGlTFModel::loadImages(tinygltf::Model &input) {
    size_t textureCount = input.images.size();
    for (size_t i = 0; i < textureCount; i++) {
        tinygltf::Image &glTFImage = input.images[i];
        // Get the image data from the glTF loader
        unsigned char *buffer = nullptr;
        VkDeviceSize bufferSize = 0;
        bool deleteBuffer = false;
        if (glTFImage.component == 3) {
            bufferSize = glTFImage.width * glTFImage.height * 4;
            buffer = new unsigned char[bufferSize];
            unsigned char *rgba = buffer;
            unsigned char *rgb = &glTFImage.image[0];
            for (size_t i = 0; i < glTFImage.width * glTFImage.height; ++i) {
                memcpy(rgba, rgb, sizeof(unsigned char) * 3);
                rgba += 4;
                rgb += 3;
            }
            deleteBuffer = true;
        } else {
            buffer = &glTFImage.image[0];
            bufferSize = glTFImage.image.size();
        }
        // Load texture from image buffer
        GGTextureImage *texture = new GGTextureImage();
        texture->loadFromPixels(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, glTFImage.width, glTFImage.height);
        mImages.push_back(texture);
        if (deleteBuffer) {
            delete[] buffer;
        }
    }
}

void GGVulkanGlTFModel::loadTextures(tinygltf::Model &input) {
    mTextureIndices.resize(input.textures.size());
    for (size_t i = 0; i < input.textures.size(); i++) {
        mTextureIndices[i] = (uint32_t) input.textures[i].source;
    }
}

void GGVulkanGlTFModel::loadMaterials(tinygltf::Model &input) {
    mMaterials.resize(input.materials.size());
    for (size_t i = 0; i < input.materials.size(); i++) {
        // We only read the most basic properties required for our sample
        tinygltf::Material glTFMaterial = input.materials[i];
        // Get the base color factor
        if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
            mMaterials[i].baseColorFactor =
                Vector4(reinterpret_cast<Real*>(glTFMaterial.values["baseColorFactor"].ColorFactor().data()));
        }
        // Get base color texture index
        if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
            mMaterials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
        }
    }
}

Node *GGVulkanGlTFModel::findNode(Node *parent, uint32_t index)
{
	Node *nodeFound = nullptr;
	if (parent->index == index)
	{
		return parent;
	}
	for (auto &child : parent->children)
	{
		nodeFound = findNode(child, index);
		if (nodeFound)
		{
			break;
		}
	}
	return nodeFound;
}

Node *GGVulkanGlTFModel::nodeFromIndex(uint32_t index)
{
	Node *nodeFound = nullptr;
	for (auto &node : mNodes)
	{
		nodeFound = findNode(node, index);
		if (nodeFound)
		{
			break;
		}
	}
	return nodeFound;
}

// POI: Update the joint matrices from the current animation frame and pass them to the GPU
void GGVulkanGlTFModel::updateJoints(Node *node)
{
	//if (node->skin > -1)
	//{
	//	// Update the joint matrices
	//	Matrix4              inverseTransform = getNodeMatrix(node);
	//	Skin                   skin = skins[node->skin];
	//	size_t                 numJoints = (uint32_t)skin.joints.size();
	//	std::vector<glm::mat4> jointMatrices(numJoints);
	//	for (size_t i = 0; i < numJoints; i++)
	//	{
	//		jointMatrices[i] = getNodeMatrix(skin.joints[i]) * skin.inverseBindMatrices[i];
	//		jointMatrices[i] = inverseTransform * jointMatrices[i];
	//	}
	//	// Update ssbo
	//	skin.ssbo.copyTo(jointMatrices.data(), jointMatrices.size() * sizeof(glm::mat4));
	//}

	for (auto &child : node->children)
	{
		updateJoints(child);
	}
}


// POI: Load the mSkins from the glTF model
void GGVulkanGlTFModel::loadSkins(tinygltf::Model &input)
{
	mSkins.resize(input.skins.size());

	for (size_t i = 0; i < input.skins.size(); i++)
	{
		tinygltf::Skin glTFSkin = input.skins[i];

		mSkins[i].name = glTFSkin.name;
		// Find the root node of the skeleton
		mSkins[i].skeletonRoot = nodeFromIndex(glTFSkin.skeleton);

		// Find joint mNodes
		for (int jointIndex : glTFSkin.joints)
		{
			Node *node = nodeFromIndex(jointIndex);
			if (node)
			{
				mSkins[i].joints.push_back(node);
			}
		}

		// Get the inverse bind matrices from the buffer associated to this skin
		if (glTFSkin.inverseBindMatrices > -1)
		{
			const tinygltf::Accessor &  accessor = input.accessors[glTFSkin.inverseBindMatrices];
			const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &    buffer = input.buffers[bufferView.buffer];
			mSkins[i].inverseBindMatrices.resize(accessor.count);
			memcpy(mSkins[i].inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(Matrix4));

			// Store inverse bind matrices for this skin in a shader storage buffer object
			// To keep this sample simple, we create a host visible shader storage buffer
			VK_CHECK_RESULT(mVulkanDevice->createBuffer(
				sizeof(Matrix4) * mSkins[i].inverseBindMatrices.size(),
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				&mSkins[i].ssbo,
				mSkins[i].inverseBindMatrices.data()));
			VK_CHECK_RESULT(mSkins[i].ssbo.map());
		}
	}
}

// POI: Load the mAnimations from the glTF model
void GGVulkanGlTFModel::loadAnimations(tinygltf::Model &input)
{
}
//{
//	mAnimations.resize(input.animations.size());
//
//	for (size_t i = 0; i < input.animations.size(); i++)
//	{
//		tinygltf::Animation glTFAnimation = input.animations[i];
//		mAnimations[i].name = glTFAnimation.name;
//
//		// Samplers
//		mAnimations[i].samplers.resize(glTFAnimation.samplers.size());
//		for (size_t j = 0; j < glTFAnimation.samplers.size(); j++)
//		{
//			tinygltf::AnimationSampler glTFSampler = glTFAnimation.samplers[j];
//			AnimationSampler &         dstSampler = mAnimations[i].samplers[j];
//			dstSampler.interpolation = glTFSampler.interpolation;
//
//			// Read sampler keyframe input time values
//			{
//				const tinygltf::Accessor &  accessor = input.accessors[glTFSampler.input];
//				const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
//				const tinygltf::Buffer &    buffer = input.buffers[bufferView.buffer];
//				const void *                dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
//				const float *               buf = static_cast<const float *>(dataPtr);
//				for (size_t index = 0; index < accessor.count; index++)
//				{
//					dstSampler.inputs.push_back(buf[index]);
//				}
//				// Adjust animation's start and end times
//				for (auto input : mAnimations[i].samplers[j].inputs)
//				{
//					if (input < mAnimations[i].start)
//					{
//						mAnimations[i].start = input;
//					};
//					if (input > mAnimations[i].end)
//					{
//						mAnimations[i].end = input;
//					}
//				}
//			}
//
//			// Read sampler keyframe output translate/rotate/scale values
//			{
//				const tinygltf::Accessor &  accessor = input.accessors[glTFSampler.output];
//				const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
//				const tinygltf::Buffer &    buffer = input.buffers[bufferView.buffer];
//				const void *                dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
//				switch (accessor.type)
//				{
//				case TINYGLTF_TYPE_VEC3: {
//					const Vector3 *buf = static_cast<const Vector3 *>(dataPtr);
//					for (size_t index = 0; index < accessor.count; index++)
//					{
//						dstSampler.outputsVec4.push_back(Vector4(buf[index], 0.0f));
//					}
//					break;
//				}
//				case TINYGLTF_TYPE_VEC4: {
//					const Vector4 *buf = static_cast<const Vector4 *>(dataPtr);
//					for (size_t index = 0; index < accessor.count; index++)
//					{
//						dstSampler.outputsVec4.push_back(buf[index]);
//					}
//					break;
//				}
//				default: {
//					std::cout << "unknown type" << std::endl;
//					break;
//				}
//				}
//			}
//		}
//
//		// Channels
//		mAnimations[i].channels.resize(glTFAnimation.channels.size());
//		for (size_t j = 0; j < glTFAnimation.channels.size(); j++)
//		{
//			tinygltf::AnimationChannel glTFChannel = glTFAnimation.channels[j];
//			AnimationChannel &         dstChannel = mAnimations[i].channels[j];
//			dstChannel.path = glTFChannel.target_path;
//			dstChannel.samplerIndex = glTFChannel.sampler;
//			dstChannel.node = nodeFromIndex(glTFChannel.target_node);
//		}
//	}
//}

Matrix4 GGVulkanGlTFModel::getNodeMatrix(Node *node)
{
	return Matrix4::Matrix4::IDENTITY;
}

void GGVulkanGlTFModel::loadglTFFile(std::string filename) {
    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;

#if defined(__ANDROID__)
    tinygltf::asset_manager = VulkanSingleHandle::getAssetManager();
#endif
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);

    std::vector<uint32_t> indexBuffer;
    std::vector<Vertex> vertexBuffer;

    if (fileLoaded) {
        loadImages(glTFInput);
        loadMaterials(glTFInput);
        loadTextures(glTFInput);
        const tinygltf::Scene &scene = glTFInput.scenes[0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = glTFInput.nodes[scene.nodes[i]];
            loadNode(node, glTFInput, nullptr, indexBuffer, vertexBuffer);
        }
    } else {
        return;
    }

	int i = 0;
	for (Node* node : mLinearNodes) {
		if (node->mesh) {
			std::cout << i++ << std::endl;
			for (Primitive* primitive : node->mesh->primitives) {
				for (uint32_t i = 0; i < primitive->vertexCount ; i++) {
					Vertex& vertex = vertexBuffer[primitive->firstVertex + i];
					// Pre-Multiply vertex colors with material base color
					if (1) {
						if (primitive->materialIndex>6)
						{
							vertex.color = Vector4(1.0f);
						}
						else
						{
							vertex.color = mMaterials[primitive->materialIndex].baseColorFactor * vertex.color;
						}
					}
				}
			}
		}
	}
	Renderable::loadVao(vertexBuffer, indexBuffer);
}

void GGVulkanGlTFModel::loadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input, Node *parent,
                                 std::vector<uint32_t> &indexBuffer, std::vector<Vertex> &vertexBuffer) {
    Node *node = new Node();
	Mesh *newMesh = new Mesh();
	node->mesh = newMesh;
    node->matrix = Matrix4::IDENTITY;

    Quaternion rotate = Quaternion::ZERO;
    Vector3 translation = Vector3::ZERO;
    Vector3 scale = Vector3::UNIT_SCALE;

    if (inputNode.translation.size() == 3) {
        translation.x = static_cast<Real>(inputNode.translation[0]);
        translation.y = static_cast<Real>(inputNode.translation[1]);
        translation.z = static_cast<Real>(inputNode.translation[2]);
        // node.matrix.setTrans(translation);
    }
    if (inputNode.rotation.size() == 4) {
        rotate.x = static_cast<Real>(inputNode.rotation[0]);
        rotate.y = static_cast<Real>(inputNode.rotation[1]);
        rotate.z = static_cast<Real>(inputNode.rotation[2]);
        rotate.w = static_cast<Real>(inputNode.rotation[3]);
        // node.matrix = Matrix4(rotate);
    }
    if (inputNode.scale.size() == 3) {
        scale.x = static_cast<Real>(inputNode.scale[0]);
        scale.y = static_cast<Real>(inputNode.scale[1]);
        scale.z = static_cast<Real>(inputNode.scale[2]);
    }
    if (inputNode.matrix.size() == 16) {
		node->matrix = Matrix4((Real *) inputNode.matrix.data());
    };

	node->matrix.makeTransform(translation * 100, scale * 100, rotate);
	node->matrix = node->matrix.transpose();
    // Load node's children
    if (inputNode.children.size() > 0) {
        for (size_t i = 0; i < inputNode.children.size(); i++) {
            loadNode(input.nodes[inputNode.children[i]], input, node, indexBuffer, vertexBuffer);
        }
    }

    if (inputNode.mesh > -1) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        // Iterate through all primitives of this node's mesh
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];
            uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
            // Vertices
            {
                const Real *positionBuffer = nullptr;
                const Real *normalsBuffer = nullptr;
                const Real *texCoordsBuffer = nullptr;
				const Real* colorBuffer = nullptr;
				uint32_t numColorComponents;

                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount =  static_cast<uint32_t>(accessor.count);
                }
                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    normalsBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
				if (glTFPrimitive.attributes.find("COLOR_0") != glTFPrimitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = input.accessors[glTFPrimitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = input.bufferViews[colorAccessor.bufferView];
					// Color buffer are either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					colorBuffer = reinterpret_cast<const float*>(&(input.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
				}

                // Append data to model's vertex buffer
                for (uint32_t v = 0; v < vertexCount; v++) {
                    Vertex vert{};
                    vert.pos = Vector3(&positionBuffer[v * 3]);
                    vert.normal = normalsBuffer ? Vector3(&normalsBuffer[v * 3]) : Vector3(0.0f);
                    vert.uv = texCoordsBuffer ? Vector2(&texCoordsBuffer[v * 2]) : Vector2(0.0f);
					if (colorBuffer) {
						switch (numColorComponents) {
						case 3:
							vert.color = Vector4(Vector3(&colorBuffer[v * 3]), 1.0f);
						case 4:
							vert.color = Vector4(&colorBuffer[v * 4]);
						}
					}
					else {
						vert.color = Vector4(1.0f);
					}
                    vertexBuffer.push_back(vert);
                }
            }
            // Indices
            {
                const tinygltf::Accessor &accessor = input.accessors[glTFPrimitive.indices];
                const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = input.buffers[bufferView.buffer];

                indexCount += static_cast<uint32_t>(accessor.count);

                // glTF supports different component types of indices
                switch (accessor.componentType) {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                        uint32_t *buf = new uint32_t[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                               accessor.count * sizeof(uint32_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                        uint16_t *buf = new uint16_t[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                               accessor.count * sizeof(uint16_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                        uint8_t *buf = new uint8_t[accessor.count];
                        memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset],
                               accessor.count * sizeof(uint8_t));
                        for (size_t index = 0; index < accessor.count; index++) {
                            indexBuffer.push_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    default:
                        return;
                }
            }
			Primitive *primitive = new Primitive();
            primitive->firstIndex = firstIndex;
			primitive->firstVertex = vertexStart;
            primitive->indexCount = indexCount;
			primitive->vertexCount = vertexCount;
            primitive->materialIndex = glTFPrimitive.material;
			node->mesh->primitives.push_back(primitive);
        }
    }

    if (parent) {
        parent->children.push_back(node);
    } else {
        mNodes.push_back(node);
    }
	mLinearNodes.push_back(node);
}

void GGVulkanGlTFModel::drawNode(VkCommandBuffer commandBuffer, Node *node) {
    if (node->mesh->primitives.size() > 0) {
        Matrix4 nodeMatrix = node->matrix;
        Node *currentParent = node->parent;
        while (currentParent) {
            nodeMatrix = nodeMatrix * currentParent->matrix;
            currentParent = currentParent->parent;
        }
        // Pass the final matrix to the vertex shader using push constants
        vkCmdPushConstants(commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4), &nodeMatrix);
        for (auto &primitive : node->mesh->primitives) {
            if (primitive->indexCount > 0) {
                // Get the texture index for this primitive
				if (!mImages.empty())
				{
					uint32_t textureIndex = mTextureIndices[mMaterials[primitive->materialIndex].baseColorTextureIndex];
					// Bind the descriptor for the current primitive's texture to set 1
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 1, 1,
						&mImages[textureIndex]->mDescriptorSet, 0, nullptr);
				}
                vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
            }
        }
    }
    for (auto &child : node->children) {
        drawNode(commandBuffer, child);
    }
}

// Draw the glTF scene starting at the top-level-mNodes
void GGVulkanGlTFModel::buildCommandBuffers(VkCommandBuffer commandBuffer) {
    if (mProtagonist) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                &mUniformbuffers[0]->mDescriptorSet, 0, nullptr);
    } else {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1,
                                &mUniformbuffers[1]->mDescriptorSet, 0, nullptr);
    }
    // All vertices and indices are stored in single buffers, so we only need to bind once
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    // Render all mNodes at top-level
    for (auto &node : mNodes) {
        drawNode(commandBuffer, node);
    }
}

}  // namespace Gange