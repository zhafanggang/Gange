#define TINYGLTF_IMPLEMENTATION
#include "GGVulkanGlTFModel.h"
#include "GGVulkanSingleHandle.h"
#include "Math/GGQuaternion.h"

namespace Gange {

GGVulkanGlTFModel::GGVulkanGlTFModel() {
    mVulkanDevice = GGVulkanSingleHandle::getVulkanDevicePtr();
    mCopyQueue = GGVulkanSingleHandle::getVkQueue();
}

/*
    Release all Vulkan resources acquired for the model
*/
GGVulkanGlTFModel::~GGVulkanGlTFModel() {}

void GGVulkanGlTFModel::initialize() {
    loadglTFFile("../data/models/walkMan/CesiumMan.gltf");
    // loadglTFFile("../data/models/FlightHelmet/FlightHelmet.gltf");
    mVertexBuffer->create();
    mUniformbuffer->generate();

    mShaderStages.resize(2);

    mShaderStages[0] = loadShader("../Data/shaders/single/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    mShaderStages[1] = loadShader("../data/shaders/single/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    setupDescriptorSet();
}

void GGVulkanGlTFModel::loadImages(tinygltf::Model &input) {
    // Images can be stored inside the glTF (which is the case for the sample model), so instead of directly
    // loading them from disk, we fetch them from the glTF loader and upload the buffers
    images.resize(input.images.size());
    for (size_t i = 0; i < input.images.size(); i++) {
        tinygltf::Image &glTFImage = input.images[i];
        // Get the image data from the glTF loader
        unsigned char *buffer = nullptr;
        VkDeviceSize bufferSize = 0;
        bool deleteBuffer = false;
        // We convert RGB-only images to RGBA, as most devices don't support RGB-formats in Vulkan
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
        images[i].loadFromPixels(buffer, bufferSize, VK_FORMAT_R8G8B8A8_UNORM, glTFImage.width, glTFImage.height);
        if (deleteBuffer) {
            delete[] buffer;
        }
    }
}

void GGVulkanGlTFModel::loadTextures(tinygltf::Model &input) {
    textures.resize(input.textures.size());
    for (size_t i = 0; i < input.textures.size(); i++) {
        textures[i].imageIndex = input.textures[i].source;
    }
}

void GGVulkanGlTFModel::loadMaterials(tinygltf::Model &input) {
    materials.resize(input.materials.size());
    for (size_t i = 0; i < input.materials.size(); i++) {
        // We only read the most basic properties required for our sample
        tinygltf::Material glTFMaterial = input.materials[i];
        // Get the base color factor
        if (glTFMaterial.values.find("baseColorFactor") != glTFMaterial.values.end()) {
            materials[i].baseColorFactor =
                Vector4((Real *) glTFMaterial.values["baseColorFactor"].ColorFactor().data());
        }
        // Get base color texture index
        if (glTFMaterial.values.find("baseColorTexture") != glTFMaterial.values.end()) {
            materials[i].baseColorTextureIndex = glTFMaterial.values["baseColorTexture"].TextureIndex();
        }
    }
}

void GGVulkanGlTFModel::loadglTFFile(std::string filename) {
    tinygltf::Model glTFInput;
    tinygltf::TinyGLTF gltfContext;
    std::string error, warning;

#if defined(__ANDROID__)
    tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
    bool fileLoaded = gltfContext.LoadASCIIFromFile(&glTFInput, &error, &warning, filename);

    std::vector<uint32_t> indexBuffer;
    std::vector<GGVulkanGlTFModel::Vertex> vertexBuffer;

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
        // vks::tools::exitFatal("Could not open the glTF file.\n\nThe file is part of the additional asset pack.\n\nRun
        // \"download_assets.py\" in the repository root to download the latest version.", -1);
        return;
    }

    size_t vertexBufferSize = vertexBuffer.size() * sizeof(GGVulkanGlTFModel::Vertex);
    size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
    mIndexBuffer->mIndexCount = static_cast<uint32_t>(indexBuffer.size());

    struct StagingBuffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
    } vertexStaging, indexStaging;

    // Create host visible staging buffers (source)
    VK_CHECK_RESULT(
        mVulkanDevice->createBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &vertexStaging.buffer, &vertexStaging.memory, vertexBuffer.data()));
    // Index data
    VK_CHECK_RESULT(
        mVulkanDevice->createBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    &indexStaging.buffer, &indexStaging.memory, indexBuffer.data()));

    // Create device local buffers (target)
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mVertexBuffer->getBuffer()->buffer, &mVertexBuffer->getBuffer()->memory));
    VK_CHECK_RESULT(mVulkanDevice->createBuffer(
        indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mIndexBuffer->getBuffer()->buffer, &mIndexBuffer->getBuffer()->memory));

    VkCommandBuffer copyCmd = mVulkanDevice->beginOnceCommand();
    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, vertexStaging.buffer, mVertexBuffer->getBuffer()->buffer, 1, &copyRegion);
    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd, indexStaging.buffer, mIndexBuffer->getBuffer()->buffer, 1, &copyRegion);
    mVulkanDevice->endOnceCommand(copyCmd, mCopyQueue);

    // Free staging resources
    vkDestroyBuffer(mVulkanDevice->logicalDevice, vertexStaging.buffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, vertexStaging.memory, nullptr);
    vkDestroyBuffer(mVulkanDevice->logicalDevice, indexStaging.buffer, nullptr);
    vkFreeMemory(mVulkanDevice->logicalDevice, indexStaging.memory, nullptr);
}

void GGVulkanGlTFModel::loadNode(const tinygltf::Node &inputNode, const tinygltf::Model &input,
                                 GGVulkanGlTFModel::Node *parent, std::vector<uint32_t> &indexBuffer,
                                 std::vector<GGVulkanGlTFModel::Vertex> &vertexBuffer) {
    GGVulkanGlTFModel::Node node{};
    node.matrix = Matrix4::IDENTITY;

    // Get the local node matrix
    // It's either made up from translation, rotation, scale or a 4x4 matrix
    if (inputNode.translation.size() == 3) {
        node.matrix.setTrans(Vector3((Real *) inputNode.translation.data()));
    }
    if (inputNode.rotation.size() == 4) {
        Quaternion qu((Real *) inputNode.rotation.data());
        node.matrix = Matrix4(qu);
    }
    if (inputNode.scale.size() == 3) {
        node.matrix.setScale(Vector3((Real *) inputNode.scale.data()));
    }
    if (inputNode.matrix.size() == 16) {
        node.matrix = Matrix4((Real *) inputNode.matrix.data());
    };

    // Load node's children
    if (inputNode.children.size() > 0) {
        for (size_t i = 0; i < inputNode.children.size(); i++) {
            loadNode(input.nodes[inputNode.children[i]], input, &node, indexBuffer, vertexBuffer);
        }
    }

    // If the node contains mesh data, we load vertices and indices from the buffers
    // In glTF this is done via accessors and buffer views
    if (inputNode.mesh > -1) {
        const tinygltf::Mesh mesh = input.meshes[inputNode.mesh];
        // Iterate through all primitives of this node's mesh
        for (size_t i = 0; i < mesh.primitives.size(); i++) {
            const tinygltf::Primitive &glTFPrimitive = mesh.primitives[i];
            uint32_t firstIndex = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
            // Vertices
            {
                const Real *positionBuffer = nullptr;
                const Real *normalsBuffer = nullptr;
                const Real *texCoordsBuffer = nullptr;
                size_t vertexCount = 0;

                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("POSITION") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("POSITION")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    positionBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    vertexCount = accessor.count;
                }
                // Get buffer data for vertex normals
                if (glTFPrimitive.attributes.find("NORMAL") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    normalsBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }
                // Get buffer data for vertex texture coordinates
                // glTF supports multiple sets, we only load the first one
                if (glTFPrimitive.attributes.find("TEXCOORD_0") != glTFPrimitive.attributes.end()) {
                    const tinygltf::Accessor &accessor =
                        input.accessors[glTFPrimitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &view = input.bufferViews[accessor.bufferView];
                    texCoordsBuffer = reinterpret_cast<const float *>(
                        &(input.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                }

                // Append data to model's vertex buffer
                for (size_t v = 0; v < vertexCount; v++) {
                    Vertex vert{};
                    vert.pos = Vector3(&positionBuffer[v * 3]);
                    vert.normal = normalsBuffer ? Vector3(&normalsBuffer[v * 3]) : Vector3(0.0f);
                    vert.uv = texCoordsBuffer ? Vector2(&texCoordsBuffer[v * 2]) : Vector2(0.0f);
                    vert.color = Vector3(1.0f);
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
                        // std::cerr << "Index component type " << accessor.componentType << " not supported!" <<
                        // std::endl;
                        return;
                }
            }
            Primitive primitive{};
            primitive.firstIndex = firstIndex;
            primitive.indexCount = indexCount;
            primitive.materialIndex = glTFPrimitive.material;
            node.mesh.primitives.push_back(primitive);
        }
    }

    if (parent) {
        parent->children.push_back(node);
    } else {
        nodes.push_back(node);
    }
}

/*
    glTF rendering functions
*/

// Draw a single node including child nodes (if present)
void GGVulkanGlTFModel::drawNode(VkCommandBuffer commandBuffer, GGVulkanGlTFModel::Node node) {
    if (node.mesh.primitives.size() > 0) {
        // Pass the node's matrix via push constants
        // Traverse the node hierarchy to the top-most parent to get the final matrix of the current node
        Matrix4 nodeMatrix = node.matrix;
        GGVulkanGlTFModel::Node *currentParent = node.parent;
        while (currentParent) {
            nodeMatrix = currentParent->matrix * nodeMatrix;
            currentParent = currentParent->parent;
        }
        // Pass the final matrix to the vertex shader using push constants
        vkCmdPushConstants(commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Matrix4), &nodeMatrix);
        for (GGVulkanGlTFModel::Primitive &primitive : node.mesh.primitives) {
            if (primitive.indexCount > 0) {
                // Get the texture index for this primitive
                GGVulkanGlTFModel::Texture texture = textures[materials[primitive.materialIndex].baseColorTextureIndex];
                // Bind the descriptor for the current primitive's texture to set 1
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 1, 1,
                                        &images[texture.imageIndex].mDescriptorSet, 0, nullptr);
                vkCmdDrawIndexed(commandBuffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
            }
        }
    }
    for (auto &child : node.children) {
        drawNode(commandBuffer, child);
    }
}

// Draw the glTF scene starting at the top-level-nodes
void GGVulkanGlTFModel::draw(VkCommandBuffer commandBuffer) {
    // All vertices and indices are stored in single buffers, so we only need to bind once
    VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mVertexBuffer->getBuffer()->buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->getBuffer()->buffer, 0, VK_INDEX_TYPE_UINT32);
    // Render all nodes at top-level
    for (auto &node : nodes) {
        drawNode(commandBuffer, node);
    }
}

void GGVulkanGlTFModel::setupDescriptorSet() {
    /*
    This sample uses separate descriptor sets (and layouts) for the matrices and materials (textures)
*/

    std::vector<VkDescriptorPoolSize> poolSizes = {
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
        // One combined image sampler per material image/texture
        initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         static_cast<uint32_t>(images.size())),
    };
    // Number of descriptor sets = One for the scene ubo + one per image + one per skin
    const uint32_t maxSetCount = static_cast<uint32_t>(images.size()) + 1;
    VkDescriptorPoolCreateInfo descriptorPoolInfo = initializers::descriptorPoolCreateInfo(poolSizes, maxSetCount);
    VK_CHECK_RESULT(
        vkCreateDescriptorPool(mVulkanDevice->logicalDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool));

    // Descriptor set layouts
    VkDescriptorSetLayoutBinding setLayoutBinding{};
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCI =
        initializers::descriptorSetLayoutCreateInfo(&setLayoutBinding, 1);
    mDescriptorSetLayouts.resize(2);

    // Descriptor set layout for passing matrices
    setLayoutBinding =
        initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
                                                &mDescriptorSetLayouts[0]));

    // Descriptor set layout for passing material textures
    setLayoutBinding = initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                VK_SHADER_STAGE_FRAGMENT_BIT, 0);
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(mVulkanDevice->logicalDevice, &descriptorSetLayoutCI, nullptr,
                                                &mDescriptorSetLayouts[1]));

    // The pipeline layout uses three sets:
    // Set 0 = Scene matrices (VS)
    // Set 1 = Material texture (FS)

    VkPipelineLayoutCreateInfo pipelineLayoutCI = initializers::pipelineLayoutCreateInfo(
        mDescriptorSetLayouts.data(), static_cast<uint32_t>(mDescriptorSetLayouts.size()));

    VkPushConstantRange pushConstantRange =
        initializers::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(Matrix4), 0);
    // Push constant ranges are part of the pipeline layout
    pipelineLayoutCI.pushConstantRangeCount = 1;
    pipelineLayoutCI.pPushConstantRanges = &pushConstantRange;

    VK_CHECK_RESULT(vkCreatePipelineLayout(mVulkanDevice->logicalDevice, &pipelineLayoutCI, nullptr, &mPipelineLayout));

    // Descriptor set for scene matrices
    VkDescriptorSetAllocateInfo allocInfo =
        initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayouts[0], 1);
    VK_CHECK_RESULT(
        vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &mUniformbuffer->mDescriptorSet));
    VkWriteDescriptorSet writeDescriptorSet = initializers::writeDescriptorSet(
        mUniformbuffer->mDescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &mUniformbuffer->getBuffer()->descriptor);
    vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);

    // Descriptor sets for glTF model materials
    for (auto &image : images) {
        const VkDescriptorSetAllocateInfo allocInfo =
            initializers::descriptorSetAllocateInfo(mDescriptorPool, &mDescriptorSetLayouts[1], 1);
        VK_CHECK_RESULT(vkAllocateDescriptorSets(mVulkanDevice->logicalDevice, &allocInfo, &image.mDescriptorSet));
        VkWriteDescriptorSet writeDescriptorSet = initializers::writeDescriptorSet(
            image.mDescriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, &image.mDescriptorImageInfo);
        vkUpdateDescriptorSets(mVulkanDevice->logicalDevice, 1, &writeDescriptorSet, 0, nullptr);
    }
}

}  // namespace Gange