#pragma once

class Utility : public Singleton<Utility>
{
public:
    inline static RE::ObjectRefHandle  last_activation{};
    inline static RE::TESGlobal*      immersive_interactions_global{};
    inline static bool                immersive_interactions_present{};
    inline static RE::NiPointer<RE::TESObjectREFR> crosshair_ref{};

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

    // No-op when Immersive Interactions is not installed, in which case the
    // global was never looked up.
    static void SetImmersiveInteractions(float a_value) noexcept
    {
        if (immersive_interactions_present) {
            immersive_interactions_global->value = a_value;
        }
    }

    // Restores the "not stealing" state on a pass-through, leaving any value the
    // caller has deliberately set this frame alone.
    static void ResetImmersiveInteractions() noexcept
    {
        if (immersive_interactions_present && immersive_interactions_global->value == 0.0f) {
            immersive_interactions_global->value = 1.0f;
        }
    }

    // Drops a pending first interaction. Used when the player is sneaking, which
    // bypasses confirmation entirely.
    static void ClearPendingActivation() noexcept
    {
        last_activation = {};
        SetImmersiveInteractions(1.0f);
    }

    // True when a_ref is the reference armed by a previous interaction: this is
    // the confirming second interaction, so the pending state is consumed and the
    // caller should let the action through. Returns false without side effects
    // when there is nothing pending, or it was for a different reference.
    static bool ConsumeRepeatInteraction(RE::TESObjectREFR* a_ref, std::string_view a_action) noexcept
    {
        if (const auto last_ref = last_activation.get(); last_ref) {
            if (a_ref->GetFormID() == last_ref->GetFormID()) {
                logger::debug("Allowing {} for {} (0x{:x})", a_action, a_ref->GetName(), a_ref->GetFormID());
                SetImmersiveInteractions(0.0f);
                last_activation = {};

                return true;
            }
        }

        return false;
    }

    // Arms a_ref so the next interaction on it counts as confirmation. The caller
    // blocks this one, by whichever means suits its hook.
    static void ArmPendingInteraction(RE::TESObjectREFR* a_ref, std::string_view a_action) noexcept
    {
        logger::debug("Blocking {} for {} (0x{:x})", a_action, a_ref->GetName(), a_ref->GetFormID());
        last_activation = a_ref->GetHandle();
        SetImmersiveInteractions(1.0f);
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
