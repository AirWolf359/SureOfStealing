#pragma once

class Utility : public Singleton<Utility>
{
public:
    inline static RE::TESObjectREFR*  last_activation{};
    inline static RE::TESGlobal*      immersive_interactions_global{};
    inline static bool                immersive_interactions_present{};
    inline static RE::ObjectRefHandle crosshair_ref{};

    static void InitGlobal() noexcept
    {
        if (const auto handler{ RE::TESDataHandler::GetSingleton() }; handler->LookupModByName("ImmersiveInteractions.esp"sv)) {
            immersive_interactions_present = true;
            logger::info("Immersive Interactions compatibility enabled");
            if (!handler->LookupModByName("Sure of Stealing - Immersive Interactions Patch.esp"sv)) {
                logger::error("Immersive Interactions patch not found");
                stl::report_and_fail("ERROR: Sure of Stealing - Immersive Interactions Patch not installed"sv);
            }
            immersive_interactions_global = handler->LookupForm<RE::TESGlobal>(0x800, "Sure of Stealing - Immersive Interactions Patch.esp"sv);
            logger::info("Cached Immersive Interactions global: {} ({})", immersive_interactions_global->GetFormEditorID(), immersive_interactions_global->value);
            return;
        }
        logger::info("Immersive Interactions not present");
    }
};

class CrosshairRefHandler : public EventSingleton<CrosshairRefHandler, SKSE::CrosshairRefEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*) override
    {
        if (a_event) {
            Utility::crosshair_ref = a_event->crosshairRef;
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};
