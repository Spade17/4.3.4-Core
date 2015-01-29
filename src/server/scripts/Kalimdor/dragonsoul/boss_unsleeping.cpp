/* ScriptData
SDName: boss_unsleeping
SD%Complete: ~90%
SDComment:
SDCategory: Boss Unsleeping
EndScriptData
*/

#include "ScriptPCH.h"
#include "dragon_soul.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum Events
{
    EVENT_DEEP_CORRUPTION = 1,
    EVENT_BLOOD_SUMMON,
    EVENT_COID_BOLT,
	EVENT_SEARING_BLOOD,
	EVENT_MANA_VOID,
	EVENT_PSYCHIC_SLICE,
	EVENT_FORGOTTEN_ONE,
	EVENT_DIGESTIVE_ACID,
	EVENT_SUMMON_FORGOTTEN,
	EVENT_BERSERK
};

enum Spells
{
	// Unsleeping Spells
	SPELL_COID_BOLT = 104849,
	SPELL_SEARING_BLOOD = 105033,
	SPELL_MANA_VOID = 108223,
	SPELL_MANA_DIFFUSION = 105539,
	SPELL_PSYCHIC_SLICE = 105671,
	SPELL_DIGESTIVE_ACID = 105573,
	SPELL_BERSERK = 26662,

	// Blood spells
	SPELL_FUSING_VAPORS = 103968,
	SPELL_BLACK_BLOOD_OF_SHUMA = 104894,
	SPELL_COBALT_BLOOD_OF_SHUMA = 105027,
	SPELL_CRIMSON_BLOOD_OF_SHUMA = 104897,
	SPELL_GLOWING_BLOOD_OF_SHUMA = 104901,
	SPELL_ACIDIC_BLOOD_OF_SHUMA = 104898,
	SPELL_SHADOWED_BLOOD_OF_SHUMA = 104896,
};

Position const SummonPositions[6] =
{
    {-1808.229f, -3136.739f, -173.479f, 0.0f},
    {-1722.599f, -3137.159f, -173.389f, 0.0f},
    {-1662.959f, -2992.280f, -173.514f, 0.0f},
    {-1863.989f, -2993.09f, -174.110f, 0.0f},
    {-1723.76f, -2935.33f, -174.029f, 0.0f},
    {-1663.89f, -3077.129f, -174.479f, 0.0f},
};

enum npc
{
	NPC_BLACK_BLOOD = 55867,
    NPC_COBLAT_BLOOD = 55866,
    NPC_CRIMSON_BLOOD = 55865,
    NPC_GLOWING_BLOOD = 55864,
    NPC_ACIDIC_BLOOD = 55862,
    NPC_SHADOWED_BLOOD = 55863,
    NPC_MANA_VOID = 56231,
    NPC_FORGOTTEN_ONE = 56265, 
};

enum Says
{
	YELL = 0,
	WHISPER_DEATH = 1,
	WHISPER_KILL_ALL = 2,
	WHISPER_KILL = 3,
	WHISPER_AGGRO = 4
};

class boss_unsleeping : public CreatureScript
{
public:
    boss_unsleeping() : CreatureScript("boss_unsleeping") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDragonSoulAI<boss_unsleepingAI>(creature);
    }

    struct boss_unsleepingAI: public BossAI
    {
        boss_unsleepingAI(Creature* creature) : BossAI(creature, BOSS_UNSLEEPING)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;
		Position pos;

		bool mana_void, glowing, crimson;

        void Reset()
        {
             _Reset();
			glowing = false;
			crimson = false;

            instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SetData64(DATA64_KILL_COUNT, 0);
        }

        void EnterCombat(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_BLOOD_SUMMON, urand(15000,30000));
            events.ScheduleEvent(EVENT_COID_BOLT, urand(5000,15000));
			events.ScheduleEvent(EVENT_SEARING_BLOOD, 1);
			events.ScheduleEvent(EVENT_MANA_VOID, 1);
			events.ScheduleEvent(EVENT_FORGOTTEN_ONE, 1);
			events.ScheduleEvent(EVENT_DIGESTIVE_ACID, 1);
			events.ScheduleEvent(EVENT_BERSERK, 600000);
            instance->SetData(DATA_PORTALS_ON_OFF, IN_PROGRESS);
			instance->SetData64(DATA64_KILL_COUNT, 0);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add
			Talk(WHISPER_AGGRO);
			mana_void = false;
            _EnterCombat();
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
				case EVENT_BERSERK:
				    DoCast(SPELL_BERSERK);
					break;
					
                case EVENT_BLOOD_SUMMON:
					if (roll_chance_i(50))
					    me->SummonCreature(NPC_BLACK_BLOOD, SummonPositions[0], TEMPSUMMON_MANUAL_DESPAWN);
					else
					{
					    glowing = true;
					    me->SummonCreature(NPC_GLOWING_BLOOD, SummonPositions[3], TEMPSUMMON_MANUAL_DESPAWN);
					}
					if (roll_chance_i(50))
					    me->SummonCreature(NPC_COBLAT_BLOOD, SummonPositions[1], TEMPSUMMON_MANUAL_DESPAWN);
					else
					    me->SummonCreature(NPC_ACIDIC_BLOOD, SummonPositions[4], TEMPSUMMON_MANUAL_DESPAWN);
					if (roll_chance_i(50))
					{
					    crimson = true;
					    me->SummonCreature(NPC_CRIMSON_BLOOD, SummonPositions[2], TEMPSUMMON_MANUAL_DESPAWN);
					}					
					else
					    me->SummonCreature(NPC_SHADOWED_BLOOD, SummonPositions[5], TEMPSUMMON_MANUAL_DESPAWN);
					if (IsHeroic() && (crimson || roll_chance_i(50)))
					{
					    glowing = false;
					    me->SummonCreature(NPC_GLOWING_BLOOD, SummonPositions[3], TEMPSUMMON_MANUAL_DESPAWN);
					}
					else if (IsHeroic() && glowing)
					{
					    crimson = false;
					    me->SummonCreature(NPC_CRIMSON_BLOOD, SummonPositions[2], TEMPSUMMON_MANUAL_DESPAWN);
					}
					Talk(YELL);
					events.ScheduleEvent(EVENT_BLOOD_SUMMON, 82000);
					break;

                case EVENT_COID_BOLT:
					if (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA))
					    DoCastAOE(SPELL_COID_BOLT);
					else if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
						DoCast(target, SPELL_COID_BOLT);

					if (me->HasAura(SPELL_GLOWING_BLOOD_OF_SHUMA))
                        events.ScheduleEvent(EVENT_COID_BOLT, 6000);
					else
						events.ScheduleEvent(EVENT_COID_BOLT, 12000);
                    break;

				case EVENT_SEARING_BLOOD:
					if (me->HasAura(SPELL_CRIMSON_BLOOD_OF_SHUMA))
					{
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						    DoCast(target, SPELL_SEARING_BLOOD);
						events.ScheduleEvent(EVENT_SEARING_BLOOD, 3500);
					}
					else
					    events.ScheduleEvent(EVENT_SEARING_BLOOD, 1);
					break;

				case EVENT_MANA_VOID:
					if (me->HasAura(SPELL_COBALT_BLOOD_OF_SHUMA) && !mana_void)
					{
					    DoSummon(NPC_MANA_VOID, maw(), 30.0f, 0, TEMPSUMMON_MANUAL_DESPAWN);
						mana_void = true;
					}
					if (!me->HasAura(SPELL_COBALT_BLOOD_OF_SHUMA) && mana_void)
						mana_void = false;
					events.ScheduleEvent(EVENT_MANA_VOID, 1);
					break;

				case EVENT_FORGOTTEN_ONE:
					if (me->HasAura(SPELL_BLACK_BLOOD_OF_SHUMA))
					{
						if (instance->GetData64(DATA64_KILL_COUNT) == 0)
						{
							instance->SetData64(DATA64_KILL_COUNT, 3);
							events.ScheduleEvent(EVENT_SUMMON_FORGOTTEN, urand(3500, 10000));
						}
					}
					events.ScheduleEvent(EVENT_FORGOTTEN_ONE, 1000);
					break;

				case EVENT_SUMMON_FORGOTTEN:
					for (int8 i = 0; i < 3; i++)
						DoSummon(NPC_FORGOTTEN_ONE, maw(), 30.0f, 60000, TEMPSUMMON_TIMED_DESPAWN);
					break;

				case EVENT_DIGESTIVE_ACID:
					if(me->HasAura(SPELL_ACIDIC_BLOOD_OF_SHUMA))
					{
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
					    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
					    {
						    Unit* target = i->getSource();
						    DoCast(target, SPELL_DIGESTIVE_ACID);
					    }
						events.ScheduleEvent(EVENT_DIGESTIVE_ACID, 8300);
					}
					else
					    events.ScheduleEvent(EVENT_DIGESTIVE_ACID, 1);
					break;
                default:
                    break;
                }
            }		

            DoMeleeAttackIfReady();
        }

        void EnterEvadeMode()
		{
		    Talk(WHISPER_KILL_ALL);
			glowing = false;
			crimson = false;
			me->GetMotionMaster()->MoveTargetedHome();
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SetData64(DATA64_KILL_COUNT, 0);
			events.Reset();
			_EnterEvadeMode();
        }

	    void KilledUnit(Unit* /*victim*/)
		{
            Talk(WHISPER_KILL);
		}

        void JustDied(Unit* /*killer*/)
        {
            instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SetData64(DATA64_KILL_COUNT, 0);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			Talk(WHISPER_DEATH);
            _JustDied();
        }

		Creature* maw()
		{
			return me->FindNearestCreature(55544, 1000.0f, true);
		}
    };
};

class npc_blood : public CreatureScript
{
    public:
        npc_blood() : CreatureScript("npc_blood") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_bloodAI>(creature);
		}

		struct npc_bloodAI : public ScriptedAI
		{
		    npc_bloodAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			bool blood;

			void Reset()
			{
				me->SetInCombatWithZone();
				me->AddAura(SPELL_FUSING_VAPORS, me);
				me->SetSpeed(MOVE_RUN, 0.7f);
				blood = true;

				if (me->GetEntry() == NPC_BLACK_BLOOD)
				    me->AddAura(SPELL_BLACK_BLOOD_OF_SHUMA, me);

				if (me->GetEntry() == NPC_COBLAT_BLOOD)
					me->AddAura(SPELL_COBALT_BLOOD_OF_SHUMA, me);

				if (me->GetEntry() == NPC_CRIMSON_BLOOD)
					me->AddAura(SPELL_CRIMSON_BLOOD_OF_SHUMA, me);
				
				if (me->GetEntry() == NPC_GLOWING_BLOOD)
					me->AddAura(SPELL_GLOWING_BLOOD_OF_SHUMA, me);

				if (me->GetEntry() == NPC_ACIDIC_BLOOD)
					me->AddAura(SPELL_ACIDIC_BLOOD_OF_SHUMA, me);

				if (me->GetEntry() == NPC_SHADOWED_BLOOD)
					me->AddAura(SPELL_SHADOWED_BLOOD_OF_SHUMA, me);

				events.Reset();
			}

			void DamageTaken(Unit* doneBy, uint32& damage)
			{
				if (instance->GetData(DATA_BLOOD) == FAIL)
                    damage = 0;
			}

			void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

				if(Creature* u = me->FindNearestCreature(NPC_UNSLEEPING, 100.0f, true))
				{
					me->GetMotionMaster()->MovePoint(0, u->GetPositionX(), u->GetPositionY(), u->GetPositionZ(), true);

				    if(blood == true && me->FindNearestCreature(NPC_UNSLEEPING, 1.5f, true))
				    {
						blood = false;
						if(me->GetEntry() == NPC_BLACK_BLOOD)
						   u->CastSpell(u, SPELL_BLACK_BLOOD_OF_SHUMA);

						if(me->GetEntry() == NPC_COBLAT_BLOOD)
						   u->CastSpell(u, SPELL_COBALT_BLOOD_OF_SHUMA);

					   if(me->GetEntry() == NPC_CRIMSON_BLOOD)
						   u->CastSpell(u, SPELL_CRIMSON_BLOOD_OF_SHUMA);

					   if(me->GetEntry() == NPC_GLOWING_BLOOD)
						   u->CastSpell(u, SPELL_GLOWING_BLOOD_OF_SHUMA);

					   if(me->GetEntry() == NPC_ACIDIC_BLOOD)
						   u->CastSpell(u, SPELL_ACIDIC_BLOOD_OF_SHUMA);

					   if(me->GetEntry() == NPC_SHADOWED_BLOOD)
						   u->CastSpell(u, SPELL_SHADOWED_BLOOD_OF_SHUMA);

					   instance->SetData(DATA_BLOOD, DONE);

					   me->Kill(me);
					   me->DespawnOrUnsummon(5000);
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*pKiller*/)
			{
				me->DespawnOrUnsummon(5000);
				instance->SetData(DATA_BLOOD, FAIL);
			}
		};
};

class npc_mana_void : public CreatureScript
{
    public:
        npc_mana_void() : CreatureScript("npc_mana_void") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_mana_voidAI>(creature);
		}

		struct npc_mana_voidAI : public ScriptedAI
		{
		    npc_mana_voidAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			void Reset()
			{
				me->SetInCombatWithZone();
				me->AddAura(SPELL_MANA_VOID, me);
				events.Reset();
			}

			void JustDied(Unit* /*pKiller*/)
			{
				DoCast(me, SPELL_MANA_DIFFUSION, true);
				me->DespawnOrUnsummon(5000);
			}
		};
};

class npc_forgotten_one : public CreatureScript
{
    public:
        npc_forgotten_one() : CreatureScript("npc_forgotten_one") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return GetDragonSoulAI<npc_forgotten_oneAI>(creature);
		}

		struct npc_forgotten_oneAI : public ScriptedAI
		{
		    npc_forgotten_oneAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;
			uint64 count;

			void Reset()
            {
				events.Reset();
				me->SetInCombatWithZone();
			}

			void EnterCombat(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_PSYCHIC_SLICE, urand(5000,15000));
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
					case EVENT_PSYCHIC_SLICE:
						DoCastVictim(SPELL_PSYCHIC_SLICE);
						events.ScheduleEvent(EVENT_PSYCHIC_SLICE, urand(5000,30000));
						break;
					}
				}
			}

			void JustDied(Unit* /*killer*/)
			{
				me->DespawnOrUnsummon(5000);
				count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count - 1);
			}
		};
};

void AddSC_boss_unsleeping()
{
    new boss_unsleeping();
    new npc_blood();
    new npc_mana_void();
    new npc_forgotten_one();
}