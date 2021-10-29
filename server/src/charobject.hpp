/*
 * =====================================================================================
 *
 *       Filename: charobject.hpp
 *        Created: 04/10/2016 12:05:22
 *    Description:
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */
#pragma once

#include <map>
#include <list>
#include <deque>
#include <vector>
#include <memory>
#include <unordered_map>
#include "totype.hpp"
#include "corof.hpp"
#include "fflerror.hpp"
#include "servermap.hpp"
#include "damagenode.hpp"
#include "actionnode.hpp"
#include "timedstate.hpp"
#include "cachequeue.hpp"
#include "scopedalloc.hpp"
#include "servicecore.hpp"
#include "protocoldef.hpp"
#include "serverobject.hpp"

enum _RangeType: uint8_t
{
    RANGE_VIEW,
    RANGE_MAP,
    RANGE_SERVER,

    RANGE_VISIBLE,
    RANGE_ATTACK,
    RANGE_TRACETARGET,
};

struct COLocation
{
    uint64_t uid   = 0;
    uint32_t mapID = 0;

    int x = -1;
    int y = -1;
    int direction = DIR_NONE;
};

class CharObject: public ServerObject
{
    protected:
        class COPathFinder final: public AStarPathFinder
        {
            private:
                friend class CharObject;

            private:
                const CharObject *m_CO;

            private:
                const int m_checkCO;

            private:
                mutable std::map<uint32_t, int> m_cache;

            public:
                COPathFinder(const CharObject *, int);
               ~COPathFinder() = default;

            private:
               int GetGrid(int, int) const;
        };

    protected:
        enum SpeedType: int
        {
            SPEED_NONE = 0,
            SPEED_MOVE,
            SPEED_ATTACK,
        };

    protected:
        struct Offender
        {
            uint64_t uid = 0;
            uint64_t damage = 0;
            uint64_t activeTime = 0;
        };

    protected:
        const ServerMap *m_map;

    protected:
        const ServerMap *GetServerMap() const
        {
            return m_map;
        }

    protected:
        std::unordered_map<uint64_t, COLocation> m_inViewCOList;

    protected:
        int m_X;
        int m_Y;
        int m_direction;

    protected:
        SDHealth m_sdHealth;

    protected:
        bool m_moveLock;
        bool m_attackLock;

    protected:
        int m_lastAction = ACTION_NONE;
        std::array<uint32_t, ACTION_END> m_lastActionTime;

    protected:
        TimedState<bool> m_dead;

    protected:
        std::vector<Offender> m_offenderList;

    public:
        CharObject(
                const ServerMap *,  // server map
                uint64_t,           // uid
                int,                // map x
                int,                // map y
                int);               // direction

    public:
        ~CharObject() = default;

    protected:
        int X() const { return m_X; }
        int Y() const { return m_Y; }

    protected:
        int Direction() const
        {
            return m_direction;
        }

        uint32_t mapID() const
        {
            return m_map ? m_map->ID() : 0;
        }

        uint64_t mapUID() const
        {
            return m_map->UID();
        }

    public:
        virtual bool update() = 0;

    public:
        bool NextLocation(int *, int *, int, int);
        bool NextLocation(int *pX, int *pY, int nDistance)
        {
            return NextLocation(pX, pY, Direction(), nDistance);
        }

    public:
        void onActivate() override
        {
            ServerObject::onActivate();
            dispatchAction(ActionSpawn
            {
                .x = X(),
                .y = Y(),
                .direction = Direction(),
            });
        }

    protected:
        virtual void reportCO(uint64_t) = 0;

    protected:
        void dispatchHealth();
        void dispatchAttackDamage(uint64_t, int);

    protected:
        virtual void dispatchAction(          const ActionNode &);
        virtual void dispatchAction(uint64_t, const ActionNode &);

    protected:
        virtual int OneStepReach(int, int, int *, int *);

    protected:
        virtual int Speed(int) const;

    protected:
        void getCOLocation(uint64_t, std::function<void(const COLocation &)>, std::function<void()> = []{});

    protected:
        bool requestJump(
                int,                                // x
                int,                                // y
                int,                                // direction
                std::function<void()> = nullptr,    // fnOnOK
                std::function<void()> = nullptr);   // fnOnError

    protected:
        bool requestMove(
                int,                                // x
                int,                                // y
                int,                                // speed
                bool,                               // allowHalfMove
                bool,                               // removeMonster: force monster on (x, y) go to somewhere else to make room
                std::function<void()> = nullptr,    // fnOnOK
                std::function<void()> = nullptr);   // fnOnError

    protected:
        bool requestSpaceMove(
                int,                                // x
                int,                                // y
                bool,                               // strictMove
                std::function<void()> = nullptr,    // fnOnOK
                std::function<void()> = nullptr);   // fnOnError

    protected:
        bool requestMapSwitch(
                uint32_t,                           // mapID
                int,                                // x
                int,                                // y
                bool,                               // strictMove
                std::function<void()> = nullptr,    // fnOnOK
                std::function<void()> = nullptr);   // fnOnError

    protected:
        virtual bool canAct()    const;
        virtual bool canMove()   const;
        virtual bool canAttack() const;

    protected:
        void setLastAction(int);

    protected:
        virtual bool dcValid(int, bool) = 0;

    protected:
        virtual DamageNode getAttackDamage(int) const = 0;

    protected:
        virtual bool struckDamage(const DamageNode &) = 0;

    protected:
        void addMonster(uint32_t, int, int, bool);

    protected:
        virtual bool goDie()   = 0;
        virtual bool goGhost() = 0;

    protected:
        virtual int MaxStep() const
        {
            return 1;
        }

        virtual int MoveSpeed()
        {
            return SYS_DEFSPEED;
        }

    protected:
        // estimate how many hops we need
        // this function checks map but can't check CO
        // if we found one-hop distance we need send move request to servermap
        // return:
        //          -1: invalid
        //           0: no move needed
        //           1: one-hop can reach
        //           2: more than one-hop can reach
        int estimateHop(int, int);

    protected:
        int AttackSpeed() const
        {
            return SYS_DEFSPEED;
        }

        int MagicSpeed() const
        {
            return SYS_DEFSPEED;
        }

        int Horse() const
        {
            return 0;
        }

    protected:
        std::array<PathFind::PathNode, 3>    GetChaseGrid(int, int, int) const;
        std::vector<PathFind::PathNode> GetValidChaseGrid(int, int, int) const;

    protected:
        void GetValidChaseGrid(int, int, int, scoped_alloc::svobuf_wrapper<PathFind::PathNode, 3> &) const;

    protected:
        int CheckPathGrid(int, int) const;
        double OneStepCost(const CharObject::COPathFinder *, int, int, int, int, int) const;

    protected:
        bool inView(uint32_t, int, int) const;

    protected:
        bool updateInViewCO(const COLocation &, bool = false);
        void foreachInViewCO(std::function<void(const COLocation &)>);

    protected:
        COLocation *getInViewCOPtr(uint64_t uid)
        {
            if(auto p = m_inViewCOList.find(uid); p != m_inViewCOList.end()){
                return std::addressof(p->second);
            }
            return nullptr;
        }

    protected:
        virtual void checkFriend(uint64_t, std::function<void(int)>) = 0;

    protected:
        void queryHealth(uint64_t, std::function<void(uint64_t, SDHealth)>);
        void queryFinalMaster(uint64_t, std::function<void(uint64_t)>);

    protected:
        bool isOffender(uint64_t);

    protected:
        bool isPlayer()  const;
        bool isMonster() const;
        bool isMonster(const char8_t *) const;

    protected:
        void notifyDead(uint64_t);

    protected:
        virtual ActionNode makeActionStand() const;

    protected:
        template<typename... Args> void dispatchInViewCONetPackage(uint8_t type, Args && ... args)
        {
            for(const auto &[uid, coLoc]: m_inViewCOList){
                if(uidf::getUIDType(coLoc.uid) == UID_PLY){
                    forwardNetPackage(coLoc.uid, type, std::forward<Args>(args)...);
                }
            }
        }
};
