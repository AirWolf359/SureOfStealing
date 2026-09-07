#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    inline static bool debug_logging{};

    inline static bool chairs_and_benches{};

    inline static bool require_sneak{};

    inline static bool double_tap_while_sneaking{};
};
