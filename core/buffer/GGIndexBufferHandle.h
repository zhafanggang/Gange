#ifndef _GG_INDEX_BUFFER_HANDLE_
#define _GG_INDEX_BUFFER_HANDLE_
#include "GGBufferHandle.h"
#include <vector>

namespace Gange {

class GGIndexBufferHandle : public GGBufferHandle {

public:
    GGIndexBufferHandle();

    virtual ~GGIndexBufferHandle();

    virtual void generate() override;

    void generate(std::vector<uint32_t> &indices);

    uint32_t mIndexCount = 0;
};

}  // namespace Gange

#endif