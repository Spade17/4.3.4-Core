/* ScriptData
SDName: boss_hagara_the_stormbinder
SD%Complete: ~90%
SDComment:
SDCategory: Boss Hagara
EndScriptData
*/

#include "ScriptPCH.h"
#include "Vehicle.h"
#include "dragon_soul.h"

uint32 NpcKillCount = 1;

enum Events
{
	EVENT_ZONE_COMBAT = 1,
	EVENT_ZONE_INCOMBAT_FOR_PHASE_1,
	// Event Phase 1
	EVENT_ATTACK_1,
	EVENT_ATTACK_2,
	EVENT_ATTACK_3,
	EVENT_ATTACK_FINAL,
	EVENT_MOVE,

	// Event Phase 2
	EVENT_ICE_SPEAR,
	EVENT_ICE_TOMB, // No Work
	EVENT_SHATTERED_ICE,
	EVENT_FOCUSED_ASSAULT, // No Work

	// Event Phase 3
	EVENT_DAMAGE_PHASE3_1,
	EVENT_DAMAGE_PHASE3_3,
	EVENT_REMOVE_PHASE_3,
	HAS_ICE_SPEAR_DEATH,

	// Event Phase 4
	EVENT_DAMAGE_PHASE4_1,
	EVENT_DAMAGE_PHASE4_2,
	EVENT_SUMMON_CRYSTAL,
	EVENT_DAMAGE_PHASE4_4,
	EVENT_REMOVE_PHASE_4,
	HAS_ARRESTER_RELOAD,
	EVENT_STORM_PILLAR,

	EVENT_CAST_CRYSTAL,
	EVENT_DESTROY_CRISTAL,

	EVENT_PHASE_1,
	EVENT_PHASE_2_START,
	EVENT_PHASE_2,
	EVENT_PHASE_3,
	EVENT_PHASE_4,
};

enum Spells
{
	SPELL_FEEDBACK = 108934,
	SPELL_FOCUSED_ASSAULT = 107851,
	SPELL_FROSTFLAKE = 109325,
	SPELL_FROZEN_TEMPEST = 105256,
	SPELL_ICE_TOMB = 104448,
	SPELL_LIGHTNING_CONDUIT = 105369,
	SPELL_LIGHTNING_STORM = 105465,
	SPELL_SHATTERED_ICE = 105289,
	SPELL_STORM_PILLAR = 109541,
	SPELL_WATER_SHIELD = 105409,
	SPELL_ICE_SHIELD = 105256,
	SPELL_ICE_SPEAR = 105297,
	SPELL_ICE_WAVE = 105265,
	SPELL_ICICLE = 109315,

	SPELL_AURA_CRYSTAL = 105482,
};

enum Auras
{
	AURA_WATER_SHIELD_10_N = 105409,
	AURA_WATER_SHIELD_25_N = 109560,
	AURA_WATER_SHIELD_10_H = 109561,
	AURA_WATER_SHIELD_25_H = 109562,
	AURA_ICE_SHIELD_10_N = 105256,
	AURA_ICE_SHIELD_25_N = 109552,
	AURA_ICE_SHIELD_10_H = 109553,
	AURA_ICE_SHIELD_25_H = 109554,
};

enum Yells
{
	YELL_ADDS_1 = 0,    // sound 26223
	YELL_ADDS_2,        // sound 26224
	YELL_ADDS_3,        // sound 26225
	YELL_ADDS_4,        // sound 26226
	YELL_INTRO,         // sound 26251
	YELL_AGGRO,         // sound 26227
	YELL_DEATH,         // sound 26243
	YELL_FROSTRAY,      // sound 26244, 26245, 26246
	YELL_GLACIER,       // sound 26247, 26248
	YELL_LIGHTNING,     // sound 26252, 26253
	YELL_CIRCUIT,       // sound 26228-26234
	YELL_CRYSTAL_DEAD,  // sound 26235-26241
	YELL_CRYSTALHIT,    // sound 26242
	YELL_SLAY           // sound 26254-26257
};

enum Spells_other
{
	SPELL_CRYSTALLINE_TETHER = 105312
};

Position const IceWave[20] =
{
	{13588.018f, 13561.425f, 124.478f, 0.0f}, // Ice Wave 1
	{13537.824f, 13612.145f, 124.48f, 0.0f}, // Ice Wave 1
	{13587.688f, 13662.458f, 124.481f, 0.0f}, // Ice Wave 1
	{13637.765f, 13612.426f, 124.48f, 0.0f}, // Ice Wave 1

	{13587.025f, 13567.962f, 123.483f, 0.0f}, // Ice Wave 2
	{13542.291f, 13612.294f, 123.483f, 0.0f}, // Ice Wave 2
	{13587.239f, 13656.026f, 123.484f, 0.0f}, // Ice Wave 2
	{13631.982f, 13612.078f, 123.484f, 0.0f}, // Ice Wave 2

	{13587.458f, 13570.935f, 123.484f, 0.0f}, // Ice Wave 3
	{13546.158f, 13612.783f, 123.484f, 0.0f}, // Ice Wave 3
	{13587.568f, 13652.82f, 123.483f, 0.0f}, // Ice Wave 3
	{13628.407f, 13611.911f, 123.483f, 0.0f}, // Ice Wave 3

	{13586.944f, 13576.984f, 123.484f, 0.0f},
	{13549.333f, 13611.454f, 123.484f, 0.0f},
	{13588.519f, 13649.022f, 123.484f, 0.0f},
	{13625.485f, 13611.896f, 123.484f, 0.0f},

	{13587.355f, 13578.86f, 123.484f, 0.0f},
	{13554.788f, 13611.742f, 123.484f, 0.0f},
	{13587.404f, 13646.427f, 123.484f, 0.0f},
	{13621.064f, 13612.353f, 123.484f, 0.0f}
};

Position const Icicle[1] =
{
	{13585.291f, 13610.428f, 122.420f, 0.0f}
};

Position const FrozenBindingCrystal[4] =
{
	{13617.5f, 13580.9f, 123.567f, 0.0f},
	{13557.4f, 13643.1f, 123.567f, 0.0f},
	{13557.7f, 13580.7f, 123.567f, 0.0f},
	{13617.3f, 13643.5f, 123.567f, 0.0f}
};

Position const BoundLightningElemental[4] =
{
	{13587.624f, 13653.375f, 123.483f, 0.0f},
	{13621.863f, 13621.847f, 123.483f, 0.0f},
	{13623.250f, 13606.621f, 123.483f, 0.0f},
	{13587.191f, 13570.221f, 123.483f, 0.0f}
};

Position const CrystalConductor[4] =
{
	{13617.5f, 13580.9f, 123.567f, 0.0f},
	{13557.4f, 13643.1f, 123.567f, 0.0f},
	{13557.7f, 13580.7f, 123.567f, 0.0f},
	{13617.3f, 13643.5f, 123.567f, 0.0f}
};

Position const Wave1[3] =
{
	{13555.907f, 13612.398f, 123.483f, 0.039f},
	{13551.798f, 13608.053f, 123.483f, 0.039f},
	{13550.384f, 13615.547f, 123.483f, 0.106f}
};

Position const Wave2[11] =
{
	{13558.0f, 13642.299f, 123.567f, 5.585f},
	{13557.4f, 13639.7f, 123.567f, 5.585f},
	{13559.7f, 13639.099f, 123.567f, 5.585f},
	{13561.099f, 13641.9f, 123.57f, 5.585f},
	{13557.7f, 13583.5f, 123.567f, 0.820f},
	{13559.2f, 13585.799f, 123.567f, 0.820f},
	{13560.2f, 13583.299f, 123.567f, 0.820f},
	{13561.9f, 13581.299f, 123.567f, 0.820f},
	{13562.099f, 13584.4f, 123.567f, 0.820f},

	{13557.9f, 13581.2f, 123.483f, 0.785f},
	{13557.9f, 13643.5f, 123.567f, 5.550f}
};

Position const Wave3[6] =
{
	{13585.0f, 13651.9f, 123.526f, 4.643f},
	{13590.0f, 13571.2f, 123.542f, 1.345f},
	{13587.4f, 13576.2f, 123.484f, 1.608f},
	{13588.0f, 13652.0f, 123.514f, 4.667f},
	{13584.099f, 13570.799f, 123.545f, 1.739f},
	{13591.099f, 13651.799f, 123.528f, 4.867f}
};

Position const Wave4[11] =
{
	{13569.3f, 13616.1f, 122.503f, 1.69297f},
	{13569.7f, 13612.3f, 122.503f, 5.84685f},
	{13570.0f, 13608.3f, 122.503f, 2.32129f},
	{13566.2f, 13614.1f, 122.503f, 3.38594f},
	{13573.0f, 13615.2f, 122.503f, 0.942478f},
	{13573.8f, 13610.6f, 122.503f, 0.0872665f},
	{13572.9f, 13605.8f, 122.503f, 5.68977f},
	{13566.5f, 13609.9f, 122.503f, 3.08923f},
	{13567.2f, 13606.2f, 122.503f, 4.01426f},
	{13570.5f, 13602.9f, 122.503f, 5.13127f},

	{13578.2f, 13610.9f, 122.419f, 6.241f}
};

class boss_hagara : public CreatureScript
{
public:
    boss_hagara() : CreatureScript("boss_hagara") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDragonSoulAI<boss_hagaraAI>(creature);
    }

    struct boss_hagaraAI: public BossAI
    {
        boss_hagaraAI(Creature* creature) : BossAI(creature, BOSS_HAGARA)
        {
            instance = creature->GetInstanceScript();
			aura_is = RAID_MODE(AURA_ICE_SHIELD_10_N, AURA_ICE_SHIELD_25_N, AURA_ICE_SHIELD_10_H, AURA_ICE_SHIELD_25_H);
            aura_ws = RAID_MODE(AURA_WATER_SHIELD_10_N, AURA_WATER_SHIELD_25_N, AURA_WATER_SHIELD_10_H, AURA_WATER_SHIELD_25_H);
        }

        InstanceScript* instance;
        EventMap events;
		int8 count;
		bool action, attack1, attack2, attack3, attackfinal;
		bool phase1, phase2, phase3, phase4;
		bool special;
		uint32 aura_is, aura_ws;

        void Reset()
        {
			_Reset();
			action = true;
			phase1 = false;
			phase2 = false;
			phase3 = false;
			phase4 = false;
			special = false;
        }

		void DoAction(int32 actions)
		{
            switch (actions)
            {
                case 1:
                {
					if (action)
					{
						action = false;
						me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
						attack1 = false;
						attack2 = false;
						attack3 = false;
						attackfinal = false;
						phase1 = true;
						events.ScheduleEvent(EVENT_PHASE_1, 100);
						events.ScheduleEvent(EVENT_ZONE_INCOMBAT_FOR_PHASE_1, 1);
						break;
					}
                }
            }
        }

		// Send packet to all players in The Frozen Throne
        void SendPacketToPlayers(WorldPacket const* data) const
        {
		    Map::PlayerList const& players = me->GetMap()->GetPlayers();
		    if (!players.isEmpty())
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (Player* player = itr->getSource())
                        if (player->GetAreaId() == 5920)
                            player->GetSession()->SendPacket(data);
        }

		void SendWeather(WeatherState weather) const
        {
		    WorldPacket data(SMSG_WEATHER, 9);
		    data << uint32(weather);
		    data << float(0.5f);
		    data << uint8(0);
		    SendPacketToPlayers(&data);
        }

		void SetPlayersInCombat(bool evade)
		{
		    uint8 players = 0;
		    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
		    if (!PlayerList.isEmpty())
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (Player *player = i->getSource())
						if (player->isAlive() && !player->isGameMaster() && player->GetDistance(me) <= 150.0f)
						{
							++players;
							player->SetInCombatWith(me->ToUnit());
						}

			if (players == 0 && evade == true)
				EnterEvadeMode();
	    }

		void GetPlayersInCombat()
		{
			uint8 players = 0;
		    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
		    if (!PlayerList.isEmpty())
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (Player* player = i->getSource())
						if (player->isAlive() && !player->isGameMaster())
							++players;

			if (players == 0)
				EnterEvadeMode();
		}

		void DespawnCreatures(uint32 entry)
		{
			std::list<Creature*> creatures;
			GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

			if (creatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

        void EnterCombat(Unit* /*who*/)
        {
            _EnterCombat();
			action = false;
			phase1 = false;
			phase2 = true;
			phase3 = false;
			phase4 = false;
			special = false;
			Talk(YELL_AGGRO);
            events.ScheduleEvent(EVENT_PHASE_2, 1);
			events.ScheduleEvent(EVENT_ZONE_COMBAT, 100);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add
			instance->SetData(DATA_PORTALS_ON_OFF, IN_PROGRESS);
        }

        void UpdateAI(uint32 diff)
        {
            events.Update(diff);

			if (phase1)
			{
				if (attack1)
				{
					if (instance->GetData64(DATA64_KILL_COUNT) >= 3)
					{
					    attack1 = false;
					    instance->SetData64(DATA64_KILL_COUNT, 0);
					    events.ScheduleEvent(EVENT_ATTACK_2, 1);
					}		
				}

				if (attack2)
				{
					if (instance->GetData64(DATA64_KILL_COUNT) >= 11)
					{
					    attack2 = false;
					    instance->SetData64(DATA64_KILL_COUNT, 0);
					    events.ScheduleEvent(EVENT_ATTACK_3, 1);
					}
				}

				if (attack3)
				{
					if (instance->GetData64(DATA64_KILL_COUNT) >= 6)
					{
					    attack3 = false;
					    instance->SetData64(DATA64_KILL_COUNT, 0);
					    events.ScheduleEvent(EVENT_ATTACK_FINAL, 1);
					}
				}

				if (attackfinal)
				{
					if (instance->GetData64(DATA64_KILL_COUNT) >= 11)
					{
					    attackfinal = false;
					    instance->SetData64(DATA64_KILL_COUNT, 0);
					    events.ScheduleEvent(EVENT_MOVE, 2000);
					}
				}
			}

			if (phase3)
			{
				if (!me->HasAura(aura_is))
				{
					if (me->HasUnitState(UNIT_STATE_CASTING))
						return;

					instance->DoRemoveAurasDueToSpellOnPlayers(110317);
					phase3 = false;
					events.CancelEvent(EVENT_PHASE_3);
                    events.CancelEvent(EVENT_DAMAGE_PHASE3_3);
                    events.ScheduleEvent(EVENT_PHASE_2, 1);
					events.ScheduleEvent(EVENT_PHASE_4, 62000);
					DespawnCreatures(NPC_FORZEN_BINDIG_CRYSTAL);
					DespawnCreatures(NPC_HAGARA_CASTER);
					SendWeather(WEATHER_STATE_FINE);
				}

				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (Player* player = i->getSource())
				    {
						if (me->HasAura(aura_is))
						{
						    if (!player->HasAura(110317) && player->GetDistance(me) <= 25.0f)
								player->CastSpell(player, 110317);
						    else
							    if (player->HasAura(110317) && player->GetDistance(me) >= 26.0f)
							        player->RemoveAura(110317);
					    }
					}

				if (instance->GetData64(DATA64_KILL_COUNT) >= 4)
				{
					phase3 = false;
					instance->SetData64(DATA64_KILL_COUNT, 0);
					instance->DoRemoveAurasDueToSpellOnPlayers(110317);
					events.CancelEvent(EVENT_PHASE_3);
					events.CancelEvent(EVENT_REMOVE_PHASE_3);
					events.CancelEvent(EVENT_DAMAGE_PHASE3_3);
					events.CancelEvent(HAS_ICE_SPEAR_DEATH);
					events.ScheduleEvent(EVENT_PHASE_2, 1);
					events.ScheduleEvent(EVENT_PHASE_4, 62000);
					me->RemoveAurasDueToSpell(SPELL_ICE_SHIELD);
					DespawnCreatures(NPC_ICE_WAVE);
					DespawnCreatures(NPC_HAGARA_CASTER);
					SendWeather(WEATHER_STATE_FINE);
					DoCast(108934);
				}
			}

			if (phase4)
			{
				if (!me->HasAura(aura_ws))
				{
					if (me->HasUnitState(UNIT_STATE_CASTING))
						return;

					phase4 = false;
					events.CancelEvent(EVENT_PHASE_4);
					events.CancelEvent(EVENT_STORM_PILLAR);
                    events.ScheduleEvent(EVENT_PHASE_2, 1);
					events.ScheduleEvent(EVENT_PHASE_3, 62000);
					DespawnCreatures(NPC_CRYSTAL_CONDUCTOR);
					DespawnCreatures(NPC_BOUND_LIGHTNING_ELEMENTAL);
					DespawnCreatures(NPC_ICICLE);
					DespawnCreatures(NPC_HAGARA_CASTER);
					SendWeather(WEATHER_STATE_FINE);
				}

				if (instance->GetData64(DATA64_KILL_COUNT) >= 4)
				{
					phase4 = false;
					instance->SetData64(DATA64_KILL_COUNT, 0);
					events.CancelEvent(EVENT_PHASE_4);
					events.CancelEvent(EVENT_STORM_PILLAR);
					events.ScheduleEvent(EVENT_PHASE_2, 1);
					events.ScheduleEvent(EVENT_PHASE_3, 62000);
					DespawnCreatures(NPC_BOUND_LIGHTNING_ELEMENTAL);
					DespawnCreatures(NPC_ICICLE);
					DespawnCreatures(NPC_HAGARA_CASTER);
					me->RemoveAurasDueToSpell(SPELL_WATER_SHIELD);
					SendWeather(WEATHER_STATE_FINE);
					DoCast(108934);
				}
			}

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
				case EVENT_ZONE_INCOMBAT_FOR_PHASE_1:
					GetPlayersInCombat();
					events.ScheduleEvent(EVENT_ZONE_INCOMBAT_FOR_PHASE_1, 1000);
					break;

				case EVENT_ZONE_COMBAT:
					SetPlayersInCombat(true);
					events.CancelEvent(EVENT_ZONE_INCOMBAT_FOR_PHASE_1);
					events.ScheduleEvent(EVENT_ZONE_COMBAT, 1000);
					break;
				// Phase 1 (event start)
				case EVENT_PHASE_1:
					Talk(YELL_ADDS_1);
					events.ScheduleEvent(EVENT_ATTACK_1, 2000);
					break;

				case EVENT_ATTACK_1:
					me->SummonCreature(NPC_STORMBORN_MYRMIDON, Wave1[0], TEMPSUMMON_MANUAL_DESPAWN);
					me->SummonCreature(NPC_STORMBINDER_ADEPT, Wave1[1], TEMPSUMMON_MANUAL_DESPAWN);
					me->SummonCreature(NPC_TWILIGHT_FROST_EVOKER, Wave1[2], TEMPSUMMON_MANUAL_DESPAWN);
					attack1 = true;
					break;

				case EVENT_ATTACK_2:
					Talk(YELL_ADDS_2);
					for (int8 i = 0; i < 9; i++)
					    me->SummonCreature(NPC_CORRUPTED_FRAGMENT, Wave2[i], TEMPSUMMON_MANUAL_DESPAWN);

				    for (int8 i = 9; i < 11; i++)
					    me->SummonCreature(NPC_STORMBINDER_ADEPT, Wave2[i], TEMPSUMMON_MANUAL_DESPAWN);

					attack2 = true;
					break;

				case EVENT_ATTACK_3:
					Talk(YELL_ADDS_3);
					for (int8 i = 0; i < 2; i++)
					    me->SummonCreature(NPC_TWILIGHT_FROST_EVOKER, Wave3[i], TEMPSUMMON_MANUAL_DESPAWN);
					for (int8 i = 2; i < 4; i++)
					    me->SummonCreature(NPC_STORMBINDER_ADEPT, Wave3[i], TEMPSUMMON_MANUAL_DESPAWN);
					for (int8 i = 4; i < 6; i++)
					    me->SummonCreature(NPC_STORMBORN_MYRMIDON, Wave3[i], TEMPSUMMON_MANUAL_DESPAWN);

					attack3 = true;
					break;

				case EVENT_ATTACK_FINAL:
					Talk(YELL_ADDS_4);
					for (int8 i = 0; i < 10; i++)
					    me->SummonCreature(NPC_CORRUPTED_FRAGMENT, Wave4[i], TEMPSUMMON_MANUAL_DESPAWN);
					me->SummonCreature(NPC_LIEUTENANT_SHARA, Wave4[10], TEMPSUMMON_MANUAL_DESPAWN);

					attackfinal = true;
					break;

				case EVENT_MOVE:
					Talk(YELL_INTRO);
					GetDisk()->GetMotionMaster()->MovePoint(0, 13587.4f, 13612.0f, 122.419f);
					events.ScheduleEvent(EVENT_PHASE_2_START, 10000);
					break;

				case EVENT_PHASE_2_START:
					instance->SetData(DATA_PORTALS_ON_OFF, DONE);
				    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
					me->ExitVehicle();
					GetDisk()->DespawnOrUnsummon();
					phase1 = false;
					break;

                // Phase 2
                case EVENT_PHASE_2:
					phase2 = true;
					if (!special)
					{   
					    special = true;
                        events.ScheduleEvent(EVENT_ICE_SPEAR, 10000);
						events.ScheduleEvent(EVENT_FOCUSED_ASSAULT, 4000);
						events.ScheduleEvent(urand(EVENT_PHASE_3, EVENT_PHASE_4), 30000);
					}
					else
					{
					    events.ScheduleEvent(EVENT_ICE_SPEAR, 12000);
					    events.ScheduleEvent(EVENT_FOCUSED_ASSAULT, 15500);
						events.ScheduleEvent(EVENT_SHATTERED_ICE, urand(10500, 15000));
					}
                    break;

				case EVENT_FOCUSED_ASSAULT:
					if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
						DoCast(target, SPELL_FOCUSED_ASSAULT);
					events.ScheduleEvent(EVENT_FOCUSED_ASSAULT, 15500);
					break;

                case EVENT_ICE_SPEAR:
					DoCast(SPELL_ICE_SPEAR);
					Talk(YELL_FROSTRAY);
					if (special)
					    events.ScheduleEvent(EVENT_ICE_SPEAR, 30000);
                    break;

                case EVENT_SHATTERED_ICE:
                    if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        DoCast(target, SPELL_SHATTERED_ICE);
                    events.ScheduleEvent(EVENT_SHATTERED_ICE, urand(10500, 15000));
                    break;

			    // Phase 3
                case EVENT_PHASE_3:
					phase2 = false;
					phase3 = true;
                    events.CancelEvent(EVENT_PHASE_2);
                    events.CancelEvent(EVENT_ICE_SPEAR);
                    events.CancelEvent(EVENT_SHATTERED_ICE);
                    events.ScheduleEvent(EVENT_DAMAGE_PHASE3_1, 1);
					events.ScheduleEvent(EVENT_DAMAGE_PHASE3_3, 5000);
					Talk(YELL_GLACIER);
					me->NearTeleportTo(13587.4f, 13612.0f, 122.419f, 0.0f, true);
					me->SummonCreature(NPC_HAGARA_CASTER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
					SendWeather(WEATHER_STATE_HEAVY_SNOW);
                    DoCast(SPELL_ICE_SHIELD);
                    break;

                case EVENT_DAMAGE_PHASE3_1:
				    me->SummonCreature(NPC_ICICLE, Icicle[1], TEMPSUMMON_CORPSE_DESPAWN, 340000);
				    for (int8 i = 0; i < 20; i++)
                        me->SummonCreature(NPC_ICE_WAVE, IceWave[i], TEMPSUMMON_CORPSE_DESPAWN, 340000);

					/*for (int8 i = 4; i < 8; i++)
                        me->SummonCreature(56105, IceWave[i], TEMPSUMMON_CORPSE_DESPAWN, 340000);

					for (int8 i = 8; i < 12; i++)
                        me->SummonCreature(56105, IceWave[i], TEMPSUMMON_CORPSE_DESPAWN, 340000);

					for (int8 i = 12; i < 16; i++)
                        me->SummonCreature(56105, IceWave[i], TEMPSUMMON_CORPSE_DESPAWN, 340000);

					for (int8 i = 16; i < 20; i++)
                        me->SummonCreature(56105, IceWave[i], TEMPSUMMON_CORPSE_DESPAWN, 340000);*/
						
					for (int8 i = 0; i < 4; i++)
                        me->SummonCreature(NPC_FORZEN_BINDIG_CRYSTAL, FrozenBindingCrystal[i], TEMPSUMMON_MANUAL_DESPAWN);
					break;

                case EVENT_DAMAGE_PHASE3_3:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        me2()->CastSpell(target, SPELL_FROSTFLAKE, true);
                    events.ScheduleEvent(EVENT_DAMAGE_PHASE3_3, 5000);
                    break;

                // Phase 4
                case EVENT_PHASE_4:
					phase2 = false;
					phase4 = true;
                    events.CancelEvent(EVENT_ICE_SPEAR);
                    events.CancelEvent(EVENT_SHATTERED_ICE);
					events.ScheduleEvent(EVENT_SUMMON_CRYSTAL, 1);
					if (IsHeroic())
					    events.ScheduleEvent(EVENT_STORM_PILLAR, 5000);
					Talk(YELL_LIGHTNING);
					me->NearTeleportTo(13587.4f, 13612.0f, 122.419f, 0.0f, true);
					me->SummonCreature(NPC_HAGARA_CASTER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
					SendWeather(WEATHER_STATE_HEAVY_RAIN);
                    DoCast(SPELL_WATER_SHIELD);
                    break;

				case EVENT_STORM_PILLAR:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					    me2()->AI()->DoCast(target, SPELL_STORM_PILLAR, true);

					events.ScheduleEvent(EVENT_STORM_PILLAR, 5000);
					break;

                case EVENT_SUMMON_CRYSTAL:
				    for (int8 i = 0; i < 4; i++)
                        me->SummonCreature(NPC_CRYSTAL_CONDUCTOR, CrystalConductor[i], TEMPSUMMON_MANUAL_DESPAWN);
                    break;
                default:
                    break;
                }
            }		

			if (phase2)
                DoMeleeAttackIfReady();
        }

		void EnterEvadeMode()
		{
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SetData64(DATA64_KILL_COUNT, 0);
			instance->DoRemoveAurasDueToSpellOnPlayers(110317);
			// Despawn Creatures
			DespawnCreatures(NPC_STORMBORN_MYRMIDON);
			DespawnCreatures(NPC_STORMBINDER_ADEPT);
			DespawnCreatures(NPC_TWILIGHT_FROST_EVOKER);
			DespawnCreatures(NPC_CORRUPTED_FRAGMENT);
			DespawnCreatures(NPC_LIEUTENANT_SHARA);
			DespawnCreatures(NPC_FORZEN_BINDIG_CRYSTAL);
			DespawnCreatures(NPC_CRYSTAL_CONDUCTOR);
			DespawnCreatures(NPC_BOUND_LIGHTNING_ELEMENTAL);
			DespawnCreatures(NPC_ICE_WAVE);
			DespawnCreatures(NPC_ICICLE);
			DespawnCreatures(NPC_TWILIGHT_ELITE_SLAYER);
			DespawnCreatures(NPC_TWILIGHT_ELITE_DREADBLADE);
			DespawnCreatures(NPC_HAGARA_DISK);
			DespawnCreatures(NPC_HAGARA_CASTER);
			SendWeather(WEATHER_STATE_FINE);
			me->DespawnOrUnsummon();
			
			_EnterEvadeMode();
		}

		void KilledUnit(Unit* victim)
		{
            Talk(YELL_SLAY);
		}

        void JustDied(Unit* /*killer*/)
        {
			Talk(YELL_DEATH); // Death
            instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SetData64(DATA64_KILL_COUNT, 0);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			instance->DoRemoveAurasDueToSpellOnPlayers(110317);
			// Despawn Creatures
			DespawnCreatures(NPC_STORMBORN_MYRMIDON);
			DespawnCreatures(NPC_STORMBINDER_ADEPT);
			DespawnCreatures(NPC_TWILIGHT_FROST_EVOKER);
			DespawnCreatures(NPC_CORRUPTED_FRAGMENT);
			DespawnCreatures(NPC_LIEUTENANT_SHARA);
			DespawnCreatures(NPC_FORZEN_BINDIG_CRYSTAL);
			DespawnCreatures(NPC_CRYSTAL_CONDUCTOR);
			DespawnCreatures(NPC_BOUND_LIGHTNING_ELEMENTAL);
			DespawnCreatures(NPC_ICE_WAVE);
			DespawnCreatures(NPC_ICICLE);
			DespawnCreatures(NPC_TWILIGHT_ELITE_SLAYER);
			DespawnCreatures(NPC_TWILIGHT_ELITE_DREADBLADE);
			DespawnCreatures(NPC_HAGARA_DISK);
			DespawnCreatures(NPC_HAGARA_CASTER);
			//
			SendWeather(WEATHER_STATE_FINE);
            _JustDied();
        }

		Creature* GetDisk()
        {
			return me->FindNearestCreature(NPC_HAGARA_DISK, 100.0f, true);
        }
		
		Creature* me2()
		{
			return me->FindNearestCreature(NPC_HAGARA_CASTER, 100.0f, true);
		}
    };
};

class npc_ice_wave : public CreatureScript
{
    public:
        npc_ice_wave() : CreatureScript("npc_ice_wave") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_ice_waveAI>(creature);
		}

		struct npc_ice_waveAI : public ScriptedAI
		{
		    npc_ice_waveAI(Creature* creature) : ScriptedAI(creature)
		    {
			    //instance = creature->GetInstanceScript();
		    }

			//InstanceScript* instance;
			//EventMap events;

			void Reset()
			{
				me->AddAura(105265, me);
			}
		};
};

// Crystal Conductor
class npc_crystal_conductor : public CreatureScript
{
    public:
        npc_crystal_conductor() : CreatureScript("npc_crystal_conductor") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_crystal_conductorAI(creature);
		}

		struct npc_crystal_conductorAI : public ScriptedAI
		{
		    npc_crystal_conductorAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			Creature* hagara;
			EventMap events;
			bool AttackStart;

			void Reset()
			{
				if (hagara = me->FindNearestCreature(NPC_HAGARA, 100.0f, true))
					DoCast(hagara, 105482, true);

				AttackStart = false;
			}

			void DamageTaken(Unit* doneBy, uint32& damage)
			{
				if (!AttackStart)
				{
					AttackStart = true;
					damage = 0;
					me->SetHealth(me->GetMaxHealth());
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SummonCreature(NPC_BOUND_LIGHTNING_ELEMENTAL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(),  TEMPSUMMON_MANUAL_DESPAWN);
					events.ScheduleEvent(1, urand(15000, 25000));
				}
				if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
					damage = 0;
			}

			void UpdateAI(uint32 diff)
			{
				events.Update(diff);
				while (uint32 eventId = events.ExecuteEvent())
				{
                    switch (eventId)
                    {
					case 1:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						break;
					}
				}

				/*Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (Player* player = i->getSource())
				    {
						if (player->GetDistance(me) <= 5.0f)
							player->AddAura(105369, player);
						else
							if (player->HasAura(105369))
							    player->RemoveAura(105369);
					}*/
			}

			void JustDied(Unit* /*killer*/)
			{
				DoCast(SPELL_CRYSTALLINE_TETHER);
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);

				if (hagara = me->FindNearestCreature(NPC_HAGARA, 100.0f, true))
					hagara->AI()->Talk(YELL_CIRCUIT);

				me->DespawnOrUnsummon(2000);
			}
		};
};

// Bound Lightning Elemental
class npc_bound_lightning_elemental : public CreatureScript
{
    public:
        npc_bound_lightning_elemental() : CreatureScript("npc_bound_lightning_elemental") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_bound_lightning_elementalAI(creature);
		}

		struct npc_bound_lightning_elementalAI : public ScriptedAI
		{
		    npc_bound_lightning_elementalAI(Creature* creature) : ScriptedAI(creature)
		    {
		    }

			void UpdateAI(uint32 diff)
			{
				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				//if (Creature* crystal = me->FindNearestCreature(NPC_CRYSTAL_CONDUCTOR, 50.0f, true))
				//	crystal->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				me->DespawnOrUnsummon(5000);
			}
		};
};

// Frozen Binding Crystal
class npc_frozen_binding_crystal : public CreatureScript
{
    public:
        npc_frozen_binding_crystal() : CreatureScript("npc_frozen_binding_crystal") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_frozen_binding_crystalAI(creature);
		}

		struct npc_frozen_binding_crystalAI : public ScriptedAI
		{
		    npc_frozen_binding_crystalAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			Creature* hagara;
			bool AttackStart;

			void Reset()
			{
				if (hagara = me->FindNearestCreature(NPC_HAGARA, 100.0f, true))
					DoCast(hagara, 105482, true);

				AttackStart = false;
			}

			void DamageTaken(Unit* doneBy, uint32& damage)
			{
				if (!AttackStart)
				{
					AttackStart = true;
					if (hagara = me->FindNearestCreature(NPC_HAGARA, 100.0f, true))
					    hagara->AI()->Talk(YELL_CRYSTALHIT);
				}
			}

			void JustDied(Unit* /*killer*/)
			{
				DoCast(SPELL_CRYSTALLINE_TETHER);
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);

				if (hagara = me->FindNearestCreature(NPC_HAGARA, 100.0f, true))
					hagara->AI()->Talk(YELL_CRYSTAL_DEAD);

				me->DespawnOrUnsummon(5000);
			}
		};
};

enum SpellsSM
{
	SPELL_SPARK = 109368,
	SPELL_CHAIN_LIGHTNING = 109348
};

enum EventsSM
{
	EVENT_SPARK = 1,
	EVENT_CL
};

class npc_stormborn_myrmidon : public CreatureScript
{
    public:
        npc_stormborn_myrmidon() : CreatureScript("npc_stormborn_myrmidon") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_stormborn_myrmidonAI>(creature);
		}

		struct npc_stormborn_myrmidonAI : public ScriptedAI
		{
		    npc_stormborn_myrmidonAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			void Reset()
			{
				me->SetInCombatWithZone();
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(EVENT_SPARK, urand(1500, 3000));
				events.ScheduleEvent(EVENT_CL, urand(5000, 10000));
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
				    return;

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_SPARK:
					     DoCastVictim(SPELL_SPARK);
						 events.ScheduleEvent(EVENT_SPARK, urand(5000, 10000));
						 break;
					case EVENT_CL:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_CHAIN_LIGHTNING);
						events.ScheduleEvent(EVENT_CL, urand(5000, 15000));
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				me->DespawnOrUnsummon(60000);
			}
		};
};

class npc_stormbinder_adept : public CreatureScript
{
    public:
        npc_stormbinder_adept() : CreatureScript("npc_stormbinder_adept") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_stormbinder_adeptAI>(creature);
		}

		struct npc_stormbinder_adeptAI : public ScriptedAI
		{
		    npc_stormbinder_adeptAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			void Reset()
			{
				me->SetInCombatWithZone();
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(1, urand(2500,5000));
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim())
				    return;

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case 1:
						DoCast(109443);
						events.ScheduleEvent(1, 10000, 15000);
						break;
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				me->DespawnOrUnsummon(60000);
			}
		};
};

class npc_twilight_frost_evoker : public CreatureScript
{
    public:
        npc_twilight_frost_evoker() : CreatureScript("npc_twilight_frost_evoker") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_twilight_frost_evokerAI>(creature);
		}

		struct npc_twilight_frost_evokerAI : public ScriptedAI
		{
		    npc_twilight_frost_evokerAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			void Reset()
			{
				me->SetInCombatWithZone();
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(1, 1000);
				events.ScheduleEvent(2, urand(5000,10000));
				events.ScheduleEvent(3, urand(5000, 15000));
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
				    return;

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case 1:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, 109334);
						events.ScheduleEvent(1, urand(2500, 3500));
						break;
					case 2:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, 109423);
						events.ScheduleEvent(2, urand(5000, 15000));
						break;
					case 3:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, 109360);
						events.ScheduleEvent(3, urand(15000, 30000));
						break;
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				me->DespawnOrUnsummon(60000);
			}
		};
};

class npc_corrupted_fragment : public CreatureScript
{
    public:
        npc_corrupted_fragment() : CreatureScript("npc_corrupted_fragment") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_corrupted_fragmentAI>(creature);
		}

		struct npc_corrupted_fragmentAI : public ScriptedAI
		{
		    npc_corrupted_fragmentAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;

			void Reset()
			{
				me->SetInCombatWithZone();
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim())
				    return;

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				me->DespawnOrUnsummon(60000);
			}
		};
};

class npc_lieutenant_shara : public CreatureScript
{
    public:
        npc_lieutenant_shara() : CreatureScript("npc_lieutenant_shara") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_lieutenant_sharaAI>(creature);
		}

		struct npc_lieutenant_sharaAI : public ScriptedAI
		{
		    npc_lieutenant_sharaAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;
			bool damage;

			void Reset()
			{
				me->SetInCombatWithZone();
				damage = false;
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(1, urand(2500, 5000));
				events.ScheduleEvent(3, urand(10000, 15000));
				damage = false;
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
				    return;

				if (HealthBelowPct(25) && !damage)
				{
					events.ScheduleEvent(2, urand(3000, 5000));
					damage = true;
				}

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case 1:
						DoCastVictim(109331);
						events.ScheduleEvent(1, urand(3000, 8000));
						break;
					case 2:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						    DoCast(target, 109334);
						events.ScheduleEvent(2, urand(5000, 15000));
						break;
					case 3:
						DoCast(me, 109305);
						events.ScheduleEvent(3, urand(15000, 30000));
						break;
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				me->DespawnOrUnsummon(120000);
			}
		};
};

class npc_frost_grasp : public CreatureScript
{
    public:
        npc_frost_grasp() : CreatureScript("npc_frost_grasp") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_frost_graspAI(creature);
		}

		struct npc_frost_graspAI : public ScriptedAI
		{
		    npc_frost_graspAI(Creature* creature) : ScriptedAI(creature)
		    {
		    }

			void Reset()
			{
				me->SetInCombatWithZone();
				me->AddAura(109295, me);
			}

			void EnterCombat(Unit* /*who*/)
			{
				me->DespawnOrUnsummon(60000);
			}
		};
};

class npc_tornado_ds : public CreatureScript
{
    public:
        npc_tornado_ds() : CreatureScript("npc_tornado_ds") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_tornado_dsAI(creature);
		}

		struct npc_tornado_dsAI : public ScriptedAI
		{
		    npc_tornado_dsAI(Creature* creature) : ScriptedAI(creature)
		    {
		    }

			EventMap events;

			void Reset()
			{
				me->SetInCombatWithZone();
				DoCast(109440);
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(1, 1000);
			}

			void UpdateAI(uint32 diff)
			{
				if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
				    return;

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case 1:
						DoCastAOE(80064);
						events.ScheduleEvent(1, 1000);
						break;
					}
				}
			}
		};
};

class go_the_focusing_lris : public GameObjectScript
{
public:
    go_the_focusing_lris() : GameObjectScript("go_the_focusing_lris") { }

	Creature* disk;
	Creature* hagara;

	bool OnGossipHello(Player* player, GameObject* go)
	{
		disk = go->SummonCreature(NPC_HAGARA_DISK, 13550.0f, 13613.299f, 133.126f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
		hagara = go->SummonCreature(NPC_HAGARA, 13550.0f, 13613.299f, 133.126f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

		hagara->EnterVehicle(disk);
		hagara->AI()->DoAction(1);

		if (InstanceScript* instance = go->GetInstanceScript())
			instance->SetData(DATA_PORTALS_ON_OFF, IN_PROGRESS);

		go->SetPhaseMask(2, true);

		return false;
	}
};

// Ice Lance. 105287
class spell_il_ice_lance : public SpellScriptLoader
{
    public:
        spell_il_ice_lance() : SpellScriptLoader("spell_il_ice_lance") { }

        class spell_il_ice_lance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_il_ice_lance_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
				PreventHitDefaultEffect(effIndex);
				GetCaster()->CastSpell(GetHitPlayer(), uint32(GetEffectValue()), true);
				GetCaster()->CastSpell(GetHitPlayer(), 105285, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_il_ice_lance_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_il_ice_lance_SpellScript();
        }
};

class spell_summon_ice_spear : public SpellScriptLoader
{
    public:
        spell_summon_ice_spear() : SpellScriptLoader("spell_summon_ice_spear") { }

        class spell_summon_ice_spear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_ice_spear_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);

				int8 count = urand(1,4);
				for (int8 i = 0; i < count; i++)
                    GetCaster()->SummonCreature(entry, FrozenBindingCrystal[i], TEMPSUMMON_TIMED_DESPAWN, 15000);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_summon_ice_spear_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_summon_ice_spear_SpellScript();
        }
};

void AddSC_boss_hagara()
{
	new boss_hagara();
	new npc_stormborn_myrmidon();
	new npc_stormbinder_adept();
	new npc_twilight_frost_evoker();
	new npc_corrupted_fragment();
	new npc_lieutenant_shara();
	new npc_ice_wave();
	new npc_crystal_conductor();
	new go_the_focusing_lris();
	new npc_frost_grasp();
	new npc_tornado_ds();
	new spell_il_ice_lance();
	new npc_crystal_conductor();
	new npc_frozen_binding_crystal();
	new spell_summon_ice_spear();
}