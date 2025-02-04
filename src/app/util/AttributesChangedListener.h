#pragma once

#include <app/AttributePathParams.h>

namespace chip {
namespace app {

/// Notification object of a specific path being changed
class AttributesChangedListener
{
public:
    virtual ~AttributesChangedListener() = default;

    /// Called when the set of attributes identified by AttributePathParams (which may contain wildcards) is to be considered dirty.
    virtual void MarkDirty(const AttributePathParams & path) = 0;
};

} // namespace app
} // namespace chip
