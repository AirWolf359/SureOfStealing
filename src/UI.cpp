#include "UI.h"

#include "SKSEMenuFramework.h"
#include "Settings.h"

namespace
{
    // Draws one checkbox bound to a setting, and reports whether the user just
    // changed it. Settings are atomic, so they cannot be handed to the widget
    // directly; a local copy is round-tripped instead, which is also what gives
    // us the change signal.
    bool Toggle(const char* a_label, std::atomic<bool>& a_setting, const char* a_help) noexcept
    {
        auto       value{ a_setting.load() };
        const auto changed{ ImGuiMCP::Checkbox(a_label, &value) };

        if (changed) {
            a_setting.store(value);
        }

        if (ImGuiMCP::IsItemHovered()) {
            ImGuiMCP::SetTooltip("%s", a_help);
        }

        return changed;
    }
}

namespace UI
{
    void Register() noexcept
    {
        if (!SKSEMenuFramework::IsInstalled()) {
            logger::info("SKSE Menu Framework not installed, in-game settings menu unavailable");

            return;
        }

        SKSEMenuFramework::SetSection("Sure of Stealing");
        SKSEMenuFramework::AddSectionItem("Settings", RenderSettings);
        logger::info("Registered in-game settings menu");
    }

    void __stdcall RenderSettings()
    {
        auto changed{ false };

        ImGuiMCP::TextWrapped(
            "Changes apply immediately and are written to SureOfStealingCustom.ini. "
            "The ini shipped with the mod is never modified.");
        ImGuiMCP::Separator();

        changed |= Toggle("Chairs and benches", Settings::chairs_and_benches,
                          "Also require a second interaction before sitting on a chair or bench.");

        changed |= Toggle("Require sneak to steal", Settings::require_sneak_to_steal,
                          "Refuse to take owned items unless you are sneaking, rather than asking for a "
                          "second interaction. Applies to stealing only, never to sitting.");

        changed |= Toggle("Double tap while sneaking", Settings::double_tap_while_sneaking,
                          "Require the second interaction while sneaking as well. Unlike the setting "
                          "above, this also covers chairs and benches.");

        ImGuiMCP::Separator();

        if (Toggle("Debug logging", Settings::debug_logging,
                   "Write verbose logging to the SKSE log. Useful when reporting a problem.")) {
            // Applied here as well as saved, so the setting takes effect without
            // needing a restart like every other option on this page.
            spdlog::set_level(Settings::debug_logging ? spdlog::level::debug : spdlog::level::info);
            changed = true;
        }

        if (changed) {
            Settings::Save();
        }
    }
}
