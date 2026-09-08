#include "Hooks.h"

#include "Settings.h"
#include "Utility.h"

namespace Hooks
{
    void Install() noexcept
    {
        if (REL::Module::IsVR()) {
            PickupObject::idx = 206; // 0xce
        }

        stl::write_vfunc<RE::PlayerCharacter, PickupObject>();
        logger::info("Installed PlayerCharacter::PickUpObject hook");

        stl::write_vfunc<RE::TESFlora, ActivateFlora>();
        logger::info("Installed TESFlora::Activate hook");

        // Installed unconditionally: a vfunc hook cannot be safely removed at
        // runtime, so the setting is checked inside the thunk instead. This lets
        // bChairsAndBenches be toggled while the game is running.
        stl::write_vfunc<RE::TESFurniture, ActivateFurniture>();
        logger::info("Installed TESFurniture::Activate hook (bChairsAndBenches = {})", Settings::chairs_and_benches.load());

        stl::write_vfunc<RE::TESObjectCONT, ActivateContainer>();
        logger::info("Installed TESObjectCONT::Activate hook");
    }

    void PickupObject::Thunk(RE::PlayerCharacter* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound) noexcept
    {
        if (!a_object) {
            return func(a_this, a_object, a_count, a_arg3, a_playSound);
        }

        const auto sneaking{ a_this->IsSneaking() };

        if (a_this->Is3DLoaded() && (!sneaking || Settings::double_tap_while_sneaking)) {
            const auto form_id{ a_object->GetFormID() };
            if (a_object->IsCrimeToActivate()) {
                // Skip unread books
                if (a_object->GetBaseObject()->IsBook()) {
                    if (const auto book{ a_object->GetBaseObject()->As<RE::TESObjectBOOK>() }; !book->IsRead())
                        return func(a_this, a_object, a_count, a_arg3, a_playSound);
                }
                const auto& crosshair{ Utility::crosshair_ref };
                if (!crosshair || crosshair->GetFormID() != form_id) {
                    return func(a_this, a_object, a_count, a_arg3, a_playSound);
                }
                if (!sneaking && Settings::require_sneak_to_steal) {
                    Utility::RefusePendingInteraction(a_object, "steal"sv);

                    return;
                }
                if (Utility::ConsumeRepeatInteraction(a_object, "steal"sv)) {
                    return func(a_this, a_object, a_count, a_arg3, a_playSound);
                }
                Utility::ArmPendingInteraction(a_object, "steal"sv);

                return;
            }
        }
        else if (a_this->Is3DLoaded() && sneaking && Utility::last_activation.get()) {
            Utility::ClearPendingActivation();

            return func(a_this, a_object, a_count, a_arg3, a_playSound);
        }

        Utility::ResetImmersiveInteractions();

        return func(a_this, a_object, a_count, a_arg3, a_playSound);
    }

    bool ActivateFlora::Thunk(RE::TESFlora* a_this, RE::TESObjectREFR* a_targetRef, RE::TESObjectREFR* a_activatorRef, std::uint8_t a_arg3, RE::TESBoundObject* a_object,
                              std::int32_t a_targetCount) noexcept
    {
        if (!a_targetRef || !a_activatorRef) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        const auto name{ a_targetRef->GetName() };

        if ("Coin Purse"sv.compare(name)) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        if (const auto player{ RE::PlayerCharacter::GetSingleton() }; a_activatorRef->IsPlayerRef()) {
            const auto sneaking{ player->IsSneaking() };

            if (player->Is3DLoaded() && (!sneaking || Settings::double_tap_while_sneaking)) {
                if (a_targetRef->IsCrimeToActivate()) {
                    if (!sneaking && Settings::require_sneak_to_steal) {
                        Utility::RefusePendingInteraction(a_targetRef, "steal"sv);

                        return func(a_this, nullptr, a_activatorRef, a_arg3, a_object, 0);
                    }
                    if (Utility::ConsumeRepeatInteraction(a_targetRef, "steal"sv)) {
                        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
                    }
                    Utility::ArmPendingInteraction(a_targetRef, "steal"sv);

                    return func(a_this, nullptr, a_activatorRef, a_arg3, a_object, 0);
                }
            }
            else if (player->Is3DLoaded() && sneaking && Utility::last_activation.get()) {
                Utility::ClearPendingActivation();

                return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
            }

            Utility::ResetImmersiveInteractions();
        }

        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
    }

    bool ActivateFurniture::Thunk(RE::TESFurniture* a_this, RE::TESObjectREFR* a_targetRef, RE::TESObjectREFR* a_activatorRef, std::uint8_t a_arg3, RE::TESBoundObject* a_object,
                                  std::int32_t a_targetCount) noexcept
    {
        if (!Settings::chairs_and_benches) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        if (!a_targetRef || !a_activatorRef) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        const auto name{ a_targetRef->GetName() };

        if ("Bench"sv.compare(name) && "Chair"sv.compare(name)) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        if (const auto player{ RE::PlayerCharacter::GetSingleton() }; a_activatorRef->IsPlayerRef()) {
            // Standing up comes through this same hook. Moving forward to get out
            // of a chair activates the furniture; it is not a separate path. Getting
            // out needs no confirming, since the point is to stop sitting down by
            // accident, and refusing it strands the player in the seat because
            // sneaking is not possible while seated.
            //
            // Under the two-interaction default this went unnoticed: holding the
            // movement key repeats the activation, so the first arms the chair and
            // the next consumes it within a frame or two. That is the same reason a
            // repeated press cannot protect anyone who habitually holds or mashes
            // the activate key, which is what bRequireSneakToSit exists to address.
            // TEMPORARY DIAGNOSTIC - not for merge.
            logger::debug("Furniture activation: sitSleepState={} IsSitting={} occupied={:x}",
                          static_cast<std::uint32_t>(player->GetSitSleepState()),
                          player->IsSitting(),
                          player->GetOccupiedFurniture() ? player->GetOccupiedFurniture().get()->GetFormID() : 0u);

            if (player->IsSitting()) {
                logger::debug("Passing furniture activation through: already seated");

                return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
            }

            const auto sneaking{ player->IsSneaking() };

            // Sitting is not a crime, so unlike the other hooks there is no
            // IsCrimeToActivate check: every chair and bench is confirmed.
            if (player->Is3DLoaded() && (!sneaking || Settings::double_tap_while_sneaking)) {
                // Confirming by pressing twice does not help someone who holds
                // activate down to loot a table, since the repeat satisfies it by
                // itself. Requiring sneak instead cannot be triggered by accident,
                // while still leaving sitting possible for the quests that need it.
                if (!sneaking && Settings::require_sneak_to_sit) {
                    Utility::RefusePendingInteraction(a_targetRef, "sitting"sv);

                    return false;
                }
                if (Utility::ConsumeRepeatInteraction(a_targetRef, "sitting"sv)) {
                    return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
                }
                Utility::ArmPendingInteraction(a_targetRef, "sitting"sv);

                return false;
            }
            if (player->Is3DLoaded() && sneaking && Utility::last_activation.get()) {
                Utility::ClearPendingActivation();

                return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
            }

            Utility::ResetImmersiveInteractions();
        }

        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
    }

    bool ActivateContainer::Thunk(RE::TESObjectCONT* a_this, RE::TESObjectREFR* a_targetRef, RE::TESObjectREFR* a_activatorRef, std::uint8_t a_arg3, RE::TESBoundObject* a_object,
                                  std::int32_t a_targetCount) noexcept
    {
        if (!a_activatorRef || !a_targetRef) {
            return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
        }

        if (const auto player{ RE::PlayerCharacter::GetSingleton() }; a_activatorRef->IsPlayerRef()) {
            const auto sneaking{ player->IsSneaking() };

            if (player->Is3DLoaded() && (!sneaking || Settings::double_tap_while_sneaking)) {
                if (a_targetRef->IsCrimeToActivate()) {
                    if (Utility::ConsumeRepeatInteraction(a_targetRef, "activation"sv)) {
                        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
                    }

                    // An empty container has nothing to steal, so requiring
                    // confirmation for it is just friction. Checked after the
                    // confirmation above, so a container that is already armed
                    // still opens on its second interaction.
                    //
                    // Tested against the real inventory rather than the activation
                    // prompt. The prompt is display text: UI mods rewrite it - one
                    // was observed replacing it with Scaleform markup containing no
                    // readable words at all - localisation translates it, and it
                    // also wrongly matched any container merely named "Empty".
                    if (a_targetRef->GetInventoryItemCount() == 0) {
                        logger::debug("Skipping confirmation for empty container {} (0x{:x})", a_targetRef->GetName(), a_targetRef->GetFormID());

                        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
                    }

                    // Placed after the empty-container check: an empty container
                    // holds nothing to steal, so refusing it would be friction
                    // without purpose even when sneaking is required.
                    if (!sneaking && Settings::require_sneak_to_steal) {
                        Utility::RefusePendingInteraction(a_targetRef, "activation"sv);

                        return false;
                    }

                    Utility::ArmPendingInteraction(a_targetRef, "activation"sv);

                    return false;
                }
            }
            else if (player->Is3DLoaded() && sneaking && Utility::last_activation.get()) {
                Utility::ClearPendingActivation();

                return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
            }

            Utility::ResetImmersiveInteractions();
        }

        return func(a_this, a_targetRef, a_activatorRef, a_arg3, a_object, a_targetCount);
    }
} // namespace Hooks
