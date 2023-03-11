// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include "vRenderer/vRenderer.h"
#include "vRenderer/helper_structs/RenderingHelpers.h"
#include "vRenderer/helpers/helpers.h"
#include "vRenderer/Device.h"
#include "vRenderer/Buffer/Buffer.h"
#include "vRenderer/Buffer/VertexBuffer.h"
#include "vRenderer/Buffer/IndexBuffer.h"
#include "vRenderer/Buffer/UniformBuffer.h"
#include "vRenderer/SwapChain.h"

#endif //PCH_H
