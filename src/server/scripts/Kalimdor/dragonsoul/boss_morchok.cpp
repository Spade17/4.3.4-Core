/* ScriptData
SDName: boss_morchok
SD%Complete: ~90%
SDComment:
SDCategory: Boss Morchok
EndScriptData
*/

#include "ScriptPCH.h"
#include "GameObjectAI.h"
#include "dragon_soul.h"

enum Yells
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,
    SAY_EVENT1 = 2, // No work
    SAY_EVENT2 = 3, // No work
    SAY_EVENT3= 4,
    SAY_EVENT4 = 5,
    SAY_GROUNT1 = 6,
    SAY_GROUNT2 = 7,
    SAY_INTRO = 8,
    SAY_ORB = 9, // No work
    SAY_SLAY = 10,
    SAY_SPLIT = 11
};

enum Events
{
    EVENT_CRUSH_ARMOR = 1,
    EVENT_STOMP,
    EVENT_EARTH,
	EVENT_EARTH_START,
	EVENT_BLACK_BLOOD_OF_THE_EARTH,
    EVENT_EARTHER_VORTEX,
	EVENT_RESONATING_CRYSTAL,
	EVENT_SPIKE_DONE,
	EVENT_SAY,

	// npc black blood
	EVENT_BLACK_BLOOD,
	EVENT_BLACK_BLOOD_DESPAWN,

	// npc crystal
	EVENT_CRYSTAL_BOOM,
	EVENT_CRYSTAL_UPDATE
};

enum SpellsCrystal
{
	SPELL_DANGER = 103534,
	SPELL_WARNING = 103536,
	SPELL_SAFE = 103541,
	SPELL_RESONATING_CRYSTAL_3 = 103545,
    SPELL_RESONATING_CRYSTAL_7 = 108572
};

enum Spells
{
	SPELL_FURIOUS = 103846,
	SPELL_CRUSH_ARMOR = 103687,
	SPELL_STOMP = 103414,
	SPELL_BLACK_BLOOD_OF_THE_EARTH = 103851,
    SPELL_EARTHER_VORTEX = 103821,
	SPELL_SUMMON_KOHCROM = 109017,
	SPELL_RESONATING_CRYSTAL = 103640,
	SPELL_EARTHEN_VORTEX = 103821,
	SPELL_EARTHS_VENGEANCE = 103176,
};

enum Auras
{
	AURA_BLACK_BLOOD_OF_THE_EARTH = 103180
};

class boss_morchok : public CreatureScript
{
public:
    boss_morchok() : CreatureScript("boss_morchok") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDragonSoulAI<boss_morchokAI>(creature);
    }

    struct boss_morchokAI: public BossAI
    {
        boss_morchokAI(Creature* creature) : BossAI(creature, BOSS_MORCHOK)
        {
            instance = creature->GetInstanceScript();
			kohcrom = false;
			furious = false;
			CanAttack = false;
        }

        InstanceScript* instance;
        EventMap events;
		Creature* blackblood;
		GameObject* obj;
		Position pos;
		uint32 count1, count2;

		bool kohcrom, furious, CanAttack;

        void Reset()
        {
            _Reset();

			kohcrom = false;
			furious = false;
			count1 = 0;
			count2 = 0;
            me->RemoveAura(SPELL_FURIOUS);
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

		void GameObjectsSetState(uint32 entry, GOState state, float redius)
		{
			std::list<GameObject*> objects;

			GetGameObjectListWithEntryInGrid(objects, me, entry, redius);

			if (objects.empty())
				return;

			for (std::list<GameObject*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
				(*iter)->SetGoState(state);
		}

        void EnterCombat(Unit* /*who*/)
        {
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add

			if (!IsHeroic())
                events.ScheduleEvent(EVENT_CRUSH_ARMOR, 1500);
            events.ScheduleEvent(EVENT_STOMP, 12000);
			events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 19000);
            events.ScheduleEvent(EVENT_EARTH, urand(54000, 60000));
			//events.ScheduleEvent(EVENT_SAY, 30000);

			kohcrom = false;
			furious = false;
			CanAttack = true;
			count1 = 0;
			count2 = 0;

            Talk(SAY_AGGRO);
            _EnterCombat();
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (HealthBelowPct(90) && IsHeroic() && !kohcrom)
            {
                DoCastVictim(SPELL_SUMMON_KOHCROM);
                Talk(SAY_SPLIT);
                kohcrom = true;
            }

            if (HealthBelowPct(20) && !furious)
            {
                DoCast(me, SPELL_FURIOUS);
				furious = true;
            }

			if (CanAttack)
			    GetPlayersInCombat();

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_CRUSH_ARMOR:
		            DoCastVictim(SPELL_CRUSH_ARMOR);
                    events.ScheduleEvent(EVENT_CRUSH_ARMOR, urand(5000,15000));
                    break;

                case EVENT_STOMP:
				    if (count1 < 3)
					{
		                DoCast(SPELL_STOMP);
                        Talk(SAY_GROUNT1);
						count1++;
					}
                    events.ScheduleEvent(EVENT_STOMP, 12000);
                    break;

				case EVENT_EARTH:
					me->NearTeleportTo(-2004.24f, -2407.12f, 70.2915f, 6.23712f);
					events.ScheduleEvent(EVENT_EARTH_START, 1500);
					events.CancelEvent(EVENT_CRUSH_ARMOR);
					events.CancelEvent(EVENT_STOMP);
					events.CancelEvent(EVENT_RESONATING_CRYSTAL);
					count1 = 0;
					count2 = 0;
					break;

				case EVENT_EARTH_START:
					{
					    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
					    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
					    {
						    Unit* target = i->getSource();
						    target->AddAura(SPELL_EARTHEN_VORTEX, target);
						    target->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 10.0f, 5.0f, 0);
					    }
					    for (int32 i = 0; i < urand(10, 25); i++)
						{
							me->GetRandomNearPosition(pos, 60.0f);
							obj = me->SummonGameObject(GO_MORCHOK_SPIKE, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetAngle(me->GetPositionX(), me->GetPositionY()), 0.0f, 0.0f, 0.0f, 0.0f, 30);
							if (obj->FindNearestCreature(NPC_MORCHOK, 15.0f, true))
								obj->RemoveFromWorld();
						}
						GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_READY, 1000.0f);
						DoCast(SPELL_EARTHS_VENGEANCE);
					    events.ScheduleEvent(EVENT_BLACK_BLOOD_OF_THE_EARTH, 5100);
					    break;
					}
				case EVENT_BLACK_BLOOD_OF_THE_EARTH:
					Talk(SAY_GROUNT2);
					for (int32 i = 0; i < 300; i++)
					{
					    blackblood = DoSummon(NPC_BLACK_BLOOD_OF_THE_EARTH, me, 60.0f, 0, TEMPSUMMON_MANUAL_DESPAWN);
					    blackblood->SetInCombatWithZone();
					    blackblood->SetPhaseMask(2, true);
					}
					DoCast(SPELL_BLACK_BLOOD_OF_THE_EARTH);
					events.ScheduleEvent(EVENT_SPIKE_DONE, 16000);
				    break;

				case EVENT_SPIKE_DONE:
					GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
					events.ScheduleEvent(EVENT_EARTH, 54000);
					if (!IsHeroic())
                        events.ScheduleEvent(EVENT_CRUSH_ARMOR, 1500);
					events.ScheduleEvent(EVENT_STOMP, 12000);
					events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 18000);
					break;

				case EVENT_RESONATING_CRYSTAL:
				    if (count2 < 2)
					{
					    me->GetRandomNearPosition(pos, 60.0f);
					    me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_RESONATING_CRYSTAL, true);
						count2++;
					}
					events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 12000);
					break;

				case EVENT_SAY:
					switch (urand(0, 1))
					{
					case 0:
						Talk(SAY_EVENT3);
						break;
					case 1:
						Talk(SAY_EVENT4);
						break;
					}
					break;
                default:
                    break;
                }
            }		

            DoMeleeAttackIfReady();
        }
		
		void DamageTaken(Unit* who, uint32& damage)
        {
			if (!IsHeroic())
				return;

            if (Creature* kochrom = me->FindNearestCreature(NPC_KOHCROM, 1000.0f, true))
            {
                if (damage >= kochrom->GetHealth() - 1)
                {
                    kochrom->SetHealth(kochrom->GetMaxHealth() - 1); // don't die, stupid.                
                    kochrom->DespawnOrUnsummon(5000);
                }
                else
                    kochrom->SetHealth(kochrom->GetHealth() - damage);
            }
        }

        void KilledUnit(Unit* victim)
		{
            Talk(SAY_SLAY);
		}

		void EnterEvadeMode()
		{
			CanAttack = false;
			GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DANGER);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WARNING);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAFE);
			DespawnCreatures(NPC_BLACK_BLOOD_OF_THE_EARTH);
			DespawnCreatures(NPC_RESONATING_CRYSTAL);
			me->GetMotionMaster()->MoveTargetedHome();
			me->RemoveAura(SPELL_FURIOUS);
			kohcrom = false;
			furious = false;
			if (Creature* kochrom = me->FindNearestCreature(NPC_KOHCROM, 1000.0f, true))
				kochrom->AI()->EnterEvadeMode();
			events.Reset();

			_EnterEvadeMode();
		}

        void JustDied(Unit* /*killer*/)
        {
			GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			if (Creature* kochrom = me->FindNearestCreature(NPC_KOHCROM, 1000.0f, true))
				instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, kochrom); // Remove
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DANGER);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WARNING);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SAFE);
			DespawnCreatures(NPC_BLACK_BLOOD_OF_THE_EARTH);
			DespawnCreatures(NPC_RESONATING_CRYSTAL);

            Talk(SAY_DEATH);

            _JustDied();
        }

		private:
		    std::list<Unit *> playerList;
    };
};

class boss_kohcrom : public CreatureScript
{
public:
    boss_kohcrom() : CreatureScript("boss_kohcrom") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDragonSoulAI<boss_kohcromAI>(creature);
    }

    struct boss_kohcromAI: public ScriptedAI
    {
        boss_kohcromAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
			furious = false;
        }

        InstanceScript* instance;
        EventMap events;
		Creature* blackblood;
		GameObject* obj;
		Position pos;
		uint32 count1, count2;

		bool furious;

        void Reset()
        {
			events.Reset();

			furious = false;
			count1 = 0;
			count2 = 0;
            me->RemoveAura(SPELL_FURIOUS);
			if (Creature* morchok = me->FindNearestCreature(NPC_MORCHOK, 1000.0f, true))
				me->SetHealth(morchok->GetHealth());
        }

		void GameObjectsSetState(uint32 entry, GOState state, float redius)
		{
			std::list<GameObject*> objects;

			GetGameObjectListWithEntryInGrid(objects, me, entry, redius);

			if (objects.empty())
				return;

			for (std::list<GameObject*>::iterator iter = objects.begin(); iter != objects.end(); ++iter)
				(*iter)->SetGoState(state);
		}

        void EnterCombat(Unit* /*who*/)
        {
			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add

			events.ScheduleEvent(EVENT_STOMP, 12000);
			events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 19000);
            events.ScheduleEvent(EVENT_EARTH, urand(54000, 60000));
			//events.ScheduleEvent(EVENT_SAY, 30000);

			furious = false;
			count1 = 0;
			count2 = 0;

            //Talk(SAY_AGGRO);
        }

        void UpdateAI(uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (HealthBelowPct(20) && !furious)
            {
                DoCast(me, SPELL_FURIOUS);
				furious = true;
            }

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
				case EVENT_STOMP:
				    if (count1 < 3)
					{
		                DoCast(SPELL_STOMP);
                        Talk(SAY_GROUNT1);
						count1++;
					}
                    events.ScheduleEvent(EVENT_STOMP, 12000);
                    break;
				case EVENT_EARTH:
					me->NearTeleportTo(-2004.24f, -2407.12f, 70.2915f, 6.23712f);
					events.ScheduleEvent(EVENT_EARTH_START, 1500);
					events.CancelEvent(EVENT_CRUSH_ARMOR);
					events.CancelEvent(EVENT_STOMP);
					events.CancelEvent(EVENT_RESONATING_CRYSTAL);
					count1 = 0;
					count2 = 0;
					break;

				case EVENT_EARTH_START:
					{
					    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
					    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
					    {
						    Unit* target = i->getSource();
						    target->AddAura(SPELL_EARTHEN_VORTEX, target);
						    target->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 10.0f, 5.0f, 0);
					    }
					    for (int32 i = 0; i < urand(10, 25); i++)
						{
							me->GetRandomNearPosition(pos, 60.0f);
							obj = me->SummonGameObject(GO_MORCHOK_SPIKE, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetAngle(me->GetPositionX(), me->GetPositionY()), 0.0f, 0.0f, 0.0f, 0.0f, 30);
							if (obj->FindNearestCreature(NPC_MORCHOK, 15.0f, true))
								obj->RemoveFromWorld();
						}
						GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_READY, 1000.0f);
						DoCast(SPELL_EARTHS_VENGEANCE);
					    events.ScheduleEvent(EVENT_BLACK_BLOOD_OF_THE_EARTH, 5100);
					    break;
					}
				case EVENT_BLACK_BLOOD_OF_THE_EARTH:
					//Talk(SAY_GROUNT2);
					for (int32 i = 0; i < 300; i++)
					{
					    blackblood = DoSummon(NPC_BLACK_BLOOD_OF_THE_EARTH, me, 60.0f, 0, TEMPSUMMON_MANUAL_DESPAWN);
					    blackblood->SetInCombatWithZone();
					    blackblood->SetPhaseMask(2, true);
					}
					DoCast(SPELL_BLACK_BLOOD_OF_THE_EARTH);
					events.ScheduleEvent(EVENT_SPIKE_DONE, 16000);
				    break;

				case EVENT_SPIKE_DONE:
					GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
					events.ScheduleEvent(EVENT_EARTH, 54000);
					if (!IsHeroic())
                        events.ScheduleEvent(EVENT_CRUSH_ARMOR, 1500);
					events.ScheduleEvent(EVENT_STOMP, 12000);
					events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 18000);
					break;

				case EVENT_RESONATING_CRYSTAL:
				    if (count2 < 2)
					{
					    me->GetRandomNearPosition(pos, 60.0f);
					    me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), SPELL_RESONATING_CRYSTAL, true);
						count2++;
					}
					events.ScheduleEvent(EVENT_RESONATING_CRYSTAL, 12000);
					break;
                default:
                    break;
                }
            }		

            DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit* victim)
		{
            Talk(SAY_SLAY);
		}

		void DamageTaken(Unit* who, uint32& damage)
        {
			if (!IsHeroic())
				return;

            if (Creature* morchok = me->FindNearestCreature(NPC_MORCHOK, 1000.0f, true))
            {
                if (!damage >= morchok->GetHealth() - 1)
                    morchok->SetHealth(morchok->GetHealth() - damage);
            }
        }

		void EnterEvadeMode()
		{
			GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			me->DespawnOrUnsummon();

			_EnterEvadeMode();
		}

        void JustDied(Unit* /*killer*/)
        {
			GameObjectsSetState(GO_MORCHOK_SPIKE, GO_STATE_ACTIVE, 1000.0f);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove

            //Talk(SAY_DEATH);
        }

		private:
		    std::list<Unit *> playerList;
    };
};

class npc_black_blood_of_the_earth : public CreatureScript // 55267
{
    public:
        npc_black_blood_of_the_earth() : CreatureScript("npc_black_blood_of_the_earth") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_black_blood_of_the_earthAI(creature);
		}

		struct npc_black_blood_of_the_earthAI : public ScriptedAI
		{
		    npc_black_blood_of_the_earthAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;

			void Reset()
			{
			    events.ScheduleEvent(EVENT_BLACK_BLOOD, urand(2000, 12000));
			    me->DespawnOrUnsummon(17000);
				me->SetCanFly(true);
			}

			void UpdateAI(uint32 diff)
            {
				events.Update(diff);

				if (GameObject* obj = me->FindNearestGameObject(GO_MORCHOK_SPIKE, 5.0f))
				    if (!me->isInBack(obj, obj->GetOrientation()))
					{
						me->DespawnOrUnsummon();
					    return;
					}

				while (uint32 eventId = events.ExecuteEvent())
				{
				    switch (eventId)
				    {
					case EVENT_BLACK_BLOOD:
						me->SetPhaseMask(1, true);
						DoCast(me, AURA_BLACK_BLOOD_OF_THE_EARTH);
						events.ScheduleEvent(EVENT_BLACK_BLOOD, 4000);
						break;
					}
				}
			}
		};
};

class npc_resonating_crystal : public CreatureScript // 55346
{
    public:
        npc_resonating_crystal() : CreatureScript("npc_resonating_crystal") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_resonating_crystalAI(creature);
		}

		struct npc_resonating_crystalAI : public ScriptedAI
		{
		    npc_resonating_crystalAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			InstanceScript* instance;
			EventMap events;
			
			bool cast;

			void Reset()
			{
				events.Reset();
				me->SetInCombatWithZone();
				cast = false;
			}

			void EnterCombat(Unit* /*who*/)
			{
                events.ScheduleEvent(EVENT_CRYSTAL_BOOM, 12000);
				cast = false;
			}

			void UpdateAI(uint32 diff)
            {
				events.Update(diff);
				
				if (!cast)
				{
					if (me->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || me->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
					{
						SelectTargetList(playerList, 3, SELECT_TARGET_RANDOM, 0.0f,true);
						for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
						{
				            Unit* target = (*itr);

							if (target->GetDistance(me) <= 4.0f)
								DoCast(target, SPELL_SAFE);

							if (target->GetDistance(me) > 5.0f && target->GetDistance(me) < 20.0f)
								DoCast(target, SPELL_WARNING);

							if (target->GetDistance(me) > 21.0f && target->GetDistance(me) < 100.0f)
								DoCast(target, SPELL_DANGER);
						}
					}
					else
					{
				        SelectTargetList(playerList, 7, SELECT_TARGET_RANDOM, 0.0f,true);
						for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
						{
				            Unit* target = (*itr);

							if (target->GetDistance(me) <= 4.0f)
								DoCast(target, SPELL_SAFE);
							if (target->GetDistance(me) > 5.0f && target->GetDistance(me) < 20.0f)
								DoCast(target, SPELL_WARNING);
							if (target->GetDistance(me) > 21.0f && target->GetDistance(me) < 100.0f)
								DoCast(target, SPELL_DANGER);
						}
					}

					cast = true;
					events.ScheduleEvent(EVENT_CRYSTAL_UPDATE, 1000);
				}
				else
				{
				    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
				    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
				    {
					    Unit* target = i->getSource();

			            if (target->HasAura(SPELL_SAFE) || target->HasAura(SPELL_WARNING) || target->HasAura(SPELL_DANGER))
			            {
					        if (target->GetDistance(me) <= 4.0f)
				            {
					            if (!target->HasAura(SPELL_SAFE))
					            {
					                DoCast(target, SPELL_SAFE);

					                if (target->HasAura(SPELL_WARNING))
					                    target->RemoveAura(SPELL_WARNING);

					                if (target->HasAura(SPELL_DANGER))
					                    target->RemoveAura(SPELL_DANGER);
					            }
						    }
							if (target->GetDistance(me) > 4.0f && target->GetDistance(me) < 20.0f)
						    {
					            if (!target->HasAura(SPELL_WARNING))
						        {
					                DoCast(target, SPELL_WARNING);

					                if (target->HasAura(SPELL_SAFE))
					                    target->RemoveAura(SPELL_SAFE);

					                if (target->HasAura(SPELL_DANGER))
					                    target->RemoveAura(SPELL_DANGER);
							    }
							}
							if (target->GetDistance(me) > 20.0f && target->GetDistance(me) < 100.0f)
							{
					            if (!target->HasAura(SPELL_DANGER))
					            {
					                DoCast(target, SPELL_DANGER);

					                if (target->HasAura(SPELL_SAFE))
					                    target->RemoveAura(SPELL_SAFE);

					                if (target->HasAura(SPELL_DANGER))
					                    target->RemoveAura(SPELL_DANGER);
					            }
							}
						}
					}
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
				    switch (eventId)
				    {
					case EVENT_CRYSTAL_BOOM:
						{
						    if (me->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || me->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
						        DoCast(SPELL_RESONATING_CRYSTAL_3);
						    else
						        DoCast(SPELL_RESONATING_CRYSTAL_7);

						    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
						    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    {
						        Unit* target = i->getSource();

						        if (target->HasAura(SPELL_DANGER))
							        target->RemoveAura(SPELL_DANGER);

						        if (target->HasAura(SPELL_WARNING))
							        target->RemoveAura(SPELL_WARNING);

						        if (target->HasAura(SPELL_SAFE))
							        target->RemoveAura(SPELL_SAFE);
							}

						    me->DespawnOrUnsummon(1000);
						    break;
						}
					}
				}
			}
		    private:
		        std::list<Unit *> playerList;
		};
};

void AddSC_boss_morchok()
{
    new boss_morchok();
	new boss_kohcrom();
	new npc_black_blood_of_the_earth();
	new npc_resonating_crystal();
}