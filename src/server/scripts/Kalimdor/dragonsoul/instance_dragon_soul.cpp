/* ScriptData
SDName: Instance dragon soul
SD%Complete: ~50%
SDComment:
SDCategory: Instance
EndScriptData
*/

#include "ScriptPCH.h"
#include "dragon_soul.h"
#include "Map.h"

class instance_dragon_soul: public InstanceMapScript
{
public:
    instance_dragon_soul() : InstanceMapScript("instance_dragon_soul", 967) { }

    struct instance_dragon_soul_InstanceMapScript : public InstanceScript
    {
        instance_dragon_soul_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
        }
        // Creatures
        uint64 MorchokGUID;
        uint64 UnsleepingGUID;
        uint64 WarlordGUID;
        uint64 HagaraGUID;
        uint64 UltraxionGUID;
        uint64 WarmasterGUID;
		
		// Go
		uint64 GoTheFocusingLrisGUID;
		
		// Portals
        uint64 PortalGUID[4];

        uint64 Maelstrom_trall;
        uint64 Maelstrom_kalecgos;
        uint64 Maelstrom_ysera;
        uint64 Maelstrom_nozdormy;
        uint64 Maelstrom_alexstrasza;

		// Aspects && DragonSoul && Deathwing
        uint64 Aspect_Of_MagicGUID;
        uint64 AlexstraszaGUID;
        uint64 YseraGUID;
        uint64 NozdormuGUID;
        uint64 TrallGUID;
		uint64 DragonSoulGUID;
		uint64 WyrmrestDeathwingGUID;

		// Deathwing
		uint64 DeathwingGUID;
		uint64 arm_tentacle_1;
		uint64 arm_tentacle_2;
		uint64 wing_tentacle_1;
		uint64 wing_tentacle_2;
		
		// Datas
		uint32 PortalData;
		uint32 DSULIntro1;
		uint32 DSULIntro2;
		
		// Datas64
		uint64 KillCount;
		uint64 DragonCount;
		
		// Acts
		uint32 act_1;

        void Initialize()
        {
            SetBossNumber(EncounterCount);
			// boss
            MorchokGUID = 0;
            UnsleepingGUID = 0;
            WarlordGUID = 0;
            HagaraGUID = 0;
            UltraxionGUID = 0;
            WarmasterGUID = 0;
			
			// portals
            PortalGUID[0] = 0;
			PortalGUID[1] = 0;
			PortalGUID[2] = 0;
			PortalGUID[3] = 0;
			PortalGUID[4] = 0;

			// npc
            Maelstrom_trall = 0;
            Maelstrom_kalecgos = 0;
            Maelstrom_ysera = 0;
            Maelstrom_nozdormy = 0;
            Maelstrom_alexstrasza = 0;

			// Aspects && DragonSoul && Deathwing
            Aspect_Of_MagicGUID = 0;
            AlexstraszaGUID = 0;
            YseraGUID = 0;
            NozdormuGUID = 0;
            TrallGUID = 0;
			DragonSoulGUID = 0;
			WyrmrestDeathwingGUID = 0;

			// Deathwing
			DeathwingGUID = 0;
		    arm_tentacle_1 = 0;
			arm_tentacle_2 = 0;
			wing_tentacle_1 = 0;
			wing_tentacle_2 = 0;
			
			// Go
			GoTheFocusingLrisGUID = 0;
			
			// Datas
			PortalData = DONE;
			DSULIntro1 = NOT_STARTED;
			DSULIntro2 = NOT_STARTED;
			
			// Datas64
			KillCount = 0;
			DragonCount = 0;
			
			// Acts
			act_1 = NOT_STARTED;

			// Set State
			SetBossState(BOSS_HAGARA, NOT_STARTED);
			SetBossState(BOSS_ULTRAXION, NOT_STARTED);
        }
		
		void OnPlayerEnter(Player* player)
		{
		    if (GetBossState(BOSS_ULTRAXION) == NOT_STARTED || GetBossState(BOSS_ULTRAXION) == FAIL || GetBossState(BOSS_ULTRAXION) == DONE)
			{
			    // other
                DoRemoveAurasDueToSpellOnPlayers(106368);
                DoRemoveAurasDueToSpellOnPlayers(105554);
				DoRemoveAurasDueToSpellOnPlayers(106498);
				DoRemoveAurasDueToSpellOnPlayers(109075);
				DoRemoveAurasDueToSpellOnPlayers(105927);
				// Gifts
				DoRemoveAurasDueToSpellOnPlayers(106080);
				DoRemoveAurasDueToSpellOnPlayers(105896);
				DoRemoveAurasDueToSpellOnPlayers(105900);
				DoRemoveAurasDueToSpellOnPlayers(105903);
				DoRemoveAurasDueToSpellOnPlayers(105984);
			}
		}

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
            case NPC_MORCHOK: 
                MorchokGUID = creature->GetGUID(); 
                break;
            case NPC_WARLORD: 
                UnsleepingGUID = creature->GetGUID(); 
                break;
            case NPC_UNSLEEPING: 
                WarlordGUID = creature->GetGUID(); 
                break;
            case NPC_HAGARA: 
                HagaraGUID = creature->GetGUID(); 
                break;
            case NPC_ULTRAXION: 
                UltraxionGUID = creature->GetGUID(); 
                break;
            case NPC_WARMASTER:
                WarmasterGUID = creature->GetGUID(); 
                break;
            case NPC_TRALL: 
                TrallGUID = creature->GetGUID(); 
                break;
            case NPC_ALEXSTRASZA: 
                AlexstraszaGUID = creature->GetGUID(); 
                break;
            case NPC_YSERA: 
                YseraGUID = creature->GetGUID(); 
                break;
            case NPC_NOZDORMU: 
                NozdormuGUID = creature->GetGUID(); 
                break;
            case NPC_ASPECT_OF_MAGIC: 
                Aspect_Of_MagicGUID = creature->GetGUID(); 
                break;
			case NPC_DRAGON_SOUL:
				DragonSoulGUID = creature->GetGUID(); 
				break;
			case NPC_WYRMREST_DEATHWING:
				WyrmrestDeathwingGUID = creature->GetGUID(); 
				break;
            case NPC_MAELSTROM_TRALL: 
                Maelstrom_trall = creature->GetGUID(); 
                break;
            case NPC_MAELSTROM_KALECGOS: 
                Maelstrom_kalecgos = creature->GetGUID(); 
                break;
            case NPC_MAELSTROM_YSERA: 
                Maelstrom_ysera = creature->GetGUID(); 
                break;
            case NPC_MAELSTROM_NOZDORMY: 
                Maelstrom_nozdormy = creature->GetGUID(); 
                break;
            case NPC_MAELSTROM_ALEXSTRASZA: 
                Maelstrom_alexstrasza = creature->GetGUID(); 
				break;
			case NPC_DEATHWING_1:
				DeathwingGUID = creature->GetGUID();
                break;
			case NPC_ARM_TENTACLE_1:
				arm_tentacle_1 = creature->GetGUID();
                break;
			case NPC_ARM_TENTACLE_2:
				arm_tentacle_2 = creature->GetGUID();
                break;
			case NPC_WING_TENTACLE_1:
				wing_tentacle_1 = creature->GetGUID();
                break;
			case NPC_WING_TENTACLE_2:
				wing_tentacle_2 = creature->GetGUID();
                break;
			// portals
            case NPC_TRAVEL_TO_THE_DECK_OF_THE_SKYFIRE: 
                PortalGUID[0] = creature->GetGUID(); 
				creature->SetObjectScale(5.0f);
				creature->SetVisible(false);
				break;
			case NPC_TRAVEL_TO_WYRMREST_SUMMIT: 
                PortalGUID[1] = creature->GetGUID();
				creature->SetObjectScale(5.0f);
                break;
			case NPC_TRAVEL_TO_THE_MAELSTROM: 
                PortalGUID[2] = creature->GetGUID();
				creature->SetObjectScale(5.0f);
                break;	
			case NPC_PORTAL: 
                PortalGUID[3] = creature->GetGUID();
				creature->SetObjectScale(5.0f);
                break;
			case NPC_PORTAL_THE_EYE_OF_ETERNITY:
			    PortalGUID[4] = creature->GetGUID();
			    creature->SetObjectScale(0.1f);
			    creature->SetVisible(false);
                break;
            }
        }
		
		void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
			case GO_THE_FOCUSING_LRIS:
			    GoTheFocusingLrisGUID = go->GetGUID();
				if (GetBossState(BOSS_HAGARA) == NOT_STARTED || GetBossState(BOSS_HAGARA) == FAIL)
				    go->SetPhaseMask(1, true);
				else
					go->SetPhaseMask(2, true);
				break;
			}
        }

		void SetData(uint32 type, uint32 data)
		{
			switch (type)
			{
			case DATA_PORTALS_ON_OFF:
			    PortalData = data;
				switch (data)
				{
				case IN_PROGRESS:
				    for (uint8 i = 1; i < 3; ++i)
				        if (Creature* creature = instance->GetCreature(PortalGUID[i]))
					        creature->SetObjectScale(0.1f);
					break;
				case DONE:
					for (uint8 i = 1; i < 3; ++i)
				        if (Creature* creature = instance->GetCreature(PortalGUID[i]))
					        creature->SetObjectScale(5.0f);
					break;
				}
			    break;
			case DATA_DAMAGE_DEATHWING:
				if(data == DONE)
					if(Creature* creature = instance->GetCreature(DeathwingGUID))
						creature->CastSpell(creature, 106548);
				SaveToDB();
				break;
			case DATA_ATTACK_DEATHWING:
				if (data == IN_PROGRESS)
				{
					if(Creature* creature = instance->GetCreature(arm_tentacle_1))
					{
						creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
						creature->SetVisible(true);
					}
					
					if(Creature* creature = instance->GetCreature(arm_tentacle_2))
					{
						creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
						creature->SetVisible(true);
					}
					
					if(Creature* creature = instance->GetCreature(wing_tentacle_1))
					{
						creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
						creature->SetVisible(true);
					}
					
					if(Creature* creature = instance->GetCreature(wing_tentacle_2))
					{
						creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
						creature->SetVisible(true);
					}
				}
				SaveToDB();
				break;
			case DATA_UL_INTRO1_SAID:
				DSULIntro1 = data;
				switch (data)
				{
					case NOT_STARTED:
					{
						if (Creature* creature = instance->GetCreature(PortalGUID[4]))
						{
							creature->SetObjectScale(0.1f);
							creature->SetVisible(false);
						}
						break;
					}
					case DONE:
					{
						if (GetBossState(BOSS_HAGARA) == NOT_STARTED || GetBossState(BOSS_HAGARA) == FAIL)
							if (Creature* creature = instance->GetCreature(PortalGUID[4]))
					        {
								creature->SetObjectScale(5.0f);
								creature->SetVisible(true);
					        }
						break;
					}
				}
				SaveToDB();
			    break;
			case DATA_UL_INTRO2_SAID:
			    DSULIntro2 = data;
			    SaveToDB();
			    break;
			case DATA_ACT_1:
			    act_1 = data;
				switch (data)
				{
				case NOT_STARTED:
					if (Creature* kalecgos = instance->GetCreature(Aspect_Of_MagicGUID))
					    kalecgos->SetVisible(true);
					if (Creature* alexstrasza = instance->GetCreature(AlexstraszaGUID))
					    alexstrasza->SetVisible(true);
					if (Creature* ysera = instance->GetCreature(YseraGUID))
					    ysera->SetVisible(true);
					if (Creature* nozdormu = instance->GetCreature(NozdormuGUID))
					    nozdormu->SetVisible(true);
					if (Creature* trall = instance->GetCreature(TrallGUID))
					    trall->SetVisible(true);
					if (Creature* dragonsoul = instance->GetCreature(DragonSoulGUID))
					    dragonsoul->SetVisible(true);
					if (Creature* portal_to_skyfire = instance->GetCreature(PortalGUID[0]))
					    portal_to_skyfire->SetVisible(false);
					break;
				case DONE:
					if (Creature* kalecgos = instance->GetCreature(Aspect_Of_MagicGUID))
					    kalecgos->SetVisible(false);
					if (Creature* alexstrasza = instance->GetCreature(AlexstraszaGUID))
					    alexstrasza->SetVisible(false);
					if (Creature* ysera = instance->GetCreature(YseraGUID))
					    ysera->SetVisible(false);
					if (Creature* nozdormu = instance->GetCreature(NozdormuGUID))
					    nozdormu->SetVisible(false);
					if (Creature* trall = instance->GetCreature(TrallGUID))
					    trall->SetVisible(false);
					if (Creature* dragonsoul = instance->GetCreature(DragonSoulGUID))
					    dragonsoul->SetVisible(false);
					if (Creature* portal_to_skyfire = instance->GetCreature(PortalGUID[0]))
					    portal_to_skyfire->SetVisible(true);
					break;
				}
				SaveToDB();
				break;
			default:
				break;
			}
		}
		
		void SetData64(uint32 type, uint64 count)
		{
			switch (type)
			{
                case DATA64_KILL_COUNT:
                    KillCount = count;
                    break;
				case DATA64_DRAGON_COUNT:
				    DragonCount = count;
					break;
                default:
                    break;
			}
		}
		
		uint32 GetData(uint32 type) const
        {
            switch(type)
            {
                case DATA_PORTALS_ON_OFF:
                    return PortalData;
					break;
				case DATA_UL_INTRO1_SAID:
                    return DSULIntro1;
					break;
				case DATA_UL_INTRO2_SAID:
				    return DSULIntro2;
					break;
				case DATA_ACT_1:
				    return act_1;
					break;
            }
			
            return 0;
        }
		
		uint64 GetData64(uint32 type) const
		{
            switch(type)
            {
				case DATA64_KILL_COUNT:
				    return KillCount;
					break;
				case DATA64_DRAGON_COUNT:
				    return DragonCount;
					break;
            }
			
            return 0;
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch(type)
            {
                case BOSS_MORCHOK:
                case BOSS_WARLORD:
                case BOSS_UNSLEEPING:
					break;
                case BOSS_HAGARA:
					if (state == FAIL)
                        if (GameObject* go = instance->GetGameObject(GoTheFocusingLrisGUID))
						    go->SetPhaseMask(1, true);
					break;
                case BOSS_ULTRAXION:
					if (state == DONE)
						if (Creature* deathwing = instance->GetCreature(WyrmrestDeathwingGUID))
							deathwing->SetVisible(false);
					break;
				case BOSS_WARMASTER:
                case BOSS_SPINE_OF_DEATHWING:
				case BOSS_MADNESS_OF_DEATHWING:
					break;
            }

            return true;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "D S " << GetBossSaveData() << ' ' << DSULIntro1 << ' ' << DSULIntro2 << ' ' << act_1;

            OUT_SAVE_INST_DATA_COMPLETE;
            return saveStream.str();
        }

        void Load(const char* str)
        {
            if (!str)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(str);

            char dataHead1, dataHead2;

            std::istringstream loadStream(str);
            loadStream >> dataHead1 >> dataHead2;

            if (dataHead1 == 'D' && dataHead2 == 'S')
            {
                for (uint32 i = 0; i < EncounterCount; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;

                    SetBossState(i, EncounterState(tmpState));
                }

				loadStream >> DSULIntro1;
				SetData(DATA_UL_INTRO1_SAID, DSULIntro1);
				loadStream >> DSULIntro2;
				SetData(DATA_UL_INTRO2_SAID, DSULIntro2);
				loadStream >> act_1;
				SetData(DATA_ACT_1, act_1);
            } 
			else 
				OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_dragon_soul_InstanceMapScript(map);
    }
};

void AddSC_instance_dragon_soul()
{
    new instance_dragon_soul();
}