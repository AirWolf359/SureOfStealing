#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();

    // The primary ini ships with the mod and is never written to, so a mod
    // manager keeps ownership of it and a mod update replaces it cleanly.
    if (ini.LoadFile(R"(.\Data\SKSE\Plugins\SureOfStealing.ini)") < 0) {
        logger::warn("Could not read SureOfStealing.ini, falling back to defaults");
    }

    // A supplemental file, if the user creates one, is layered on top: keys it
    // defines win, keys it leaves out fall through to the primary. LoadFile
    // merges into the existing data rather than resetting it, and CSimpleIniA
    // does not allow duplicate keys, so loading second is what makes it win.
    if (ini.LoadFile(R"(.\Data\SKSE\Plugins\SureOfStealingCustom.ini)") >= 0) {
        logger::info("Applied overrides from SureOfStealingCustom.ini");
    }

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
    logger::info("\tbChairsAndBenches = {}", chairs_and_benches.load());
    logger::info("\tbRequireSneakToSteal = {}", require_sneak_to_steal.load());
    logger::info("\tbDoubleTapWhileSneaking = {}", double_tap_while_sneaking.load());
    logger::info("");
}
