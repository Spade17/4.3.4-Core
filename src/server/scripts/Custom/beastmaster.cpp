#include "ScriptPCH.h"
#include "Pet.h"

class beastmaster :  public CreatureScript
{
	public: 
		beastmaster() : CreatureScript("beastmaster"){ }

									
		void CreatePet(Player *player, Creature * m_creature, uint32 entry)
		{
			if(player->getClass() != CLASS_HUNTER)
			{
				m_creature->MonsterWhisper("You're not a hunter.", player->GetGUID());
				player->PlayerTalkClass->SendCloseGossip();
				return;
			}

			if(player->GetPet())
			{
				m_creature->MonsterWhisper("First you must abandon your current pet.", player->GetGUID());
				player->PlayerTalkClass->SendCloseGossip();
				return;
			}

			if (Creature *creatureTarget = m_creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY()+2, player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 500))
			    if(Pet* pet = player->CreateTamedPetFrom(creatureTarget, 0))
			    {
				    // kill original creature
				    creatureTarget->setDeathState(JUST_DIED);
				    creatureTarget->RemoveCorpse();
				    creatureTarget->SetHealth(0);

				    // prepare visual effect for levelup
				    pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel() - 1);
				    pet->GetMap()->AddToMap((Creature*)pet);
				    // visual effect for levelup
				    pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel());


				    if(!pet->InitStatsForLevel(player->getLevel()))
					    sLog->outError(LOG_FILTER_PETS, "Pet Create fail: no init stats for entry %u", entry);

				    pet->UpdateAllStats();

				    // caster have pet now
				    player->SetMinion(pet, true);

				    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
				    pet->InitTalentForLevel();
				    player->PetSpellInitialize();

				    //end
				    player->PlayerTalkClass->SendCloseGossip();
				    m_creature->MonsterWhisper("You now have a pet, you should probably feed it...", player->GetGUID());
			    }
		}
				
		bool OnGossipHello(Player *player, Creature * m_creature)
		{

			if(player->getClass() != CLASS_HUNTER)
			{
				m_creature->MonsterWhisper("You're not a hunter, and cannot tame pets.", player->GetGUID());
				return false;
			}

			player->ADD_GOSSIP_ITEM(4, "Tame a new Pet!", GOSSIP_SENDER_MAIN, 100);
			player->ADD_GOSSIP_ITEM(2, "Show me the stables.", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_STABLEPET);
			player->ADD_GOSSIP_ITEM(6, "Show me your food.", GOSSIP_SENDER_MAIN, GOSSIP_OPTION_VENDOR);
			player->ADD_GOSSIP_ITEM(5, "Nevermind", GOSSIP_SENDER_MAIN, 200);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			return true;
				
		}
		
		bool OnGossipSelect(Player *player, Creature * m_creature, uint32 sender, uint32 action)
		{
			player->PlayerTalkClass->ClearMenus();
			switch (action)
			{
				case 100: // Submenu
					player->ADD_GOSSIP_ITEM(4, "I want to tame a bear!", GOSSIP_SENDER_MAIN, 1001);
					player->ADD_GOSSIP_ITEM(2, "I want to tame a boar!", GOSSIP_SENDER_MAIN, 1002);
					player->ADD_GOSSIP_ITEM(6, "I want to tame a crab!", GOSSIP_SENDER_MAIN, 1003);
					player->ADD_GOSSIP_ITEM(5, "I want to tame a spider!", GOSSIP_SENDER_MAIN, 1004);
					player->ADD_GOSSIP_ITEM(5, "Main Menu.", GOSSIP_SENDER_MAIN, 1005);
					player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
					break;
				
				case 200: // Nevermind.
					player->CLOSE_GOSSIP_MENU();
					break;

				case GOSSIP_OPTION_STABLEPET: // Stables
					player->GetSession()->SendStablePet(m_creature->GetGUID());									
					break;
				
				case GOSSIP_OPTION_VENDOR: // Food
					player->GetSession()->SendListInventory(m_creature->GetGUID());
					break;
				
				case 1001: // Bear
					player->ADD_GOSSIP_ITEM(4, "Tame a Light Brown Bear", GOSSIP_SENDER_MAIN, 1196);
					player->ADD_GOSSIP_ITEM(2, "Tame a White Bear", GOSSIP_SENDER_MAIN, 7444);
					player->ADD_GOSSIP_ITEM(4, "Tame a Black Bear", GOSSIP_SENDER_MAIN, 1128);
					player->ADD_GOSSIP_ITEM(2, "Tame a Dark Brown Bear", GOSSIP_SENDER_MAIN, 3809);
					player->ADD_GOSSIP_ITEM(4, "Tame an Infected Bear", GOSSIP_SENDER_MAIN, 17661);
					player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
					break;
						
				case 1002: // Boar
					player->ADD_GOSSIP_ITEM(4, "Tame a Spiked Red Boar", GOSSIP_SENDER_MAIN, 8303);
					player->ADD_GOSSIP_ITEM(2, "Tame a Spiked Blue Boar", GOSSIP_SENDER_MAIN, 22180);
					player->ADD_GOSSIP_ITEM(4, "Tame a Spiked Green Boar", GOSSIP_SENDER_MAIN, 16992);
					player->ADD_GOSSIP_ITEM(2, "Tame an Armored Boar", GOSSIP_SENDER_MAIN, 4511);
					player->ADD_GOSSIP_ITEM(4, "Tame a Black Boar", GOSSIP_SENDER_MAIN, 119);
					player->ADD_GOSSIP_ITEM(4, "Tame a Tan Boar", GOSSIP_SENDER_MAIN, 1985);
					player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
					break;
				
				case 1003: // Crab
					player->ADD_GOSSIP_ITEM(4, "Tame a Red Crab", GOSSIP_SENDER_MAIN, 3106);
					player->ADD_GOSSIP_ITEM(2, "Tame a Blue Crab", GOSSIP_SENDER_MAIN, 26521);
					player->ADD_GOSSIP_ITEM(4, "Tame a Green Crab", GOSSIP_SENDER_MAIN, 922);
					player->ADD_GOSSIP_ITEM(2, "Tame a Brown Crab", GOSSIP_SENDER_MAIN, 3814);
					player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
					break;
				
				case 1004: // Spider
					player->ADD_GOSSIP_ITEM(4, "Tame a Crystal Spider", GOSSIP_SENDER_MAIN, 10376);
					player->ADD_GOSSIP_ITEM(2, "Tame a Black Widow Spider", GOSSIP_SENDER_MAIN, 930);
					player->ADD_GOSSIP_ITEM(4, "Tame a Brown Furry Spider", GOSSIP_SENDER_MAIN, 30206);
					player->ADD_GOSSIP_ITEM(2, "Tame a Red Furry Spider", GOSSIP_SENDER_MAIN, 8933);
					player->ADD_GOSSIP_ITEM(4, "Tame a White Spider", GOSSIP_SENDER_MAIN, 4040);
					player->ADD_GOSSIP_ITEM(2, "Tame a Brown Spider", GOSSIP_SENDER_MAIN, 22044);
					player->ADD_GOSSIP_ITEM(4, "Tame a Red Spiky Spider", GOSSIP_SENDER_MAIN, 4380);
					player->ADD_GOSSIP_ITEM(2, "Tame a Green Spider", GOSSIP_SENDER_MAIN, 4376);
					player->ADD_GOSSIP_ITEM(4, "Tame a Multicolored Spiky Spider", GOSSIP_SENDER_MAIN, 26625);
					player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
					break;
					
				default:
					CreatePet(player, m_creature, action);
		
			}
			
		return true;
		}    
};

void AddSC_beastmaster() 
{
	new beastmaster();
}