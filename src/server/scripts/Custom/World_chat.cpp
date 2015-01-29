/*
<--------------------------------------------------------------------------->
 - Developer(s): Ghostcrawler336
 - Complete: 100%
 - ScriptName: 'World Chat Command'
 - Comment: Untested
<--------------------------------------------------------------------------->
*/

#include "ScriptPCH.h"
#include "Chat.h"
#include "Common.h"

/* Colors */
#define MSG_COLOR_ORANGE "|cffFFA500"
#define MSG_COLOR_DARKORANGE "|cffFF8C00"
#define MSG_COLOR_RED "|cffFF0000"
#define MSG_COLOR_LIGHTRED "|cffD63931"
#define MSG_COLOR_ROYALBLUE "|cff4169E1"
#define MSG_COLOR_LIGHTBLUE "|cffADD8E6"
#define MSG_COLOR_YELLOW "|cffFFFF00"
#define MSG_COLOR_GREEN "|cff008000"
#define MSG_COLOR_PURPLE "|cff660066"
#define MSG_COLOR_WHITE  "|cffffffff"
#define MSG_COLOR_SUBWHITE  "|cffbbbbbb"
#define MSG_COLOR_BLACK  "|cff000000"

/* Ranks */
#define OWNER "Owner"
#define ADMINISTRATOR "Admin"
#define HEAD_OF_STAFF "Head GM"
#define DEVELOPER "Developer"
#define GAMEMASTER "Senior GM"
#define JUNIOR_GM "Junior GM"
#define TRIAL_GM "Trial GM"
#define MODERATOR "Trial GM"
#define VIP "VIP"
#define PLAYER "Player"

class World_Chat : public CommandScript
{
    public:
    World_Chat() : CommandScript("World_Chat") { }

    static bool HandleWorldChatCommand(ChatHandler * pChat, const char * msg)
    {
		if(!*msg)
			return false;

        Player * player = pChat->GetSession()->GetPlayer();
        char message[1024];

		switch(player->GetSession()->GetSecurity())
		{
			case SEC_PLAYER:
				snprintf(message, 1024, "[World][Player][%s%s|r]: %s%s|r", MSG_COLOR_WHITE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_VIP:
				snprintf(message, 1024, "[World][VIP][%s%s|r]: %s%s|r", MSG_COLOR_GREEN, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_MODERATOR:
				snprintf(message, 1024, "[World][Trail GM][%s%s|r]: %s%s|r", MSG_COLOR_WHITE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_DEVELOPER:
				snprintf(message, 1024, "[World][Developer][%s%s|r]: %s%s|r", MSG_COLOR_BLACK, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_TRIAL_GM:
				snprintf(message, 1024, "[World][Trial GM][%s%s|r]: %s%s|r", MSG_COLOR_YELLOW, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
	        case SEC_JUNIOR_GM:
				snprintf(message, 1024, "[World][Junior GM][%s%s|r]: %s%s|r", MSG_COLOR_LIGHTBLUE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_GAMEMASTER:
				snprintf(message, 1024, "[World][Senior GM][%s%s|r]: %s%s|r", MSG_COLOR_ROYALBLUE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_HEAD_OF_STAFF:
				snprintf(message, 1024, "[World][Head GM][%s%s|r]: %s%s|r", MSG_COLOR_RED, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_ADMINISTRATOR:
				snprintf(message, 1024, "[World][Admin][%s%s|r]: %s%s|r", MSG_COLOR_PURPLE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;
			case SEC_OWNER:
				snprintf(message, 1024, "[World][Owner][%s%s|r]: %s%s|r", MSG_COLOR_PURPLE, player->GetName().c_str(), MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;

			case SEC_CONSOLE:
				snprintf(message, 1024, "[World][%sAdministrator|r][%s%s|r]: %s%s|r", MSG_COLOR_RED, MSG_COLOR_LIGHTRED, player->GetName().c_str(),
					MSG_COLOR_LIGHTBLUE, msg);
				sWorld->SendGlobalText(message, NULL);
			break;

			
			
			
		}
        return true;
    }

    ChatCommand * GetCommands() const
    {
            static ChatCommand HandleWorldChatCommandTable[] =
            {
                    { "world",        SEC_PLAYER,         true,     &HandleWorldChatCommand,               "",  NULL },
                    { NULL,              0,               false,    NULL,                                  "",  NULL }
            };
            return HandleWorldChatCommandTable;
    }
};

void AddSC_World_Chat()
{
        new World_Chat;
}