#include "Settings.h"

namespace
{
    // The primary ships with the mod and is only ever read. The supplemental is
    // the only file this plugin writes.
    constexpr auto primary_ini{ R"(.\Data\SKSE\Plugins\SureOfStealing.ini)" };
    constexpr auto supplemental_ini{ R"(.\Data\SKSE\Plugins\SureOfStealingCustom.ini)" };
}

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();

    // The primary ini ships with the mod and is never written to, so a mod
    // manager keeps ownership of it and a mod update replaces it cleanly.
    if (ini.LoadFile(primary_ini) < 0) {
        logger::warn("Could not read SureOfStealing.ini, falling back to defaults");
    }

    // A supplemental file, if the user creates one, is layered on top: keys it
    // defines win, keys it leaves out fall through to the primary. LoadFile
    // merges into the existing data rather than resetting it, and CSimpleIniA
    // does not allow duplicate keys, so loading second is what makes it win.
    if (ini.LoadFile(supplemental_ini) >= 0) {
        logger::info("Applied overrides from SureOfStealingCustom.ini");
    }

    debug_logging = ini.GetBoolValue("Log", "Debug", false);

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    chairs_and_benches = ini.GetBoolValue("General", "bChairsAndBenches");

    // Default passed explicitly so behaviour does not depend on the library's
    // own default for a key that an existing ini will not contain.
    require_sneak_to_sit = ini.GetBoolValue("General", "bRequireSneakToSit", false);
    require_sneak_to_steal = ini.GetBoolValue("General", "bRequireSneakToSteal", false);
    double_tap_while_sneaking = ini.GetBoolValue("General", "bDoubleTapWhileSneaking", false);

    logger::info("Loaded settings");
    logger::info("\tbChairsAndBenches = {}", chairs_and_benches.load());
    logger::info("\tbRequireSneakToSit = {}", require_sneak_to_sit.load());
    logger::info("\tbRequireSneakToSteal = {}", require_sneak_to_steal.load());
    logger::info("\tbDoubleTapWhileSneaking = {}", double_tap_while_sneaking.load());
    logger::info("");
}

void Settings::Save() noexcept
{
    CSimpleIniA ini;

    ini.SetUnicode();

    // Read what is already there first, so other keys and any comments the user
    // has written survive being rewritten.
    ini.LoadFile(supplemental_ini);

    ini.SetBoolValue("General", "bChairsAndBenches", chairs_and_benches.load());
    ini.SetBoolValue("General", "bRequireSneakToSit", require_sneak_to_sit.load());
    ini.SetBoolValue("General", "bRequireSneakToSteal", require_sneak_to_steal.load());
    ini.SetBoolValue("General", "bDoubleTapWhileSneaking", double_tap_while_sneaking.load());
    ini.SetBoolValue("Log", "Debug", debug_logging.load());

    if (ini.SaveFile(supplemental_ini) < 0) {
        logger::error("Could not write SureOfStealingCustom.ini");

        return;
    }

    logger::info("Saved settings to SureOfStealingCustom.ini");
}
