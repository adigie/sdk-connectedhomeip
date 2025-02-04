#pragma once

namespace chip {
namespace app {

enum class MarkAttributeDirty
{
    kIfChanged,
    kNo,
    // kYes might need to be used if the attribute value was previously changed
    // without reporting, and now is being set in a situation where we know
    // reporting needs to be triggered (e.g. because QuieterReportingAttribute
    // indicated that).
    kYes,
};
}
} // namespace chip
