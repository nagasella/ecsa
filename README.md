# ECSA (Entity System Advance)

ECSA (Entity-Component-System Advance) is an Entity-Component-System (ECS) framework written in C++. It is mostly targetting Game Boy Advance (GBA) game development, so it does not make any use of C++ features like RTTI or exceptions.

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ECSA and its features. You can use it to learn and also as a reference to come back to from time to time. It covers all aaspects from basic usage to common optimization techniques. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

* [An introduction to Entiy Systems](#an-introduction-to-entity-systems)

* [An introduction to ECSA](#an-introduction-to-ecsa)

* [Using ECSA in your project](#using-ecsa-in-your-project)

* [Entity tables](#entity-tables)

* [Entities and components](#entities-and-components)

* [Systems](#systems)

* [Queries](#queries)

* [Appendix: boosting performance with ARM code](#appendix-boosting-performance-with-arm-code)

## An introduction to Entity Systems

Entity Systems (or, more precisely, Entity-Component-System frameworks, ECS) have been a hot topic in game development for several years, although there are different visions about how to implement one. ECSA is inspired by the model discussed in a series of articles by Adam Martin from 2007 entitled [Entity Systems are the future of mmog development](https://web.archive.org/web/20131226102755/http://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (now only available on web archive). The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a relational database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components, and Systems are routines that process all the entities that share a common set of Components. Entity IDs may also be retrieved through queries.

## An introduction to ECSA

ECSA is a compact, header-only framework. It aims to implement the vision described in the articles linked above, allowing you to organize your game in a way that resembles a _relational database_ (although here the _relational_ aspect is arguably not important). It provides a clear framework to organize the code of your games and [makes it simple to identify performance-critical parts of your code to compile as ARM instructions](#appendix-boosting-performance-with-arm-code). Although ECS frameworks are generally notorious for being _cache-friendly_ and therefore providing very good performance compared to classical OOP, the hardware of the GBA is such that these kinds of advantages cannot be appreciated as much: the GBA does not have a cache in the modern sense, all the memory addresses are accessed directly from the CPU and so there is no such thing as a _cache-friendly_ approach to programming for the GBA.

Still, ECSA is relatively performant and provides several knobs to optimize memory efficiency; it can also be of great value in giving a clear framework to architect your game, by making use of few, simple building blocks:
* [_Entity tables_](#entity-tables) are the main data structure of ECSA: they contain all the data about game objects, organized in a tabular format. You can have one or more entity tables in your game
* [_Entities_](#entities-and-components) are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* [_Components_](#entities-and-components) are the _column indices_ of the table, and they implement the data for each entity - each entity can own (or not) each component
* [_Systems_](#systems) are objects that are used to update (every frame) all the entities that satisfy a certain condition
* [_Queries_](#queries) can be performed on an entity table, and they allow to retrieve IDs of entities that satisfy a certain condition (pretty much like SQL queries allow to filter rows in a table)

## Using ECSA in your project

You can use ECSA with any GBA game engine out there. For example, [Butano](https://github.com/GValiente/butano) is an awesome game engine for GBA, which I highly recommend. If you are planning to use ECSA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ECSA's `include` path. 

You can also use ECSA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website. Again, you will need to add ECSA's `include` path to your project.

## Entity tables

Entity tables allow to store game objects and their components. Technically speaking, an _entity_ is the row index of an entitiy table, while _components_ are the columns of the table. In order to optimize memory consumption, ECSA allows to parameterize entity tables with these template parameters:

* `Entities`: the maximum number of entities available (the number of rows for the table)

* `Components`: the maximum number of [components](#components) available (the number of columns for the table)

* `Systems`: the maximum number of [systems](#systems) supported by the table

Therefore, we can define an entity table like this:

```cpp
ecsa::EntityTable<100, 2, 4> table;
```

This means that the `EntityTable` can contain up to `100` entities and up to `2` components, and have `4` associated systems.

It is generally useful to define an alias for a table like this:

```cpp
using Table = ecsa::EntityTable<100, 2, 4>;
```

I will use this alias throughout the tutorial for simplicity.

## Entities and components

A new entity can be created inside a table like this: 

```cpp
ecsa::Entity e = table.create();
```

Here, `e` is an ID that identifies the entity uniquely (`ecsa::Entity` is just an alias for `unsigned short`).

Then, we can define some components:

```cpp
#define POSITION 0
#define VELOCITY 1

struct Vector2 : public ecsa:Component 
{
    int x, y;
};
```

`POSITION` and `VELOCITY` are just two integer indexes that are used to identify the components. Components are identified both by their _type_ and by their _index_, which allows to have multiple components of the same type. In any case, they need to inherit from the struct `ecsa::Component`. 

We can add the components to the entity created above like this:

```cpp
table.add<POSITION>(e, new Vector2(0, 0));
table.add<VELOCITY>(e, new Vector2(1, 1));
```

It is important that components are created using `new`, since ECSA will call `delete` when they need to be destroyed. Finally, we need to _subscribe_ the entity to all relevant systems in the table:

```cpp
table.subscribe(e);
```

At this point, we can get references to each component using the entity table:

```cpp
Vector2 & p = table.get<Vector2, POSITION>(e);
Vector2 & v = table.get<Vector2, VELOCITY>(e);

p.x += v.x;
p.y += v.y;
```

Finally, entities can be deleted from the table using:

```cpp
table.destroy(e);
```

If a component of the entity hold resources, like for example sprites or backgrounds, make sure to deallocate them in the component's destructor, and they will be freed when the entity is destroyed. 

To destroy all entities in a table, simply use:

```cpp
table.clear();
```

## Systems

Systems are the objects we use to process (update) entities at each frame. A system will process only entities that satisfy a certain condition.

Each system has to inherit from `ecsa::System`, a template class. For example...

```cpp
class MySystem : public ecsa::System<64>
{

};
```

... Defines a system that can process up to 64 entities.

A system can implement one or more of the following functions (none is mandatory):
* `void init()`: initialization logic (ran only once)
* `void update()`: update logic (ran every frame)
* `bool select(ecsa::Entity e)`: allows to determine whether each newly created entity should be processed by the system; it is triggered automatically every time a new entity is _subscribed_ to a table

If no `select` function is defined, the system will not process _any_ entity (it can still be used to handle generic game logic that is not related to any entity).

Let's make a practical example and implement an update logic for the `Position` and `Velocity` components (basically, the query will change the position of each entity based on its valocity):

```cpp
class SysMovement : public ecsa::System<100>
{
    // a reference to the entity table
    Table & table;

    public:

    // constructor - takes a reference to the entity table
    SysMovement(Table & table) : ecsa::System<100>(), table(table)
    {

    }

    // the system will process only entities that have both a POSITION and VELOCITY component
    bool select(ecsa::entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    // update logic for each frame
    void update() override
    {
        // loop on all entities subscribed to this system
        for (ecsa::Entity e : this->subscribed())
        {
            Position & p = table.get<Position, POSITION>(e);
            Velocity & v = table.get<Velocity, VELOCITY>(e);

            p.x += v.x;
            p.y += v.y;
        }
    }
};
```

Then, the system can be added to the table. An integer index has to be defined for each system, pretty much like for components: this allows to retrieve the system later on. Here is an example of how to use the system defined above in the main program of our game:

```cpp
#define SYSMOVEMENT 0

// define an entity table
Table table;

// add all the systems and initialize them
table.add<SYSMOVEMENT>(new SysMovement(table));
table.init();

// can add some entities with their components at this point...

// main loop
while (true)
{
    // call update() for every system previously added
    table.update();
}

```

And that's it. Now, every time an entity is added to the table, if it owns a `POSITION` and `VELOCITY` component it will be updated by the system.

Keep in mind that it is possible at run time to activate or deactivate systmes. For example...

```cpp
table.deactivate<SYSMOVEMENT>();
```

... Will deactivate the system (it will not be processed anymore by `table.update()`). The system can then be activated again by:

```cpp
table.activate<SYSMOVEMENT>();
```

You can also deactivate or activate _all_ the systems associated to a table by using:

```cpp
table.deactivate_all();
```

And:

```cpp
table.activate_all();
```

This makes it easy to block certain functionalities during the game, and implement (for example) game pause menus.

## Queries

Often it is useful, at any point in the program, to retrieve the IDs of entites that satisfy a certain condition: queries allow to do exactly this. ECSA offers several types of queries, listed in the next sub-sections. In general, the result of a query is an `ecsa::Vector`, which is a vector-like data structure with maximum capacity defined at compile time (however, remember that by design it does NOT preserve the order of the elements when an element is removed).

### 1. Queries based on a system

We can obtain all the IDs of the entities currently subscribed by a certain system (for example, the `SysMovement` defined above) by using:

```cpp
ecsa::Vector<ecsa::Entity, 100> ids = table.query<100, SYSMOVEMENT>();
```

Now we can loop on these entities, for example using a range-based for loop:

```cpp
for (ecsa::Entity e : ids)
{
    // do something...
}
```

### 2. Queries based on a function

Arbitraily complex queries can be performed through simple functions. The function has to be defined as follows:

```cpp
bool find_entities_with_positive_x(Table & table, ecsa::Entity e)
{
    Vecotr2 & p = table.get<Vector2, POSITION>(e);
    if (p.x > 0)
        return true;
    return false;
}
```

Note that this is a `bool` function returning `true` if the entity satisfies the condition, and `false` otherwise. We can then use it to query entities from the table:

```cpp
ecsa::Vector<ecsa::Entity, 100> ids = table.query<100>(&find_entities_with_positive_x);
```

The query above will run on _every_ entity in the table. However, to make it faster, we may decide to run it only on the entities of the `SysMovement` previously defined:

```cpp
ecsa::Vector<ecsa::Entity, 100> ids = table.query<100, SYSMOVEMENT>(&find_entities_with_positive_x);
```

### 3. Queries based on a function taking a parameter

We can make queries _dynamic_ by making it take an extra parameter of any kind, so that we can filter based on that parameter. For example:

```cpp
bool find_entities_with_positive_x(Table & table, ecsa::Entity e, int & x_limit)
{
    Vecotr2 & p = table.get<Vector2, POSITION>(e);
    if (p.x > x_limit)
        return true;
    return false;
}
```

Now, the function can be used as follows:

```cpp
int x_limit = 200;

ecsa::Vector<ecsa::Entity, 100> ids = table.query<100, int>(&find_entities_with_positive_x, x_limit);
```

Here, the parameter we are passing is an `int`, but it could be of any type. Clearly, its value may change dynamically during the game, which makes this kind of queries very powerful. We can also run this query only on the entities processed by the `SysMovement`:

```cpp
int x_limit = 200;

ecsa::Vector<ecsa::Entity, 100> ids = table.query<100, SYSMOVEMENT, int>(&find_entities_with_positive_x, x_limit);
```

## Appendix: boosting performance with ARM code

In GBA development, when you need some extra performance it is often a good idea to compile performance-critical parts of your program as ARM instructions, stored in IWRAM (by default, code is compiled as Thumb and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but similar macros exist for other libraries, like libtonc. 

We can apply this principle to systems to improve performance for critical parts of the game: in ECSA, systems generally implement small `update` functions that take care of very specialized tasks; this makes it easy to identify performance-critical parts of your program to compile as ARM code in IWRAM. For example, we can modify the query `SysMovement` from above like this - first, the `sys_movement.h` file:

```cpp
class SysMovement : public ecsa::System<100>;
{
    Table & table;

    public:

    SysMovement(Table & table);
    bool select(ecsa::Entity e) override;
    BN_CODE_IWRAM void update() override; // marked as IWRAM (ARM) code
};
```

Then, we will implement in `sys_movement.cpp` all functions APART from the `update` function:

```cpp
#include "sys_movement.h"

SysMovement::SysMovement(Table & table)
    : ecsa::System<100>()
{

}

bool SysMovement::select(ecsa::Entity e) override
{
    return table.has<POSITION>(e) && table.has<VELOCITY>(e);
}
```

And finally we can implement the `update` function in a file named `sys_movement.iwram.cpp`:

```cpp
#include "sys_movement.h"

void SysMovement::update()
{
    for (ecsa::Entity e : this->subscribed())
    {
        Vecotr2 & p = table.get<Vecotr2, POSITION>;
        Vecotr2 & v = table.get<Vecotr2, VELOCITY>;

        p.x += v.x;
        p.y += v.y;
    }
}
```