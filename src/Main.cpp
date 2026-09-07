#include "Hooks.h"
#include "Logging.h"
#include "Settings.h"
#include "UI.h"
#include "Utility.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Settings::LoadSettings();
        Utility::InitGlobal();
        CrosshairRefHandler::Register();
        Hooks::Install();
        UI::Register();
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitializeLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", plugin->GetName(), version);

    // Pass false so CommonLibSSE does not initialise logging itself. Its
    // Init() otherwise calls log::init(), which installs its own default
    // logger and pattern, reopens the log file - discarding anything logged
    // before this point - and sets flush_on(info) in release builds, leaving
    // every debug line stranded in an unflushed buffer.
    Init(skse, false);

    if (const auto messaging = SKSE::GetMessagingInterface(); !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", plugin->GetName());
    logger::info("");

    return true;
}
