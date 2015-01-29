/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Creature.h"
#include "CreatureAI.h"
#include "MapManager.h"
#include "FleeingMovementGenerator.h"
#include "PathGenerator.h"
#include "ObjectAccessor.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"
#include "VMapFactory.h"

#define MIN_QUIET_DISTANCE 28.0f
#define MAX_QUIET_DISTANCE 43.0f

template<class T>
void FleeingMovementGenerator<T>::_setTargetLocation(T* owner)
{
    if (!owner)
        return;

    if (owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
        return;

    owner->AddUnitState(UNIT_STATE_FLEEING_MOVE);

    float x, y, z;
    _getPoint(owner, x, y, z);

	// Add LOS check for target point
    Position mypos;
    owner->GetPosition(&mypos);
    bool isInLOS = VMAP::VMapFactory::createOrGetVMapManager()->isInLineOfSight(owner->GetMapId(),
                                                                                mypos.m_positionX,
                                                                                mypos.m_positionY,
                                                                                mypos.m_positionZ + 2.0f,
                                                                                x, y, z + 2.0f);
    if (!isInLOS)
    {
        i_nextCheckTime.Reset(urand(1000, 1500));
        return;
    }

    PathGenerator path(owner);
    path.SetPathLengthLimit(15.0f);
    bool result = path.CalculatePath(x, y, z);
    if (!result || (path.GetPathType() & PATHFIND_NOPATH))
    {
        i_nextCheckTime.Reset(urand(1000, 1500));
        return;
    }

    Movement::MoveSplineInit init(owner);
	owner->AddUnitState(UNIT_STATE_FLEEING | UNIT_STATE_ROAMING);
    init.MovebyPath(path.GetPath());
    init.SetWalk(false);
    int32 traveltime = init.Launch();
    i_nextCheckTime.Reset(traveltime + urand(800, 1500));
}

template<class T>
void FleeingMovementGenerator<T>::_getPoint(T* owner, float &x, float &y, float &z)
{
    if (!owner)
        return;

    x = owner->GetPositionX();
    y = owner->GetPositionY();
    z = owner->GetPositionZ();

    float temp_x, temp_y, angle;
    const Map* _map = owner->GetBaseMap();
    // primitive path-finding
    for (uint8 i = 0; i < 18; ++i)
    {
        if (i_only_forward && i > 2)
            break;

        float distance = 5.0f;

        switch (i)
        {
            case 0:
                angle = i_cur_angle;
                break;
            case 1:
                angle = i_cur_angle;
                distance /= 2;
                break;
            case 2:
                angle = i_cur_angle;
                distance /= 4;
                break;
            case 3:
                angle = i_cur_angle + static_cast<float>(M_PI/4);
                break;
            case 4:
                angle = i_cur_angle - static_cast<float>(M_PI/4);
                break;
            case 5:
                angle = i_cur_angle + static_cast<float>(M_PI/4);
                distance /= 2;
                break;
            case 6:
                angle = i_cur_angle - static_cast<float>(M_PI/4);
                distance /= 2;
                break;
            case 7:
                angle = i_cur_angle + static_cast<float>(M_PI/2);
                break;
            case 8:
                angle = i_cur_angle - static_cast<float>(M_PI/2);
                break;
            case 9:
                angle = i_cur_angle + static_cast<float>(M_PI/2);
                distance /= 2;
                break;
            case 10:
                angle = i_cur_angle - static_cast<float>(M_PI/2);
                distance /= 2;
                break;
            case 11:
                angle = i_cur_angle + static_cast<float>(M_PI/4);
                distance /= 4;
                break;
            case 12:
                angle = i_cur_angle - static_cast<float>(M_PI/4);
                distance /= 4;
                break;
            case 13:
                angle = i_cur_angle + static_cast<float>(M_PI/2);
                distance /= 4;
                break;
            case 14:
                angle = i_cur_angle - static_cast<float>(M_PI/2);
                distance /= 4;
                break;
            case 15:
                angle = i_cur_angle +  static_cast<float>(3*M_PI/4);
                distance /= 2;
                break;
            case 16:
                angle = i_cur_angle -  static_cast<float>(3*M_PI/4);
                distance /= 2;
                break;
            case 17:
                angle = i_cur_angle + static_cast<float>(M_PI);
                distance /= 2;
                break;
            default:
                angle = 0.0f;
                distance = 0.0f;
                break;
        }

        temp_x = x + distance * std::cos(angle);
        temp_y = y + distance * std::sin(angle);
        Trinity::NormalizeMapCoord(temp_x);
        Trinity::NormalizeMapCoord(temp_y);
        if (owner->IsWithinLOS(temp_x, temp_y, z))
        {
            bool is_water_now = _map->IsInWater(x,y,z);

            if (is_water_now && _map->IsInWater(temp_x,temp_y,z))
            {
                x = temp_x;
                y = temp_y;
                return;
            }
            float new_z = _map->GetHeight(owner->GetPhaseMask(), temp_x, temp_y, z, true);

            if (new_z <= INVALID_HEIGHT)
                continue;

            bool is_water_next = _map->IsInWater(temp_x, temp_y, new_z);

            if ((is_water_now && !is_water_next && !is_land_ok) || (!is_water_now && is_water_next && !is_water_ok))
                continue;

            if (!(new_z - z) || distance / fabs(new_z - z) > 1.0f)
            {
                float new_z_left = _map->GetHeight(owner->GetPhaseMask(), temp_x + 1.0f* std::cos(angle+static_cast<float>(M_PI/2)),temp_y + 1.0f* std::sin(angle+static_cast<float>(M_PI/2)),z,true);
                float new_z_right = _map->GetHeight(owner->GetPhaseMask(), temp_x + 1.0f* std::cos(angle-static_cast<float>(M_PI/2)),temp_y + 1.0f* std::sin(angle-static_cast<float>(M_PI/2)),z,true);
                if (fabs(new_z_left - new_z) < 1.2f && fabs(new_z_right - new_z) < 1.2f)
                {
                    x = temp_x;
                    y = temp_y;
                    z = new_z;
                    return;
                }
            }
        }
    }
    i_to_distance_from_caster = 0.0f;
    i_nextCheckTime.Reset(urand(500,1000));
    return;
}

template<class T>
void FleeingMovementGenerator<T>::DoInitialize(T* owner)
{
    if (!owner)
        return;

    Movement::MoveSplineInit _Init(owner);
    owner->CastStop();
    owner->AddUnitState(UNIT_STATE_FLEEING | UNIT_STATE_ROAMING);
    owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner->SetUInt64Value(UNIT_FIELD_TARGET, 0);
    owner->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

    if(Unit * fright = ObjectAccessor::GetUnit(*owner, i_frightGUID))
    {
        i_caster_x = fright->GetPositionX();
        i_caster_y = fright->GetPositionY();
        i_caster_z = fright->GetPositionZ();
    }
    else
    {
        i_caster_x = owner->GetPositionX();
        i_caster_y = owner->GetPositionY();
        i_caster_z = owner->GetPositionZ();
    }

    i_only_forward = false;
    i_cur_angle = 0.0f;
    i_last_distance_from_caster = 0.0f;
    i_to_distance_from_caster = 0.0f;
    _setTargetLocation(owner);
}

template<>
void FleeingMovementGenerator<Player>::DoFinalize(Player* owner)
{
    owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner->ClearUnitState(UNIT_STATE_FLEEING | UNIT_STATE_ROAMING);
    owner->StopMoving();
}

template<>
void FleeingMovementGenerator<Creature>::DoFinalize(Creature* owner)
{
    owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner->ClearUnitState(UNIT_STATE_FLEEING | UNIT_STATE_ROAMING);
    if (owner->getVictim())
        owner->SetTarget(owner->getVictim()->GetGUID());
}

template<class T>
void FleeingMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
bool FleeingMovementGenerator<T>::DoUpdate(T* owner, uint32 time_diff)
{
    if (!owner || !owner->isAlive())
        return false;

    if (owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
    {
        owner->ClearUnitState(UNIT_STATE_FLEEING_MOVE);
        return true;
    }

    i_nextCheckTime.Update(time_diff);
    if (i_nextCheckTime.Passed() && owner->movespline->Finalized())
        _setTargetLocation(owner);

    return true;
}

template void FleeingMovementGenerator<Player>::DoInitialize(Player*);
template void FleeingMovementGenerator<Creature>::DoInitialize(Creature*);
template void FleeingMovementGenerator<Player>::_getPoint(Player*, float&, float&, float&);
template void FleeingMovementGenerator<Creature>::_getPoint(Creature*, float&, float&, float&);
template void FleeingMovementGenerator<Player>::_setTargetLocation(Player*);
template void FleeingMovementGenerator<Creature>::_setTargetLocation(Creature*);
template void FleeingMovementGenerator<Player>::DoReset(Player*);
template void FleeingMovementGenerator<Creature>::DoReset(Creature*);
template bool FleeingMovementGenerator<Player>::DoUpdate(Player*, uint32);
template bool FleeingMovementGenerator<Creature>::DoUpdate(Creature*, uint32);

void TimedFleeingMovementGenerator::Finalize(Unit* owner)
{
    owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING);
    owner->ClearUnitState(UNIT_STATE_FLEEING|UNIT_STATE_FLEEING_MOVE);
    if (Unit* victim = owner->getVictim())
    {
        if (owner->isAlive())
        {
            owner->AttackStop();
            owner->ToCreature()->AI()->AttackStart(victim);
        }
    }
}

bool TimedFleeingMovementGenerator::Update(Unit* owner, uint32 time_diff)
{
    if (!owner->isAlive())
        return false;

    if (owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
    {
        owner->ClearUnitState(UNIT_STATE_FLEEING_MOVE);
        return true;
    }

    i_totalFleeTime.Update(time_diff);
    if (i_totalFleeTime.Passed())
        return false;

    // This calls grant-parent Update method hiden by FleeingMovementGenerator::Update(Creature &, uint32) version
    // This is done instead of casting Unit& to Creature& and call parent method, then we can use Unit directly
    return MovementGeneratorMedium< Creature, FleeingMovementGenerator<Creature> >::Update(owner, time_diff);
}
