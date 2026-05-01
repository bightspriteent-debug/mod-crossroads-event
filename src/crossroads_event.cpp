#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Creature.h"
#include "TemporarySummon.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "World.h"

namespace
{
    constexpr uint32 MAP_KALIMDOR = 1;
    constexpr uint32 ATTACKER_ENTRY = 3111;
    constexpr uint32 DESPAWN_MS = 10 * 60 * 1000;
    constexpr uint32 ZONE_BARRENS = 17;
    constexpr uint8 DEFAULT_EVENT_LEVEL = 15;
    constexpr uint8 MIN_EVENT_LEVEL = 8;
    constexpr uint8 MAX_EVENT_LEVEL = 25;

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

    uint8 GetAverageBarrensPlayerLevel()
    {
    uint32 totalLevel = 0;
    uint32 playerCount = 0;

    HashMapHolder<Player>::MapType const& players = ObjectAccessor::GetPlayers();

    for (auto const& pair : players)
    {
        Player* player = pair.second;

        if (!player || !player->IsInWorld())
            continue;

        if (player->IsGameMaster())
            continue;

        if (player->GetZoneId() != ZONE_BARRENS)
            continue;

        totalLevel += player->GetLevel();
        ++playerCount;
    }

    if (playerCount == 0)
        return DEFAULT_EVENT_LEVEL;

    uint8 averageLevel = totalLevel / playerCount;

    if (averageLevel < MIN_EVENT_LEVEL)
        averageLevel = MIN_EVENT_LEVEL;

    if (averageLevel > MAX_EVENT_LEVEL)
        averageLevel = MAX_EVENT_LEVEL;

    return averageLevel;
}

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
            uint8 eventLevel = GetAverageBarrensPlayerLevel();
            if (creature)
                creature->SetLevel(eventLevel);
                creature->SetDefaultMovementType(RANDOM_MOTION_TYPE);
                creature->GetMotionMaster()->Initialize();
                ++spawned;
        }

        handler->PSendSysMessage("Crossroads Attack: spawned %u attackers.", spawned);
    }
}
using namespace Acore::ChatCommands;

class crossroads_attack_commandscript : public CommandScript
{
public:
    crossroads_attack_commandscript() : CommandScript("crossroads_attack_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable startCommand =
        {
            { "start", HandleStart, SEC_GAMEMASTER, Console::No }
        };

        static ChatCommandTable attackCommand =
        {
            { "attack", startCommand }
        };

        static ChatCommandTable rootCommand =
        {
            { "crossroads", attackCommand }
        };

        return rootCommand;
    }

    static bool HandleStart(ChatHandler* handler)
    {
        SpawnCrossroadsAttack(handler);
        return true;
    }
};

void AddCrossroadsEventScripts()
{
    new crossroads_attack_commandscript();
}