// DCE-prevention entry point for libmediapipe_face_landmarker.so.
// Bazel cc_binary requires main(); volatile function-pointer references prevent the
// linker from stripping MediaPipe symbols under -c opt / LTO.
#include "mediapipe/tasks/cc/vision/face_landmarker/face_landmarker.h"
#include "mediapipe/tasks/cc/vision/face_landmarker/face_landmarker_result.h"
#include "mediapipe/tasks/cc/core/base_options.h"
#include "mediapipe/tasks/cc/vision/core/running_mode.h"
#include "mediapipe/gpu/gl_texture_buffer.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_buffer_format.h"
#include "mediapipe/framework/formats/image.h"

using namespace mediapipe;
using namespace mediapipe::tasks::vision::face_landmarker;

volatile void* g_prevent_dce = nullptr;

int main(int argc, char** argv) {
    // Force reference to FaceLandmarker::Create (static factory)
    g_prevent_dce = reinterpret_cast<void*>(&FaceLandmarker::Create);

    // Force reference to GlTextureBuffer::Wrap (static factory)
    using WrapFunc = std::unique_ptr<GlTextureBuffer> (*)(
        GLenum, GLuint, int, int, GpuBufferFormat,
        GlTextureBuffer::DeletionCallback);
    g_prevent_dce = reinterpret_cast<void*>(
        static_cast<WrapFunc>(&GlTextureBuffer::Wrap));

    // Force reference to Image (GPU-buffer ctor)
    (void)sizeof(mediapipe::Image);

    // Force reference to GpuBufferFormat enum
    volatile auto fmt = GpuBufferFormat::kRGBA32;
    (void)fmt;

    return 0;
}
