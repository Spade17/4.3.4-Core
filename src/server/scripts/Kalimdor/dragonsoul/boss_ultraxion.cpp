/* ScriptData
SDName: boss_ultraxion
SD%Complete: ~90%
SDComment:
SDCategory: Boss Ultraxion
EndScriptData
*/

#include "ScriptPCH.h"
#include "dragon_soul.h"

#define H_TARGETS_10 1
#define H_TARGETS_25 3
#define H_TARGETS_10_HEROIC 2
#define H_TARGETS_25_HEROIC 5

#define F_TARGETS_10 1
#define F_TARGETS_25 3
#define F_TARGETS_10_HEROIC 2
#define F_TARGETS_25_HEROIC 6

#define TRALL_MENU "We are ready!"

enum Events
{
	EVENT_ATTACK_START = 1,
	EVENT_TWILIGHT_BURST,
	EVENT_FADING_LIGHT,
	EVENT_YELL_INTRO_2,
	EVENT_FLEW_DONE,
	EVENT_UNSTABLE_MONSTROSITY,
	EVENT_UNSTABLE_MONSTROSITY_2,
	EVENT_ENTER_ZONE_COMBAT,
	EVENT_BERSERK,

	EVENT_HOUR_OF_TWILIGHT = 133,
	EVENT_HOUR_OF_TWILIGHT_TIME = 134,
	EVENT_TWILIGHT_ERUPTION = 135,
	EVENT_TWILIGHT_ERUPTION_KILLCASTER = 136
};

enum Spells
{
	SPELL_TWILIGHT_SHIFT = 106369,
	SPELL_HOUR_OF_TWILIGHT = 106371,
	SPELL_TWILIGHT_BURST = 106415,
	SPELL_TWILIGHT_ERUPTION = 106388,
	SPELL_FADING_LIGHT = 109075,
	SPELL_UNSTABLE_MONSTROSITY_6SEC = 106372,
	SPELL_UNSTABLE_MONSTROSITY_5SEC = 106376,
	SPELL_UNSTABLE_MONSTROSITY_4SEC = 106377,
	SPELL_UNSTABLE_MONSTROSITY_3SEC = 106378,
	SPELL_UNSTABLE_MONSTROSITY_2SEC = 106379,
	SPELL_UNSTABLE_MONSTROSITY_1SEC = 106380,
	SPELL_FADED_INTO_TWILIGHT = 105927,
	SPELL_BERSERK = 26662
};

enum Text
{
	YELL_AGGRO = 1, // 26314
	YELL_INTRO_1 = 2, // 26317
	YELL_INTRO_2 = 3, // 26318
};

enum DSActions
{
	ACTION_ATTACK_ON_ULTRAXION = 1,
	ACTION_ULTRAXION_ATTACKSTART,
	ACTION_START,
	ACTION_MOVE,
	ACTION_ATTACK,
	ACTION_PHASE_2,
	ACTION_END,
	ACTION_HOUR_OF_TWILIGHT,
	ACTION_TWILIGHT_ERUPTION
};

class boss_ultraxion : public CreatureScript
{
public:
    boss_ultraxion() : CreatureScript("boss_ultraxion") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDragonSoulAI<boss_ultraxionAI>(creature);
    }

    struct boss_ultraxionAI: public BossAI
    {
        boss_ultraxionAI(Creature* creature) : BossAI(creature, BOSS_ULTRAXION)
        {
            instance = creature->GetInstanceScript();
			fading_light = RAID_MODE(109075, 110080, 110079, 110078);
        }

        InstanceScript* instance;
        EventMap events;
		Position pos;
		uint32 fading_light;

		bool burst;
		bool flew;

        void Reset()
        {
			me->ResetLootMode();
			events.Reset();
			burst = false;
			flew = false;
			me->GetMotionMaster()->MovePoint(1, -1686.472f, -2383.564f, 356.575f);
			me->SetPhaseMask(17, true);
			events.ScheduleEvent(EVENT_YELL_INTRO_2, 10000);
			events.ScheduleEvent(EVENT_UNSTABLE_MONSTROSITY, 1);
			if (instance->GetData(DATA_PORTALS_ON_OFF) == IN_PROGRESS)
			{
				me->DespawnOrUnsummon();
				instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			}
			Talk(YELL_INTRO_1);
        }

        void EnterCombat(Unit* /*who*/)
        {
			_EnterCombat();
			burst = false;
            instance->SetData(DATA_PORTALS_ON_OFF, IN_PROGRESS);
			events.ScheduleEvent(EVENT_TWILIGHT_ERUPTION, 360000);
			events.ScheduleEvent(EVENT_FADING_LIGHT, 60000);
			events.ScheduleEvent(EVENT_HOUR_OF_TWILIGHT, 45500);
			events.ScheduleEvent(EVENT_ENTER_ZONE_COMBAT, 1000);
			events.ScheduleEvent(EVENT_BERSERK, 300000);
        }

		void DoAction(int32 actions)
		{
			switch (actions)
			{
			case ACTION_HOUR_OF_TWILIGHT:
				dragonsoul()->AI()->DoAction(ACTION_HOUR_OF_TWILIGHT);
				events.ScheduleEvent(EVENT_TWILIGHT_ERUPTION, 5000);
				events.CancelEvent(EVENT_HOUR_OF_TWILIGHT);
				break;
			}
		}

		void CastHeroicWill()
		{
			Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
			    if (Unit* targets = i->getSource())
				    if (!targets->HasAura(105554))
						targets->AddAura(105554, targets);
		}

		void CastUnstableMonstrosity()
		{
			// ”даление аур в Spell_linked_spell
			if (me->HasAura(SPELL_UNSTABLE_MONSTROSITY_6SEC))
				DoCast(SPELL_UNSTABLE_MONSTROSITY_5SEC);
			else if (me->HasAura(SPELL_UNSTABLE_MONSTROSITY_5SEC))
				DoCast(SPELL_UNSTABLE_MONSTROSITY_4SEC);
			else if (me->HasAura(SPELL_UNSTABLE_MONSTROSITY_4SEC))
				DoCast(SPELL_UNSTABLE_MONSTROSITY_3SEC);
			else if (me->HasAura(SPELL_UNSTABLE_MONSTROSITY_3SEC))
				DoCast(SPELL_UNSTABLE_MONSTROSITY_2SEC);
			else if (me->HasAura(SPELL_UNSTABLE_MONSTROSITY_2SEC))
				DoCast(SPELL_UNSTABLE_MONSTROSITY_1SEC);
		}
		
		void RemoveAllAuras()
		{
		    // Gifts
		    instance->DoRemoveAurasDueToSpellOnPlayers(106080);
		    instance->DoRemoveAurasDueToSpellOnPlayers(105896);
		    instance->DoRemoveAurasDueToSpellOnPlayers(105900);
		    instance->DoRemoveAurasDueToSpellOnPlayers(105903);
		    instance->DoRemoveAurasDueToSpellOnPlayers(105984);
		    // other
		    instance->DoRemoveAurasDueToSpellOnPlayers(106368);
		    instance->DoRemoveAurasDueToSpellOnPlayers(105554);
			instance->DoRemoveAurasDueToSpellOnPlayers(106498);
			instance->DoRemoveAurasDueToSpellOnPlayers(109075);
			instance->DoRemoveAurasDueToSpellOnPlayers(105927);
		}

		void EnterEvadeMode()
		{
			instance->SetBossState(BOSS_ULTRAXION, FAIL);
			instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
			RemoveAllAuras();
			me->DespawnOrUnsummon();
		}

		void SetPlayersInCombat()
		{
		    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
		    if (!PlayerList.isEmpty())
				for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					if (Player *player = i->getSource())
						if (player->isAlive() && !player->isGameMaster() && player->GetAreaId() == 5928)
							player->SetInCombatWith(me->ToUnit());
	    }

        void UpdateAI(uint32 diff)
        {
			if (!flew)
			{
				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_UNSTABLE_MONSTROSITY:
					{
						if (Creature* cr = me->FindNearestCreature(45979, 200.0f, true))
						{
							for (int8 i = 0; i < 3; i++)
							{
						        cr->GetRandomNearPosition(pos, 50.0f);
						        me->CastSpell(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 106375, true);
							}
						}
						events.ScheduleEvent(EVENT_UNSTABLE_MONSTROSITY, 500);
						break;
					}
					case EVENT_YELL_INTRO_2:
					{
						Talk(YELL_INTRO_2);
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
						for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    if (Unit* targets = i->getSource())
						        if (targets->GetAreaId() == 5928)
						            DoCast(targets, SPELL_TWILIGHT_SHIFT, true);
						CastHeroicWill();
						events.ScheduleEvent(EVENT_FLEW_DONE, 10000);
						break;
					}
					case EVENT_FLEW_DONE:
						flew = true;
						events.ScheduleEvent(EVENT_ATTACK_START, 2000);
						break;
					}
				}

				events.Update(diff);
			}

			if (!flew)
				return;
				
			{
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
				     for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
						if (Player* victim = itr->getSource())
						    if (victim->isAlive() && !victim->isGameMaster() && victim->GetAreaId() == 5928)
			                    if (!me->IsWithinMeleeRange(victim, 5.0f) && !burst)
			                    {
			                        burst = true;
			                        events.ScheduleEvent(EVENT_TWILIGHT_BURST, 2000);
			                    }
			                    else
			                    {
			                        burst = false;
			                        events.CancelEvent(EVENT_TWILIGHT_BURST);
			                    }
            }
				
            while (uint32 eventId = events.ExecuteEvent())
            {
				switch (eventId)
				{
				case EVENT_ATTACK_START:
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1);
					events.ScheduleEvent(EVENT_UNSTABLE_MONSTROSITY_2, 60000);
					events.CancelEvent(EVENT_UNSTABLE_MONSTROSITY);
					DoCast(SPELL_UNSTABLE_MONSTROSITY_6SEC);
					SetPlayersInCombat();
					CastHeroicWill();
					instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Ad
					if (Creature* dragonsoul = me->FindNearestCreature(NPC_DRAGON_SOUL, 1000.0f, true))
						dragonsoul->AI()->DoAction(ACTION_ULTRAXION_ATTACKSTART);
					Talk(YELL_AGGRO);
					break;

				case EVENT_ENTER_ZONE_COMBAT:
					SetPlayersInCombat();
					events.ScheduleEvent(EVENT_ENTER_ZONE_COMBAT, 1000);
					break;

				case EVENT_BERSERK:
					DoCast(SPELL_BERSERK);
					break;

				case EVENT_UNSTABLE_MONSTROSITY_2:
					CastUnstableMonstrosity();
					events.ScheduleEvent(EVENT_UNSTABLE_MONSTROSITY_2, 60000);
					break;

				case EVENT_TWILIGHT_ERUPTION:
					DoCast(SPELL_TWILIGHT_ERUPTION);
					events.ScheduleEvent(EVENT_TWILIGHT_ERUPTION_KILLCASTER, 5500);
					break;

				case EVENT_TWILIGHT_ERUPTION_KILLCASTER:
					dragonsoul()->AI()->DoAction(ACTION_TWILIGHT_ERUPTION);
					break;

				case EVENT_TWILIGHT_BURST:
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						DoCast(target, SPELL_TWILIGHT_BURST, true);
					events.ScheduleEvent(EVENT_TWILIGHT_BURST, 1100);
					break;

				case EVENT_FADING_LIGHT:
					if (me->getVictim()->HasAura(fading_light))
					    DoCastVictim(SPELL_FADING_LIGHT);
					SelectTargetList(playerList, RAID_MODE(F_TARGETS_10, F_TARGETS_25, F_TARGETS_10_HEROIC, F_TARGETS_25_HEROIC), SELECT_TARGET_RANDOM, 0.0f, true);
					for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
					{
						Unit* targets = (*itr);
						if (!targets->HasAura(fading_light))
					        DoCast(targets, SPELL_FADING_LIGHT);
					}
					events.ScheduleEvent(EVENT_FADING_LIGHT, 120000);
					break;

				case EVENT_HOUR_OF_TWILIGHT:
				{
					events.ScheduleEvent(EVENT_HOUR_OF_TWILIGHT_TIME, 4990);
					Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
				    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
					    if (Player* player = i->getSource())
					        DoCast(player, SPELL_HOUR_OF_TWILIGHT);
					events.ScheduleEvent(EVENT_HOUR_OF_TWILIGHT, 45500);
					break;
				}
				case EVENT_HOUR_OF_TWILIGHT_TIME:
				{
					uint8 players = 0;
					Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
				    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
					    if (Player* player = i->getSource())
						    if (!player->HasAura(106175) && player->isAlive() && player->GetAreaId() == 5928)
							    ++players;

					if (players < RAID_MODE(H_TARGETS_10, H_TARGETS_25, H_TARGETS_10_HEROIC, H_TARGETS_25_HEROIC))
						DoAction(ACTION_HOUR_OF_TWILIGHT);
					break;
				}
				}
            }		

			events.Update(diff);

            DoMeleeAttackIfReady();
        }

		void KilledUnit(Unit* victim)
        {
        }

        void JustDied(Unit* pKiller)
        {
			_JustDied();
			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
            instance->SetData(DATA_PORTALS_ON_OFF, DONE);
			RemoveAllAuras();
			dragonsoul()->AI()->DoAction(ACTION_END);
            pKiller->SummonGameObject(RAID_MODE(GO_ULTRAXION_LOOT_10N, GO_ULTRAXION_LOOT_25N, GO_ULTRAXION_LOOT_10H, GO_ULTRAXION_LOOT_25H), -1753.63f,  -2370.01f, 340.845f, 0.0f, 0.0f, 0.0f, -0.66913f, 0.743145f, 100000);
        }

		Creature* dragonsoul()
        {
            return me->FindNearestCreature(NPC_DRAGON_SOUL, 1000.0f, true);
        }

	private:
		std::list<Unit *> playerList;

    };
};

// 109075
class spell_ds_fading_light : public SpellScriptLoader
{
    public:
        spell_ds_fading_light() : SpellScriptLoader("spell_ds_fading_light") { }

        class spell_ds_fading_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ds_fading_light_AuraScript);
			
			bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FADED_INTO_TWILIGHT))
                    return false;
                return true;
            }

			void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
				Unit* target = GetTarget();
				AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
				if (removeMode != AURA_REMOVE_BY_EXPIRE)
                    return;

				if (target->HasAura(106368))
					GetCaster()->Kill(target, true);
				else
					target->CastSpell(target, SPELL_FADED_INTO_TWILIGHT, true);
            }

            void Register()
            {
				AfterEffectRemove += AuraEffectRemoveFn(spell_ds_fading_light_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ds_fading_light_AuraScript();
        }
};

// 106372
class spell_ds_unstable_monstrosity : public SpellScriptLoader
{
    public:
        spell_ds_unstable_monstrosity() : SpellScriptLoader("spell_ds_unstable_monstrosity") { }

        class spell_ds_unstable_monstrosity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ds_unstable_monstrosity_AuraScript);

			Position pos;

			bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(106375))
                    return false;
                return true;
            }

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
				Unit* victim = GetCaster()->getVictim();
				GetCaster()->CastSpell(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 106375, false);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ds_unstable_monstrosity_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ds_unstable_monstrosity_AuraScript();
        }
};

enum DSEvents
{
	// Other
	EVENT_FACTING_TO_OBJ = 1,
	// Events Intro 1
	EVENT_INTRO_1_SAY_1,
	EVENT_INTRO_1_SAY_2,
	EVENT_INTRO_1_SAY_3,
	EVENT_INTRO_1_SAY_4,
	EVENT_INTRO_1_SAY_5,
	EVENT_INTRO_1_SAY_6,
	EVENT_INTRO_1_SAY_7,
	EVENT_INTRO_1_SAY_8,
	EVENT_INTRO_1_SAY_9,

	// Events Intro 2
	EVENT_INTRO_2_SAY_1,
	EVENT_INTRO_2_SAY_2,
	EVENT_INTRO_2_SAY_3,
	EVENT_INTRO_2_RITUAL_1,
	EVENT_INTRO_2_SAY_4,
	EVENT_INTRO_2_RITUAL_2,
	EVENT_INTRO_2_SAY_5,
	EVENT_INTRO_2_SAY_6,
	EVENT_INTRO_2_SAY_7,
	EVENT_INTRO_2_RITUAL_3,

	// Events Intro 3
	EVENT_INTRO_3_YELL_1,
	EVENT_INTRO_3_YELL_2,
	EVENT_INTRO_3_SAY,

	// Events Attack Ultraxion
	EVENT_ULTRAXION_1,
	EVENT_ULTRAXION_2,
	EVENT_ULTRAXION_3,
	EVENT_LDOZ_TRALL_SAY,
	EVENT_LAST_DEFENDER_OF_AZEROTH,
	EVENT_GOL_ALEXSTRASZA_SAY,
	EVENT_GIFT_OF_LIFE,
	EVENT_GIFT_OF_LIFE_2,
	EVENT_EOD_YSERA_SAY,
	EVENT_ESSENCE_OF_DREAMS,
	EVENT_ESSENCE_OF_DREAMS_2,
	EVENT_SOM_KALECGOS_SAY,
	EVENT_SOURCE_OF_MAGIC,
	EVENT_SOURCE_OF_MAGIC_2,
	EVENT_T_NOZDORMU_SAY,
	EVENT_TIMELOOP,
	// Phase 2
	EVENT_PHASE_2_SAY,
	// End
	EVENT_END_SAY_1,
	EVENT_END_SAY_2,
	EVENT_END_MOVE,
	EVENT_END_MOVIE,
	EVENT_END_TELEPORT
};

enum DSGossips
{
	GOSSIP_ACTION_START_INTRO_2 = 1
};

enum DSSpells
{
	SPELL_WARD_OF_TIME = 108160,
	SPELL_WARD_OF_EARTH = 108161,
	SPELL_WARD_OF_MAGIC = 108162,
	SPELL_WARD_OF_LIFE = 108163,
	SPELL_WARD_OF_DREAMS = 108164,
	SPELL_LAST_DEFENDER_OF_AZEROTH = 106080,
	SPELL_GIFT_OF_LIFE = 106042,
	SPELL_ESSENCE_OF_DREAMS = 106049,
	SPELL_SOURCE_OF_MAGIC = 106050,
	SPELL_TIMELOOP = 105984
};

#define TwhilighAssaulterCount 15

Position const TwilightAssaulter[15] =
{
	{-1739.28f, -2413.82f, 384.919f, 1.81319f},
	{-1794.55f, -2372.97f, 393.308f, 2.3983f},
	{-1776.1f, -2414.53f, 389.929f, 4.95871f},
	{-1854.6f, -2413.46f, 392.854f, 5.7441f},
	{-1838.05f, -2397.06f, 408.709f, 4.76235f},
	{-1793.9f, -2354.42f, 386.704f, 4.97834f},
	{-1763.34f, -2337.16f, 393.219f, 4.0123f},
	{-1718.81f, -2355.59f, 379.388f, 3.168f},
	{-1702.98f, -2375.94f, 384.306f, 2.72032f},
	{-1701.56f, -2416.57f, 380.043f, 1.68359f},
	{-1731.92f, -2448.88f, 383.236f, 1.12989f},
	{-1747.53f, -2471.81f, 381.815f, 3.81988f},
	{-1800.22f, -2417.08f, 381.715f, 3.95732f},
	{-1771.82f, -2388.87f, 398.91f, 3.98088f},
	{-1760.84f, -2375.58f, 397.212f, 4.2597f}
};

Position const AspectCastPosition[6] =
{
	{-1764.344f, -2387.663f, 341.597f}, 
	{-1766.328f, -2383.785f, 341.597f},

	{-1763.708f, -2394.558f, 341.598f},
	{-1764.882f, -2398.622f, 351.597f},

	{-1763.261f, -2391.098f, 341.598f},
	{-1758.883f, -2390.536f, 341.195f}
};

class npc_trall_ul : public CreatureScript
{
public:
    npc_trall_ul() : CreatureScript("npc_trall_ul") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (InstanceScript* instance = creature->GetInstanceScript())
        {
            if (instance->GetData(DATA_UL_INTRO2_SAID) == DONE 
			&& instance->GetBossState(BOSS_ULTRAXION) == NOT_STARTED 
			|| instance->GetBossState(BOSS_ULTRAXION) == FAIL)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, TRALL_MENU, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_START_INTRO_2);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }
        
    bool OnGossipSelect(Player *player, Creature *creature, uint32 sender, uint32 uiAction)
    {
        if (sender == GOSSIP_SENDER_MAIN)
        {
            player->PlayerTalkClass->ClearMenus();
            switch(uiAction)
            {
                case GOSSIP_ACTION_START_INTRO_2:
                    player->CLOSE_GOSSIP_MENU();
					if (Creature* dragonsoul = creature->FindNearestCreature(NPC_DRAGON_SOUL, 100.0f, true))
						dragonsoul->AI()->DoAction(ACTION_ATTACK_ON_ULTRAXION);
                    break;
                default:
                    break;
            }
        }
        return true;
    }
};

class npc_the_dragon_soul : public CreatureScript
{
    public:
        npc_the_dragon_soul() : CreatureScript("npc_the_dragon_soul") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_the_dragon_soulAI(creature);
		}
		
		struct npc_the_dragon_soulAI : public ScriptedAI
		{
		    npc_the_dragon_soulAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
				intro1 = false;
				intro2 = false;
				intro3 = false;
				phase1 = false;
				phase2 = false;
			}
			
			InstanceScript* instance;
			EventMap events;
			bool intro1, intro2, intro3, phase1, phase2;

			void Reset()
			{
				events.ScheduleEvent(EVENT_FACTING_TO_OBJ, 1000);
				me->SetPhaseMask(17, true);
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

			void DoAction(int32 actions)
			{
				switch (actions)
				{
                    case ACTION_ATTACK_ON_ULTRAXION:
						deathwing()->AI()->DoAction(ACTION_ATTACK_ON_ULTRAXION);
						break;
					case ACTION_ULTRAXION_ATTACKSTART:
						events.ScheduleEvent(EVENT_LDOZ_TRALL_SAY, 6000);
						break;
					case ACTION_START:
						events.ScheduleEvent(EVENT_INTRO_3_YELL_1, 1000);
						break;
					case ACTION_PHASE_2:
						events.ScheduleEvent(EVENT_PHASE_2_SAY, 11000);
						break;
					case ACTION_END:
						events.ScheduleEvent(EVENT_END_SAY_1, 20000);
						break;
					case ACTION_HOUR_OF_TWILIGHT:
						events.ScheduleEvent(EVENT_HOUR_OF_TWILIGHT, 1000);
						break;
					case ACTION_TWILIGHT_ERUPTION:
						events.ScheduleEvent(EVENT_TWILIGHT_ERUPTION, 1);
						break;
				}
			}

			void EventsStop()
			{
				if (phase1)
				    instance->SetBossState(BOSS_ULTRAXION, FAIL);
				instance->SetData64(DATA64_DRAGON_COUNT, 0);
				instance->SetData64(DATA64_KILL_COUNT, 0);
				if (Creature* ultraxion = me->FindNearestCreature(NPC_ULTRAXION, 1000.0f, true))
					ultraxion->AI()->EnterEvadeMode();
				DespawnCreatures(56249);
				Events(false, false, 1.0f);
				events.ScheduleEvent(EVENT_FACTING_TO_OBJ, 4000);
				phase1 = false;
				phase2 = false;

				// Cancle Events
				events.CancelEvent(EVENT_LDOZ_TRALL_SAY);
				events.CancelEvent(EVENT_LAST_DEFENDER_OF_AZEROTH);
				events.CancelEvent(EVENT_GOL_ALEXSTRASZA_SAY);
				events.CancelEvent(EVENT_GIFT_OF_LIFE);
				events.CancelEvent(EVENT_EOD_YSERA_SAY);
				events.CancelEvent(EVENT_ESSENCE_OF_DREAMS);
				events.CancelEvent(EVENT_SOM_KALECGOS_SAY);
				events.CancelEvent(EVENT_SOURCE_OF_MAGIC);
				events.CancelEvent(EVENT_T_NOZDORMU_SAY);
				events.CancelEvent(EVENT_TIMELOOP);
			}

			void Events(bool startend, bool hot, float speed)
			{
				// Speed
				if (!hot)
				{
				    trall()->SetSpeed(MOVE_RUN, speed);
				    nozdormu()->SetSpeed(MOVE_RUN, speed);
				    ysera()->SetSpeed(MOVE_RUN, speed);
				    alexstrasza()->SetSpeed(MOVE_RUN, speed);
				    kalecgos()->SetSpeed(MOVE_RUN, speed);
				    me->SetSpeed(MOVE_RUN, speed);
				}

				if (startend)
				{
					// Cast
					trall()->AI()->DoCast(me, SPELL_WARD_OF_EARTH, true);
					nozdormu()->AI()->DoCast(me, SPELL_WARD_OF_TIME, true);
					ysera()->AI()->DoCast(me, SPELL_WARD_OF_DREAMS, true);
					alexstrasza()->AI()->DoCast(me, SPELL_WARD_OF_LIFE, true);
					kalecgos()->AI()->DoCast(me, SPELL_WARD_OF_MAGIC, true);
					// Move
					trall()->GetMotionMaster()->MovePoint(0, trall()->GetPositionX(), trall()->GetPositionY(), trall()->GetPositionZ()+2);
					nozdormu()->GetMotionMaster()->MovePoint(0, nozdormu()->GetPositionX(), nozdormu()->GetPositionY(), nozdormu()->GetPositionZ()+2);
					ysera()->GetMotionMaster()->MovePoint(0, ysera()->GetPositionX(), ysera()->GetPositionY(), ysera()->GetPositionZ()+2);
					alexstrasza()->GetMotionMaster()->MovePoint(0, alexstrasza()->GetPositionX(), alexstrasza()->GetPositionY(), alexstrasza()->GetPositionZ()+2);
					kalecgos()->GetMotionMaster()->MovePoint(0, kalecgos()->GetPositionX(), kalecgos()->GetPositionY(), kalecgos()->GetPositionZ()+2);
					me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()+6);
					// Deathwing move
					deathwing()->AI()->DoAction(ACTION_MOVE);
				}
				else
				{
					trall()->RemoveAura(SPELL_WARD_OF_EARTH);
					nozdormu()->RemoveAura(SPELL_WARD_OF_TIME);
					ysera()->RemoveAura(SPELL_WARD_OF_DREAMS);
					alexstrasza()->RemoveAura(SPELL_WARD_OF_LIFE);
					kalecgos()->RemoveAura(SPELL_WARD_OF_MAGIC);
					if (!hot)
					{
					    me->RemoveAllAuras();
					    trall()->GetMotionMaster()->MoveTargetedHome();
					    nozdormu()->GetMotionMaster()->MoveTargetedHome();
					    ysera()->GetMotionMaster()->MoveTargetedHome();
					    alexstrasza()->GetMotionMaster()->MoveTargetedHome();
					    kalecgos()->GetMotionMaster()->MoveTargetedHome();
					    me->GetMotionMaster()->MoveTargetedHome();
					}
				}
			}

			void GetPlayersInCombat()
			{
				uint8 players = 0;
				Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
				if (!PlayerList.isEmpty())
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					    if (Player* player = i->getSource())
						    if (player->isAlive() && !player->isGameMaster() && player->GetAreaId() == 5928)
							    ++players;

				if (players == 0)
					EventsStop();
			}

			bool isTank(Player* tank)
			{
				switch (tank->GetPrimaryTalentTree(tank->GetActiveSpec()))
				{
				case TALENT_TREE_WARRIOR_PROTECTION:
					return true;
					break;
				case TALENT_TREE_PALADIN_PROTECTION:
					return true;
					break;
				case TALENT_TREE_DEATH_KNIGHT_BLOOD:
					return true;
					break;
				case TALENT_TREE_DRUID_FERAL_COMBAT:
					return true;
					break;
				default:
					return false;
					break;
				}
				return true;
			}
			
			void UpdateAI(uint32 diff)
			{
				if (Player* target = me->FindNearestPlayer(20.0f, true))
				{
					if (target->GetDistance(me) <= 20.0f)
					{
						if (instance->GetData(DATA_UL_INTRO1_SAID) == NOT_STARTED 
							&& instance->GetBossState(BOSS_WARLORD) == DONE 
							&& instance->GetBossState(BOSS_UNSLEEPING) == DONE && !intro1)
						{
							intro1 = true;
							events.ScheduleEvent(EVENT_INTRO_1_SAY_1, 10000);
						}

						if (instance->GetData(DATA_UL_INTRO2_SAID) == NOT_STARTED 
							&& instance->GetBossState(BOSS_HAGARA) == DONE && !intro2)
						{
							intro2 = true;
							events.ScheduleEvent(EVENT_INTRO_2_SAY_1, 30000);
						}
					}
				}
				
				if (instance->GetData(DATA_UL_INTRO2_SAID) == DONE && !intro3 
					&& instance->GetBossState(BOSS_ULTRAXION) == NOT_STARTED 
					|| instance->GetBossState(BOSS_ULTRAXION) == FAIL)
				{
				    if (!trall()->HasAura(108492))
				        trall()->AI()->DoCast(trall(), 108492, true);
					if (!alexstrasza()->HasAura(108490))
				        alexstrasza()->AI()->DoCast(alexstrasza(), 108490, true);
					if (!ysera()->HasAura(108494))
				        ysera()->AI()->DoCast(ysera(), 108494, true);
					if (!nozdormu()->HasAura(108493))
				        nozdormu()->AI()->DoCast(nozdormu(), 108493, true);
					if (!kalecgos()->HasAura(108491))
				        kalecgos()->AI()->DoCast(kalecgos(), 108491, true);
				}

				if (instance->GetData64(DATA64_KILL_COUNT) >= TwhilighAssaulterCount)
				{
					instance->SetData64(DATA64_KILL_COUNT, 0);
					deathwing()->AI()->DoAction(ACTION_PHASE_2);
				}

				if (phase1 || phase2)
				    GetPlayersInCombat();

				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					// Other
					case EVENT_FACTING_TO_OBJ:
						trall()->SetFacingToObject(me);
						nozdormu()->SetFacingToObject(me);
						ysera()->SetFacingToObject(me);
						alexstrasza()->SetFacingToObject(me);
						kalecgos()->SetFacingToObject(me);
						break;
					// Events Intro 1
					case EVENT_INTRO_1_SAY_1:
						trall()->AI()->Talk(0); // 25904
						events.ScheduleEvent(EVENT_INTRO_1_SAY_2, 15000);
						break;
					case EVENT_INTRO_1_SAY_2:
						alexstrasza()->AI()->Talk(0); // 26503
						events.ScheduleEvent(EVENT_INTRO_1_SAY_3, 5000);
						break;
					case EVENT_INTRO_1_SAY_3:
						kalecgos()->AI()->Talk(0); // 26547
						events.ScheduleEvent(EVENT_INTRO_1_SAY_4, 10000);
						break;
					case EVENT_INTRO_1_SAY_4:
						kalecgos()->AI()->Talk(1); // 26265
						events.ScheduleEvent(EVENT_INTRO_1_SAY_5, 10000);
						break;
					case EVENT_INTRO_1_SAY_5:
						trall()->AI()->Talk(1); // 25905
						events.ScheduleEvent(EVENT_INTRO_1_SAY_6, 10000);
						break;
					case EVENT_INTRO_1_SAY_6:
						ysera()->AI()->Talk(2); // 26147
						events.ScheduleEvent(EVENT_INTRO_1_SAY_7, 10000);
						break;
					case EVENT_INTRO_1_SAY_7:
						trall()->AI()->Talk(2); // 25906
						events.ScheduleEvent(EVENT_INTRO_1_SAY_8, 7000);
						break;
					case EVENT_INTRO_1_SAY_8:
						kalecgos()->AI()->Talk(2); // 26266
						events.ScheduleEvent(EVENT_INTRO_1_SAY_9, 13000);
						break;
					case EVENT_INTRO_1_SAY_9:
						instance->SetData(DATA_UL_INTRO1_SAID, DONE);
						break;
					// Events Intro 2
					case EVENT_INTRO_2_SAY_1:
						ysera()->AI()->Talk(0); // 26145
						events.ScheduleEvent(EVENT_INTRO_2_SAY_2, 5000);
						break;
					case EVENT_INTRO_2_SAY_2:
						nozdormu()->AI()->Talk(0); // 25953
						events.ScheduleEvent(EVENT_INTRO_2_SAY_3, 3000);
						break;
					case EVENT_INTRO_2_SAY_3:
						kalecgos()->AI()->Talk(3); // 26262
						events.ScheduleEvent(EVENT_INTRO_2_RITUAL_1, 5000);
						events.ScheduleEvent(EVENT_INTRO_2_SAY_4, 10000);
						break;
					case EVENT_INTRO_2_RITUAL_1:
						ysera()->AI()->DoCast(me, 108494, true);
						nozdormu()->AI()->DoCast(me, 108493, true);
						kalecgos()->AI()->DoCast(me, 108491, true);
						break;
					case EVENT_INTRO_2_SAY_4:
						alexstrasza()->AI()->Talk(2); // 26504
						events.ScheduleEvent(EVENT_INTRO_2_RITUAL_2, 5000);
						events.ScheduleEvent(EVENT_INTRO_2_SAY_5, 10000);
						break;
					case EVENT_INTRO_2_RITUAL_2:
						alexstrasza()->AI()->DoCast(me, 108490, true);
						break;
					case EVENT_INTRO_2_SAY_5:
						trall()->AI()->Talk(3); // 25898
						events.ScheduleEvent(EVENT_INTRO_2_SAY_6, 8000);
						break;
					case EVENT_INTRO_2_SAY_6:
						trall()->AI()->Talk(4); // 25899
						events.ScheduleEvent(EVENT_INTRO_2_SAY_7, 8000);
						break;
					case EVENT_INTRO_2_SAY_7:
						trall()->AI()->Talk(5);  // 25900
						events.ScheduleEvent(EVENT_INTRO_2_RITUAL_3, 5000);
						break;
					case EVENT_INTRO_2_RITUAL_3:
						trall()->AI()->DoCast(me, 108492, true);
						instance->SetData(DATA_UL_INTRO2_SAID, DONE);
						break;
					case EVENT_INTRO_3_YELL_1:
						deathwing()->AI()->Talk(0);
						events.ScheduleEvent(EVENT_INTRO_3_YELL_2, 10000);
						break;
					case EVENT_INTRO_3_YELL_2:
						for (int8 i = 0; i < TwhilighAssaulterCount; i++)
						    deathwing()->SummonCreature(56249, TwilightAssaulter[i], TEMPSUMMON_MANUAL_DESPAWN);
						deathwing()->AI()->Talk(1);
						events.ScheduleEvent(EVENT_INTRO_3_SAY, 18000);
						break;
					case EVENT_INTRO_3_SAY:
						alexstrasza()->AI()->Talk(3);
						events.ScheduleEvent(EVENT_ULTRAXION_1, 5000);
						break;
					case EVENT_PHASE_2_SAY:
						ysera()->AI()->Talk(1);
						events.ScheduleEvent(EVENT_ULTRAXION_3, 10000);
						break;
					case EVENT_ULTRAXION_1:
						intro3 = true;
						Events(true, false, 0.1f);
						events.ScheduleEvent(EVENT_ULTRAXION_2, 10000);
						break;
					case EVENT_ULTRAXION_2:
					{
						phase1 = true;
						instance->SetData64(DATA64_DRAGON_COUNT, 0);
						instance->SetData64(DATA64_KILL_COUNT, 0);
						std::list<Creature*> creatures;
						GetCreatureListWithEntryInGrid(creatures, me, 56249, 1000.0f);
						for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
							(*iter)->AI()->DoAction(ACTION_ATTACK);
						break;
					}
					case EVENT_ULTRAXION_3:
						phase1 = false;
						phase2 = true;
						me->SummonCreature(NPC_ULTRAXION, -1616.576f, -2369.113f, 311.776f, 3.33f, TEMPSUMMON_MANUAL_DESPAWN);
						break;
					case EVENT_LDOZ_TRALL_SAY:
						trall()->AI()->Talk(6);
						events.ScheduleEvent(EVENT_LAST_DEFENDER_OF_AZEROTH, 3000);
						break;
					case EVENT_LAST_DEFENDER_OF_AZEROTH:
					{
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
					    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    if (Player* tank = i->getSource())
								if (tank->GetAreaId() == 5928 && isTank(tank) && tank->isAlive())
									trall()->AI()->DoCast(tank, SPELL_LAST_DEFENDER_OF_AZEROTH, true);
						events.ScheduleEvent(EVENT_GOL_ALEXSTRASZA_SAY, 67000);
						break;
					}
					case EVENT_GOL_ALEXSTRASZA_SAY:
						alexstrasza()->AI()->Talk(4); // 26506
						events.ScheduleEvent(EVENT_GIFT_OF_LIFE, 3000);
						break;
					case EVENT_GIFT_OF_LIFE:
						alexstrasza()->CastSpell(me->getVictim(), SPELL_GIFT_OF_LIFE, true);
						if (Is25ManRaid())
							events.ScheduleEvent(EVENT_GIFT_OF_LIFE_2, 1000);
					    events.ScheduleEvent(EVENT_EOD_YSERA_SAY, 70000);
						break;
					case EVENT_GIFT_OF_LIFE_2:
						alexstrasza()->CastSpell(me->getVictim(), SPELL_GIFT_OF_LIFE, true);
						break;
					case EVENT_EOD_YSERA_SAY:
						ysera()->AI()->Talk(3);
						events.ScheduleEvent(EVENT_ESSENCE_OF_DREAMS, 3000);
						break;
					case EVENT_ESSENCE_OF_DREAMS:
						ysera()->CastSpell(me->getVictim(), SPELL_ESSENCE_OF_DREAMS, true);
						if (Is25ManRaid())
							events.ScheduleEvent(EVENT_ESSENCE_OF_DREAMS_2, 1000);
					    events.ScheduleEvent(EVENT_SOM_KALECGOS_SAY, 57000);
						break;
					case EVENT_ESSENCE_OF_DREAMS_2:
						ysera()->CastSpell(me->getVictim(), SPELL_ESSENCE_OF_DREAMS, true);
						break;
					case EVENT_SOM_KALECGOS_SAY:
						kalecgos()->AI()->Talk(4);
						events.ScheduleEvent(EVENT_SOURCE_OF_MAGIC, 3000);
						break;
					case EVENT_SOURCE_OF_MAGIC:
						kalecgos()->CastSpell(me->getVictim(), SPELL_SOURCE_OF_MAGIC, true);
						if (Is25ManRaid())
							events.ScheduleEvent(EVENT_SOURCE_OF_MAGIC_2, 1000);
					    events.ScheduleEvent(EVENT_T_NOZDORMU_SAY, 60000);
						break;
					case EVENT_SOURCE_OF_MAGIC_2:
						kalecgos()->CastSpell(me->getVictim(), SPELL_SOURCE_OF_MAGIC, true);
						break;
					case EVENT_T_NOZDORMU_SAY:
						nozdormu()->AI()->Talk(1);
						events.ScheduleEvent(EVENT_TIMELOOP, 3000);
						break;
					case EVENT_TIMELOOP:
					{
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();
					    for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    if (Player* player = i->getSource())
								if (player->GetAreaId() == 5928 && player->isAlive())
									nozdormu()->AddAura(SPELL_TIMELOOP, player);
						break;
					}
					case EVENT_END_SAY_1:
						Events(false, false, 1.0f);
						DoCast(110489); // Dragon Soul Cosmetic(charged)
						alexstrasza()->AI()->Talk(5); // 26505
						events.ScheduleEvent(EVENT_END_SAY_2, 10000);
						break;
					case EVENT_END_SAY_2:
						trall()->AI()->Talk(7); // 25908
						events.ScheduleEvent(EVENT_END_MOVE, 8000);
						break;
					case EVENT_END_MOVE:
						trall()->GetMotionMaster()->MovePoint(0, -1789.449f, -2392.185f, 341.354f);
						events.ScheduleEvent(EVENT_END_MOVIE, 5000);
						break;
					case EVENT_END_MOVIE:
					{
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

						if (PlList.isEmpty())
						    return;

						for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    if (Player* player = i->getSource())
								player->SendMovieStart(MOVIE_ACT_1);

						events.ScheduleEvent(EVENT_END_TELEPORT, 22000);
						break;
					}
					case EVENT_END_TELEPORT:
					{
						me->SummonCreature(NPC_TRAVEL_TO_THE_DECK_OF_THE_SKYFIRE, -1802.141f, -2364.638f, 340.796f, 5.234f, TEMPSUMMON_MANUAL_DESPAWN);
						instance->SetData(DATA_ACT_1, DONE);
						Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

						if (PlList.isEmpty())
						    return;

						for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
						    if (Player* player = i->getSource())
								player->TeleportTo(967, 13397.298f, -12131.405f, 153.8732f, 3.152f);
						break;
					}

					// Combat
					case EVENT_HOUR_OF_TWILIGHT:
						Events(false, true, 1.0f);
						break;
					case EVENT_TWILIGHT_ERUPTION:
						EventsStop();
						break;
					}
				}
			}

			Creature* trall()
			{
			    return me->FindNearestCreature(NPC_TRALL, 100.0f, true);
			}

			Creature* nozdormu()
			{
			    return me->FindNearestCreature(NPC_NOZDORMU, 100.0f, true);
			}

			Creature* ysera()
			{
			    return me->FindNearestCreature(NPC_YSERA, 100.0f, true);
			}

			Creature* alexstrasza()
			{
			    return me->FindNearestCreature(NPC_ALEXSTRASZA, 100.0f, true);
			}

			Creature* kalecgos()
			{
			    return me->FindNearestCreature(NPC_ASPECT_OF_MAGIC, 100.0f, true);
			}

			Creature* deathwing()
			{
			    return me->FindNearestCreature(NPC_WYRMREST_DEATHWING, 10000.0f, true);
			}
		};
};

Position const MovePositions[5] =
{
    {-1838.178f, -2130.049f, 366.027f}, // move 1
	{-1593.372f, -2180.85f, 366.511f}, // move 2
    {-1511.902f, -2369.759f, 385.544f}, // move 3
	{-1652.401f, -2378.151f, 369.274f}, // Event
	{-1552.229f, -2378.484f, 369.412f} // event
};

enum DWSpells
{
	SPELL_FLAME_BREATH = 105392
};

enum DWEvents
{
	EVENT_FLAME_BREATH = 1,
	EVENT_FACTING_TO,
	EVENT_MOVE_1,
	EVENT_MOVE_2,
	EVENT_MOVE_3,
	EVENT_PHASE_2_YELL_1,
	EVENT_PHASE_2_YELL_2,
	EVENT_PHASE_2_YELL_3
};

class npc_ul_deathwing : public CreatureScript
{
    public:
        npc_ul_deathwing() : CreatureScript("npc_ul_deathwing") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_ul_deathwingAI(creature);
		}
		
		struct npc_ul_deathwingAI : public ScriptedAI
		{
		    npc_ul_deathwingAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }
			
			InstanceScript* instance;
			EventMap events;
			bool move1, move2, move2_1, move3;

			void Reset()
			{
				me->SetSpeed(MOVE_FLIGHT, 4.0f);
				events.ScheduleEvent(EVENT_FLAME_BREATH, 5000);
				events.ScheduleEvent(EVENT_MOVE_1, 1000);
				move1 = true;
				move2 = false;
				move2_1 = false;
				move3 = false;
			}

			void DoAction(int32 actions)
			{
				switch (actions)
				{
                    case ACTION_ATTACK_ON_ULTRAXION:
						instance->SetBossState(BOSS_ULTRAXION, IN_PROGRESS);
						events.CancelEvent(EVENT_FLAME_BREATH);
						events.CancelEvent(EVENT_MOVE_1);
						events.CancelEvent(EVENT_MOVE_2);
						events.CancelEvent(EVENT_MOVE_3);
						me->StopMoving();
						
						me->GetMotionMaster()->MovePoint(3, MovePositions[3]);
						break;
					case ACTION_MOVE:
						events.ScheduleEvent(EVENT_MOVE_1, 2000);
						events.ScheduleEvent(EVENT_FLAME_BREATH, 7000);
						break;
					case ACTION_PHASE_2:
						events.CancelEvent(EVENT_FLAME_BREATH);
						events.CancelEvent(EVENT_MOVE_1);
						events.CancelEvent(EVENT_MOVE_2);
						events.CancelEvent(EVENT_MOVE_3);
						me->StopMoving();
						me->SetFacingToObject(dragonsoul());
						Talk(2);
						events.ScheduleEvent(EVENT_PHASE_2_YELL_1, 15000);
						break;
				}
			}

			void MovementInform(uint32 type, uint32 pointId)
            {
				switch (pointId)
				{
				case 3:
					dragonsoul()->AI()->DoAction(ACTION_START);
					//me->SetFacingTo(3.211f); // Does not work. Fix me
					events.ScheduleEvent(EVENT_FACTING_TO, 1000);
					break;
				}
			}
			
			void UpdateAI(uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_FLAME_BREATH:
						DoCast(SPELL_FLAME_BREATH);
						events.ScheduleEvent(EVENT_FLAME_BREATH, urand(5000, 30000));
						break;
					case EVENT_FACTING_TO:
						me->SetFacingTo(3.211f);
						break;
					case EVENT_MOVE_1:
						me->GetMotionMaster()->MovePoint(0, MovePositions[0]);
						move1 = true;
						events.ScheduleEvent(EVENT_MOVE_2, 20000);
						break;
					case EVENT_MOVE_2:
						me->GetMotionMaster()->MovePoint(1, MovePositions[1]);
						if (!move2)
						{
							move1 = false;
							move2 = true;
						    events.ScheduleEvent(EVENT_MOVE_3, 20000);
						}
						else
						{
							move2 = false;
							move2_1 = true;
							move3 = false;
							events.ScheduleEvent(EVENT_MOVE_1, 20000);
						}
						break;
					case EVENT_MOVE_3:
						move3 = true;
						me->GetMotionMaster()->MovePoint(2, MovePositions[2]);
						events.ScheduleEvent(EVENT_MOVE_2, 20000);
						break;
					case EVENT_PHASE_2_YELL_1:
						Talk(3);
						events.ScheduleEvent(EVENT_PHASE_2_YELL_2, 13000);
						break;
					case EVENT_PHASE_2_YELL_2:
						Talk(4);
						dragonsoul()->AI()->DoAction(ACTION_PHASE_2);
						events.ScheduleEvent(EVENT_MOVE_1, 9000);
						events.ScheduleEvent(EVENT_FLAME_BREATH, 14000);
						break;
					}
				}
			}

			Creature* dragonsoul()
			{
			    return me->FindNearestCreature(NPC_DRAGON_SOUL, 1000.0f, true);
			}
		};
};

enum TASpells
{
	SPELL_TWILIGHT_BREATH = 105858,
	SPELL_TWILIGHT_FLAMES = 105579
};

enum TAEvents
{
	EVENT_ATTACK = 1,
	EVENT_TWILIGHT_BREATH,
};

class npc_twilight_assaulter : public CreatureScript
{
    public:
        npc_twilight_assaulter() : CreatureScript("npc_twilight_assaulter") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_twilight_assaulterAI(creature);
		}
		
		struct npc_twilight_assaulterAI : public ScriptedAI
		{
		    npc_twilight_assaulterAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }
			
			InstanceScript* instance;
			EventMap events;
			bool update;

			void Reset()
			{
				me->SetSpeed(MOVE_FLIGHT, 4.0f);
				if (roll_chance_i(80))
				    me->GetMotionMaster()->MoveRandom(urand(5.0f, 25.0f));
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				update = false;
			}

			void EnterCombat(Unit* /*who*/)
			{
				events.ScheduleEvent(EVENT_TWILIGHT_BREATH, 15000);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			}

			void DoAction(int32 actions)
			{
				switch (actions)
				{
				case ACTION_ATTACK:
					events.ScheduleEvent(EVENT_ATTACK, urand(1000, 5000));
					break;
				}
			}
			
			void UpdateAI(uint32 diff)
			{
				events.Update(diff);

				if (update && instance->GetData64(DATA64_DRAGON_COUNT) <= 2)
				{
					update = false;
					uint64 count = instance->GetData64(DATA64_DRAGON_COUNT);
					instance->SetData64(DATA64_DRAGON_COUNT, count + 1);
					me->SetInCombatWithZone();
					if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						AttackStart(target);
					if (roll_chance_i(50))
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						    DoCast(target, SPELL_TWILIGHT_FLAMES, true);
				}

				while (uint32 eventId = events.ExecuteEvent())
				{
					switch (eventId)
					{
					case EVENT_ATTACK:
						if (instance->GetData64(DATA64_DRAGON_COUNT) <= 2)
						{
							uint64 count = instance->GetData64(DATA64_DRAGON_COUNT);
							instance->SetData64(DATA64_DRAGON_COUNT, count + 1);
							me->SetInCombatWithZone();
							if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
								AttackStart(target);
							if (roll_chance_i(50))
						        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
						            DoCast(target, SPELL_TWILIGHT_FLAMES, true);
						}
						else
							update = true;
						break;
					case EVENT_TWILIGHT_BREATH:
						if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
							DoCast(target, SPELL_TWILIGHT_BREATH);
						events.ScheduleEvent(EVENT_TWILIGHT_BREATH, urand(2500, 15000));
						break;
					}
				}

				DoMeleeAttackIfReady();
			}

			void JustDied(Unit* /*killer*/)
			{
				uint64 count = instance->GetData64(DATA64_KILL_COUNT);
				instance->SetData64(DATA64_KILL_COUNT, count + 1);
				uint64 dragoncount = instance->GetData64(DATA64_DRAGON_COUNT);
				instance->SetData64(DATA64_DRAGON_COUNT, dragoncount - 1);
				me->DespawnOrUnsummon(5000);
			}
		};
};

void AddSC_boss_ultraxion()
{
	new boss_ultraxion();
	new npc_the_dragon_soul();
	new npc_trall_ul();
	new npc_ul_deathwing();
	new npc_twilight_assaulter();
	new spell_ds_unstable_monstrosity();
	new spell_ds_fading_light();
}