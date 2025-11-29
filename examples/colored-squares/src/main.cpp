#include "bn_core.h"
#include "bn_keypad.h"

#include "ecsa.h"

#include "cs_entities.h"
#include "cs_definitions.h"

#include "cs_sys_movement.h"
#include "cs_sys_rotation.h"
#include "cs_sys_scaling.h"
#include "cs_sys_visibility.h"
#include "cs_sys_entity_manager.h"
#include "cs_sys_animation.h"

using namespace cs;

int main()
{
    bool paused = false;

    bn::core::init();

    // define an entity table
    Table table;

    // set up all the updaters, cached queries...
    table.add<Ids::SYSMOVEMENT>(new SysMovement(table));
    table.add<Ids::SYSROTATION>(new SysRotation(table));
    table.add<Ids::SYSVISIBILITY>(new SysVisibility(table));
    table.add<Ids::SYSSCALING>(new SysScaling(table));
    table.add<Ids::SYSANIMATION>(new SysAnimation(table));
    table.add<Ids::SYSENTITYMANAGER>(new SysEntityManager(table));

    // initialize all the updaters
    table.init();

    // add 4 entities
    entities::red_square(table);
    entities::blue_square(table);
    entities::yellow_square(table);
    entities::flashing_square(table);

    while (true)
    {
        // update table
        table.update();
        
        bn::core::update();
    }

}