#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Creature.h"
#include "TemporarySummon.h"
#include "CreatureAI.h"
#include "MotionMaster.h"
#include "Unit.h"
#include "ObjectAccessor.h"
#include "World.h"

namespace
{
    constexpr uint32 MAP_KALIMDOR = 1;
    constexpr uint32 ATTACKER_ENTRY = 3456;
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

    struct TargetPoint
    {
    float x, y, z;
    };

    TargetPoint AttackPoints[] =
    {
    { -482.723f, -2735.0f, 92.0f },
    { -478.0f, -2733.0f, 92.0f },
    { -474.0f, -2732.0f, 93.0f }
    };

    uint8 GetAverageBarrensPlayerLevel(ChatHandler* handler)
    {
    uint32 totalLevel = 0;
    uint32 playerCount = 0;

    HashMapHolder<Player>::MapType const& players = ObjectAccessor::GetPlayers();
    handler->PSendSysMessage("---- Barrens Players ----");

    for (auto const& pair : players)
    {
        Player* player = pair.second;

        if (!player || !player->IsInWorld())
            continue;

        if (player->IsGameMaster())
            continue;

        if (player->GetZoneId() != ZONE_BARRENS)
            continue;

        uint8 level = player->GetLevel();    
        std::string msg = "Player: " + std::string(player->GetName()) + " (Level " + std::to_string(level) + ")";

        handler->SendSysMessage(msg.c_str());

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
   
    std::string msg = "Players average level: " + std::to_string(averageLevel);
    handler->SendSysMessage(msg.c_str());
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
            uint8 eventLevel = GetAverageBarrensPlayerLevel(handler);
            std::string msg = "Crossroads Attack: event level" + std::to_string(eventLevel);
            handler->SendSysMessage(msg.c_str());
            if (creature)
                creature->SetLevel(eventLevel);
                uint32 health = 80 + (eventLevel * 35);
                creature->SetMaxHealth(health);
                creature->SetHealth(health);
                creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, eventLevel * 1.5f);
                creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, eventLevel * 2.5f);
                creature->UpdateDamagePhysical(BASE_ATTACK);
                uint32 index = spawned % (sizeof(AttackPoints) / sizeof(AttackPoints[0]));
                creature->SetReactState(REACT_AGGRESSIVE);
creature->SetWalk(false);

creature->SetHomePosition(
    AttackPoints[index].x,
    AttackPoints[index].y,
    AttackPoints[index].z,
    creature->GetOrientation()
);

creature->GetMotionMaster()->Clear();

creature->GetMotionMaster()->MovePoint(
    1,
    AttackPoints[index].x,
    AttackPoints[index].y,
    AttackPoints[index].z
);
                
                ++spawned;
        }
        // handler->PSendSysMessage("--------------------------");
        // handler->PSendSysMessage("Average Barrens Level: %u", averageLevel);
        
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