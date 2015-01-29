/* ScriptData
SDName: dragon_soul_teleport_and_transport
SD%Complete: ~50%
SDComment:
SDCategory: Teleport and Transport
EndScriptData
*/


#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "Vehicle.h"
#include "dragon_soul.h"

Position const ValeeraMove[4] = 
{
	{-1796.139f, -2304.151f, 75.378f},
	{-1761.352f, -1918.498f, 167.278f},
	{-1760.377f, -1916.292f, -99.225f},
	{-1738.49f, -1821.749f, -191.55f} // end
};

Position const EiendormiMove[4] = 
{
	{-1777.807f, -2478.441f, 74.235f},
	{-1773.306f, -3028.074f, 147.553f},
	{-1774.058f, -3041.294f, -29.101f},
	{-1855.767f, -3071.929f, -146.524f} // end
};

Position const NethestraszMove[7] = 
{
	{-1640.749f, -2377.507f, 94.707f},
	{-1687.167f, -2270.384f, 176.948f},
	{-1792.717f, -2271.224f, 287.36f},
	{-1874.976f, -2361.625f, 389.567f},
	{-1770.352f, -2391.114f, 360.189f}, // end
	{-1703.348f, -2382.762f, 347.25f},
	{-1628.218f, -2383.541f, 202.471f}
};

enum Events
{
	EVENT_MOVE_1 = 1,
	EVENT_MOVE_2,
	EVENT_MOVE_3,
	EVENT_MOVE_4,
	EVENT_MOVE_END,

	// Move nethestrasz
	EVENT_N_MOVE_1,
	EVENT_N_MOVE_2,
	EVENT_N_MOVE_3,
	EVENT_N_MOVE_4,
	EVENT_N_MOVE_5,
	EVENT_N_MOVE_6_END,
	EVENT_N_MOVE_7,
	EVENT_N_MOVE_END
};

class npc_eiendormi : public CreatureScript
{
    public:
        npc_eiendormi() : CreatureScript("npc_eiendormi") { }

		Creature* eiendormi;

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(BOSS_MORCHOK) == DONE)
				{
                    eiendormi = creature->SummonCreature(57291, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
				    player->EnterVehicle(eiendormi, 0);
				}
            }

            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }
};

class npc_eiendormi_tr : public CreatureScript
{
    public:
        npc_eiendormi_tr() : CreatureScript("npc_eiendormi_tr") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_eiendormi_trAI(creature);
		}

		struct npc_eiendormi_trAI : public ScriptedAI
		{
		    npc_eiendormi_trAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			//Vehicle* vehicle;
			InstanceScript* instance;
			EventMap events;

			void PassengerBoarded(Unit* who, int8 seatId, bool apply)
			{
				if (apply)
				{
				    events.ScheduleEvent(EVENT_MOVE_1, 1000);
	                me->SetSpeed(MOVE_FLIGHT, 5.0f);
	                me->SetCanFly(true);
	                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				}
				else
					who->CastSpell(who, 44795, true); // parachute
			}

			void UpdateAI(uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
				     switch (eventId)
				     {
				     case EVENT_MOVE_1:
						 me->GetMotionMaster()->MovePoint(0, EiendormiMove[0]);
						 events.ScheduleEvent(EVENT_MOVE_2, 2000);
						 break;
					 case EVENT_MOVE_2:
						 me->GetMotionMaster()->MovePoint(1, EiendormiMove[1]);
						 events.ScheduleEvent(EVENT_MOVE_3, 16000);
						 break;
					 case EVENT_MOVE_3:
						 me->GetMotionMaster()->MovePoint(1, EiendormiMove[2]);
						 events.ScheduleEvent(EVENT_MOVE_4, 5000);
						 break;
					 case EVENT_MOVE_4:
						 me->GetMotionMaster()->MovePoint(1, EiendormiMove[3]);
						 events.ScheduleEvent(EVENT_MOVE_END, 5000);
						 break;
					 case EVENT_MOVE_END:
						 if (me->GetVehicleKit())
							 me->GetVehicleKit()->RemoveAllPassengers();
						 me->DespawnOrUnsummon();
						 break;
					 }
				}
			}
		};
};

class npc_valeera : public CreatureScript
{
    public:
        npc_valeera() : CreatureScript("npc_valeera") { }
		
		Creature* valeera;

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(BOSS_MORCHOK) == DONE)
				{
					valeera = creature->SummonCreature(57290, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
				    player->EnterVehicle(valeera, 0);
				}
            }

            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }
};

class npc_valeera_tr : public CreatureScript
{
    public:
        npc_valeera_tr() : CreatureScript("npc_valeera_tr") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_valeera_trAI(creature);
		}

		struct npc_valeera_trAI : public ScriptedAI
		{
		    npc_valeera_trAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			//Vehicle* vehicle;
			InstanceScript* instance;
			EventMap events;

			void PassengerBoarded(Unit* who, int8 seatId, bool apply)
			{
				if (apply)
				{
				    events.ScheduleEvent(EVENT_MOVE_1, 1000);
	                me->SetSpeed(MOVE_FLIGHT, 5.0f);
	                me->SetCanFly(true);
	                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				}
				else
					who->CastSpell(who, 44795, true); // parachute
			}

			void UpdateAI(uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
				     switch (eventId)
				     {
				     case EVENT_MOVE_1:
						 me->GetMotionMaster()->MovePoint(0, ValeeraMove[0]);
						 events.ScheduleEvent(EVENT_MOVE_2, 2000);
						 break;
					 case EVENT_MOVE_2:
						 me->GetMotionMaster()->MovePoint(1, ValeeraMove[1]);
						 events.ScheduleEvent(EVENT_MOVE_3, 11000);
						 break;
					 case EVENT_MOVE_3:
						 me->GetMotionMaster()->MovePoint(1, ValeeraMove[2]);
						 events.ScheduleEvent(EVENT_MOVE_4, 5000);
						 break;
					 case EVENT_MOVE_4:
						 me->GetMotionMaster()->MovePoint(1, ValeeraMove[3]);
						 events.ScheduleEvent(EVENT_MOVE_END, 8000);
						 break;
					 case EVENT_MOVE_END:
						 if (me->GetVehicleKit())
							 me->GetVehicleKit()->RemoveAllPassengers();
						 me->DespawnOrUnsummon();
						 break;
					 }
				}
			}
		};
};

class npc_nethestrasz : public CreatureScript
{
    public:
        npc_nethestrasz() : CreatureScript("npc_nethestrasz") { }

		Creature* nethestrasz;

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(BOSS_UNSLEEPING) == DONE && instance->GetBossState(BOSS_WARLORD) == DONE)
				{
                    nethestrasz = creature->SummonCreature(57298, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
				    player->EnterVehicle(nethestrasz, 0);
				}
            }

            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            return true;
        }
};

class npc_nethestrasz_tr : public CreatureScript
{
    public:
        npc_nethestrasz_tr() : CreatureScript("npc_nethestrasz_tr") { }

		CreatureAI* GetAI(Creature* creature) const
		{
		    return new npc_nethestrasz_trAI(creature);
		}

		struct npc_nethestrasz_trAI : public ScriptedAI
		{
		    npc_nethestrasz_trAI(Creature* creature) : ScriptedAI(creature)
		    {
			    instance = creature->GetInstanceScript();
		    }

			//Vehicle* vehicle;
			InstanceScript* instance;
			EventMap events;

			void PassengerBoarded(Unit* who, int8 seatId, bool apply)
			{
				if (apply)
				{
				    events.ScheduleEvent(EVENT_N_MOVE_1, 1000);
	                me->SetSpeed(MOVE_FLIGHT, 5.0f);
	                me->SetCanFly(true);
	                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				}
				else
					who->CastSpell(who, 44795, true); // parachute
			}

			void UpdateAI(uint32 diff)
			{
				events.Update(diff);

				while (uint32 eventId = events.ExecuteEvent())
				{
				     switch (eventId)
				     {
				     case EVENT_N_MOVE_1:
						 me->GetMotionMaster()->MovePoint(0, NethestraszMove[0]);
						 events.ScheduleEvent(EVENT_N_MOVE_2, 4000);
						 break;
					 case EVENT_N_MOVE_2:
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[1]);
						 events.ScheduleEvent(EVENT_N_MOVE_3, 4000);
						 break;
					 case EVENT_N_MOVE_3:
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[2]);
						 events.ScheduleEvent(EVENT_N_MOVE_4, 4000);
						 break;
					 case EVENT_N_MOVE_4:
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[3]);
						 events.ScheduleEvent(EVENT_N_MOVE_5, 5000);
						 break;
					 case EVENT_N_MOVE_5:
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[4]);
						 events.ScheduleEvent(EVENT_N_MOVE_6_END, 3000);
						 break;
					 case EVENT_N_MOVE_6_END:
						 if (me->GetVehicleKit())
							 me->GetVehicleKit()->RemoveAllPassengers();
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[5]);
						 events.ScheduleEvent(EVENT_N_MOVE_7, 2000);
						 break;
					 case EVENT_N_MOVE_7:
						 me->GetMotionMaster()->MovePoint(1, NethestraszMove[6]);
						 events.ScheduleEvent(EVENT_N_MOVE_END, 3000);
						 break;
					 case EVENT_N_MOVE_END:
						 me->DespawnOrUnsummon();
						 break;
					 }
				}
			}
		};
};

class travel_to_wyrmrest_base : public CreatureScript
{
    public:
        travel_to_wyrmrest_base() : CreatureScript("travel_to_wyrmrest_base") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetData(DATA_PORTALS_ON_OFF) == DONE)
                {
                    player->TeleportTo(967, -1793.22f, -2391.78f, 45.604f, 5.871f);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
            }

            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

class travel_to_wyrmrest_summit : public CreatureScript
{
    public:
        travel_to_wyrmrest_summit() : CreatureScript("travel_to_wyrmrest_summit") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetData(DATA_PORTALS_ON_OFF) == DONE || instance->GetData(DATA_PORTALS_ON_OFF) == FAIL)
                {
                    player->TeleportTo(967, -1786.92f, -2393.18f, 341.355f, 6.141f);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
            }

            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

class travel_to_the_eye_of_eternity : public CreatureScript
{
    public:
        travel_to_the_eye_of_eternity() : CreatureScript("travel_to_the_eye_of_eternity") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(BOSS_UNSLEEPING) == DONE && instance->GetBossState(BOSS_WARLORD) == DONE)
                {
                    player->TeleportTo(967, 13629.7f, 13611.9f, 123.482f, 3.468f);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
            }

            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

class travel_to_the_deck_of_the_skyfire : public CreatureScript
{
    public:
        travel_to_the_deck_of_the_skyfire() : CreatureScript("travel_to_the_deck_of_the_skyfire") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetBossState(BOSS_ULTRAXION) == DONE)
                {
                    player->TeleportTo(967, 13397.298f, -12131.405f, 153.8732f, 3.152f);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                }
            }

            player->CLOSE_GOSSIP_MENU();
            return true;
        }
};

void AddSC_dragon_soul_teleport_and_transport()
{
    new npc_eiendormi();
	new npc_eiendormi_tr();
    new npc_valeera();
	new npc_valeera_tr();
    new npc_nethestrasz();
	new npc_nethestrasz_tr();
    new travel_to_wyrmrest_base;
    new travel_to_wyrmrest_summit;
    new travel_to_the_eye_of_eternity;
    new travel_to_the_deck_of_the_skyfire;
}