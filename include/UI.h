#pragma once

namespace UI
{
    // Registers the in-game settings menu, if SKSE Menu Framework is installed.
    // Does nothing when it is not, which is the supported configuration for
    // anyone who would rather just edit the ini.
    void Register() noexcept;

    // __stdcall is required: the framework calls this through a
    // void(__stdcall*)() function pointer.
    void __stdcall RenderSettings();
}
