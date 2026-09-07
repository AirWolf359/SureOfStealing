#pragma once

class Settings : public Singleton<Settings>
{
public:
    static void LoadSettings() noexcept;

    // Atomic because these are read from the hook thunks, which the game
    // dispatches across worker threads - interaction logging showed a different
    // thread almost every time - while the planned in-game menu will write them
    // from the render thread. Plain bools would be a data race, benign on x86 in
    // practice but undefined all the same. Reads and assignment are implicit, so
    // call sites are unchanged; only formatting needs an explicit load().
    inline static std::atomic<bool> debug_logging{};

    inline static std::atomic<bool> chairs_and_benches{};

    inline static std::atomic<bool> require_sneak_to_steal{};

    inline static std::atomic<bool> double_tap_while_sneaking{};
};
