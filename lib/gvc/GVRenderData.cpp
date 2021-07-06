#include "GVRenderData.h"
#include <gvc/gvc.h>

GVRenderData::GVRenderData(char *data, size_t length)
    : m_data(data), m_length(length) {}
GVRenderData::~GVRenderData() { gvFreeRenderData(m_data); }
