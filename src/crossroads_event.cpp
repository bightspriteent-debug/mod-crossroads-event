#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Creature.h"
#include "TemporarySummon.h"

namespace
{
    constexpr uint32 MAP_KALIMDOR = 1;
    constexpr uint32 ATTACKER_ENTRY = 3111;
    constexpr uint32 DESPAWN_MS = 10 * 60 * 1000;

    struct SpawnPoint
    {
        float x, y, z, o;
    };

    SpawnPoint SpawnPoints[] =
    {
        { -785.823f, -2836.455f, 91.666f, 0.0f },
        { -785.823f, -2836.455f, 91.666f, 0.0f },
        { -785.823f, -2836.455f, 91.666f, 0.0f },
    };

    void SpawnCrossroadsAttack(ChatHandler* handler)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Map* map = player->GetMap();

        if (!map)
        {
            handler->PSendSysMessage("Crossroads Attack: could not load map.");
            return;
        }

        uint32 spawned = 0;

        for (SpawnPoint const& point : SpawnPoints)
        {
            Creature* creature = player->SummonCreature(
                ATTACKER_ENTRY,
                point.x,
                point.y,
                point.z,
                point.o,
                TEMPSUMMON_TIMED_DESPAWN,
                DESPAWN_MS
            );

            if (creature)
                ++spawned;
        }

        handler->PSendSysMessage("Crossroads Attack: spawned %u attackers.", spawned);
    }
}

class crossroads_attack_commandscript : public CommandScript
{
public:
    crossroads_attack_commandscript() : CommandScript("crossroads_attack_commandscript") { }

   Acore::ChatCommands::ChatCommandTable GetCommands() const override
{
    using namespace Acore::ChatCommands;

    static ChatCommandTable startCommand =
    {
        ChatCommandBuilder("start", HandleStart, SEC_GAMEMASTER)
    };

    static ChatCommandTable attackCommand =
    {
        ChatCommandBuilder("attack", startCommand)
    };

    static ChatCommandTable rootCommand =
    {
        ChatCommandBuilder("crossroads", attackCommand)
    };

    return rootCommand;
}

    static bool HandleStart(ChatHandler* handler, Optional<std::string_view> /*args*/)
    {
        SpawnCrossroadsAttack(handler);
        return true;
    }
};

void AddCrossroadsEventScripts()
{
    new crossroads_attack_commandscript();
}