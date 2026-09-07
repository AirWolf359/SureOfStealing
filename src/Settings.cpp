#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\SureOfStealing.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    chairs_and_benches = ini.GetBoolValue("General", "bChairsAndBenches");

    // Default passed explicitly so behaviour does not depend on the library's
    // own default for a key that an existing ini will not contain.
    require_sneak_to_steal = ini.GetBoolValue("General", "bRequireSneakToSteal", false);
    double_tap_while_sneaking = ini.GetBoolValue("General", "bDoubleTapWhileSneaking", false);

    logger::info("Loaded settings");
    logger::info("\tbChairsAndBenches = {}", chairs_and_benches);
    logger::info("\tbRequireSneakToSteal = {}", require_sneak_to_steal);
    logger::info("\tbDoubleTapWhileSneaking = {}", double_tap_while_sneaking);
    logger::info("");
}
