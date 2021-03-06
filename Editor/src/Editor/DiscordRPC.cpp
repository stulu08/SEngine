#include "DiscordRPC.h"
#include <discord_rpc.h>

namespace Stulu {

    std::future<void> rpc;

    static void handleDiscordReady(const DiscordUser* connectedUser)
    {
        CORE_INFO("Discord: connected to user {0}#{1} - {2}",
            connectedUser->username,
            connectedUser->discriminator,
            connectedUser->userId);
    }
    static void handleDiscordDisconnected(int errcode, const char* message)
    {
        CORE_ERROR("Discord: disconnected ({0}: {1})", errcode, message);
    }
    static void handleDiscordError(int errcode, const char* message)
    {
        CORE_ERROR("Discord: error ({0}: {1})", errcode, message);
    }
    static void handleDiscordJoin(const char* secret)
    {
        CORE_INFO("Discord: join ({0})", secret);
    }
    static void handleDiscordSpectate(const char* secret)
    {
        CORE_INFO("Discord: spectate ({0})", secret);
    }
   
    std::string DiscordRPC::s_state = "Waiting on loading screen";
    std::string DiscordRPC::s_details = "Editing a scene";
    std::string DiscordRPC::s_largeImageKey = "engine-logo-bg";

    bool DiscordRPC::s_running = false;
    bool DiscordRPC::s_activated = false;

    void DiscordRPC::init(const char* applicationID) {
        rpc = std::async(DiscordRPC::threadLoop, applicationID);
        s_activated = true;
    }
    void DiscordRPC::shutdown(){
        s_running = false;
        if (s_activated) {
            rpc.wait();
            Discord_Shutdown();
            s_activated = false;
        }
    }
    void DiscordRPC::create(const char* applicationID) {
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        handlers.ready = handleDiscordReady;
        handlers.disconnected = handleDiscordDisconnected;
        handlers.errored = handleDiscordError;
        handlers.joinGame = handleDiscordJoin;
        handlers.spectateGame = handleDiscordSpectate;
        Discord_Initialize(applicationID, &handlers, 1, NULL);
    }

    void DiscordRPC::threadLoop(const char* applicationID) {
        int64_t startTime = time(0);
        int sendPresence = 1;
        s_running = true;
        create(applicationID);
        while (s_running) {
            
            if (sendPresence) {
                DiscordRichPresence discordPresence;
                memset(&discordPresence, 0, sizeof(discordPresence));
                discordPresence.state = s_state.c_str();
                discordPresence.details = s_details.c_str();
                discordPresence.startTimestamp = startTime;
                discordPresence.largeImageKey = s_largeImageKey.c_str();
                Discord_UpdatePresence(&discordPresence);
            }
            else {
                Discord_ClearPresence();
            }

#ifdef DISCORD_DISABLE_IO_THREAD
            Discord_UpdateConnection();
#endif
            Discord_RunCallbacks();
        }
    }
}