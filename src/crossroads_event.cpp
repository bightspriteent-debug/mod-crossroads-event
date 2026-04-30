#include "ScriptMgr.h"
#include "Chat.h"
#include "MapManager.h"
#include "Map.h"
#include "Creature.h"

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
        Map* map = sMapMgr->CreateBaseMap(MAP_KALIMDOR);

        if (!map)
        {
            handler->PSendSysMessage("Crossroads Attack: could not load map.");
            return;
        }

        uint32 spawned = 0;

        for (SpawnPoint const& point : SpawnPoints)
        {
            Creature* creature = map->SummonCreature(
                ATTACKER_ENTRY,
                point.x,
                point.y,
                point.z,
                point.o,
                nullptr,
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

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> startCommand =
        {
            { "start", SEC_GAMEMASTER, false, &HandleStart, "" }
        };

        static std::vector<ChatCommand> attackCommand =
        {
            { "attack", SEC_GAMEMASTER, false, nullptr, "", startCommand }
        };

        static std::vector<ChatCommand> rootCommand =
        {
            { "crossroads", SEC_GAMEMASTER, false, nullptr, "", attackCommand }
        };

        return rootCommand;
    }

    static bool HandleStart(ChatHandler* handler, char const*)
    {
        SpawnCrossroadsAttack(handler);
        return true;
    }
};

void AddCrossroadsAttackScripts()
{
    new crossroads_attack_commandscript();
}