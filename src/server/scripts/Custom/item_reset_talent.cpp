#include "ScriptPCH.h"
 
class item_reset_talent : public ItemScript
{
        public:
                item_reset_talent() : ItemScript("item_reset_talent") {}
               
        bool OnUse(Player * pPlayer, Item * /*item*/, SpellCastTargets const& /*targets*/)
        {
                if (pPlayer->IsInCombat())
                {
                        pPlayer->GetSession()->SendNotification("Vous etes en combat!");
                        return false;
                }
				else
                {
                        pPlayer->ResetTalents(true);
                        pPlayer->SendTalentsInfoData(false);
                        pPlayer->GetSession()->SendNotification("Vos talents ont ete reset!");
						return true;
                }
        }
};
 
void AddSC_item_reset_talent()
{
    new item_reset_talent();
}