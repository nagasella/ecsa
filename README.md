# ECSA (Entity-Component-System Advance)

ECSA (Entity-Component-System Advance) is an Entity-Component-System (ECS) framework written in C++. It is mostly targetting Game Boy Advance (GBA) game development, so it does not make any use of C++ features like RTTI or exceptions, but nothing prevents to use it for C++ games developed for other platforms.

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ECSA and its features. It covers all aaspects from basic usage to common optimization techniques. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

* [An introduction to Entiy Systems](#an-introduction-to-entity-systems)

* [An introduction to ECSA](#an-introduction-to-ecsa)

* [Using ECSA in your project](#using-ecsa-in-your-project)

* [Entity tables](#entity-tables)

* [Entities and components](#entities-and-components)

* [Systems](#systems)

* [Example of main program](#example-of-main-program)

* [Queries](#queries)

* [Optimized queries](#optimized-queries)

* [IWRAM components](#iwram-components)

* [Boosting performance with ARM code](#boosting-performance-with-arm-code)

## An introduction to Entity Systems

Entity Systems (or, more precisely, Entity-Component-System frameworks, ECS) have been a hot topic in game development for many years, although there are different visions about how to implement one. ECSA is inspired by the model discussed in a series of articles by Adam Martin from 2007 entitled [Entity Systems are the future of mmog development](https://web.archive.org/web/20131226102755/http://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (now only available on web archive). The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, and implement something more similar to a relational database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components, and Systems are routines that process all the entities that share a common set of Components. Entity IDs may also be retrieved through queries.

## An introduction to ECSA

ECSA is a compact, header-only framework. It aims to implement the vision described in the articles linked above, allowing you to organize your game in a way that resembles a _relational database_ (although here the _relational_ aspect is arguably not relevant here). Although ECS frameworks are generally notorious for being _cache-friendly_ and therefore providing improved performance compared to classical OOP, this is not so relevant for the GBA as it does not have a cache in the modern sense. Having said that, ECSA allows to use effectively the memory hierarchy of the GBA and take advantage of existing features like the possibility to [compile part of the game code as ARM instructions](#boosting-performance-with-arm-code) in a way that is very simple and clean, allowing to address performance-critical parts of a game easily.

ECSA's architecture is based on the following simple building blocks:
* [_Entity tables_](#entity-tables) are the main data structure of ECSA: they hold together all the data about game objects, which are organized in a tabular format. Multiple entity tables can be used at the same time
* [_Entities_](#entities-and-components) are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* [_Components_](#entities-and-components) are the _column indexes_ of the table, and they implement the data for each entity - each entity can own (or not) each component
* [_Systems_](#systems) are objects that are used to update (every frame) all the entities that satisfy a certain condition
* [_Queries_](#queries) can be performed on an entity table, and they allow to retrieve IDs of entities that satisfy a certain condition (pretty much like SQL queries allow to filter rows in a table)

## Using ECSA in your project

You can use ECSA with any GBA game engine out there. For example, [Butano](https://github.com/GValiente/butano) is an awesome game engine for GBA that abstracts a lot of the low-level GBA stuff very nicely. If you are planning to use ECSA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano project's makefile to include ECSA's `include` path. 

You can also use ECSA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website. Again, you will need to add ECSA's `include` path to your project. Then you can include ECSA simply by using:

```cpp
#include "ecsa.h"
```

## Entity tables

Entity tables allow to store game objects and their components. Technically speaking, an _entity_ is the row index of an entitiy table, while _components_ are the columns of the table. In order to optimize memory consumption, ECSA allows to parameterize entity tables with these template parameters:

* `Entities`: the maximum number of entities available (the maximum number of rows in the table)

* `Components`: the maximum number of components available (the number of columns in the table)

* `Systems`: the maximum number of systems supported by the table

Therefore, we can define an entity table like this:

```cpp
ecsa::EntityTable<100, 2, 4> table;
```

This means that the `EntityTable` can contain up to `100` entities and up to `2` components, and have `4` associated systems. If the table is big, you may need to allocate it on the heap using `new`, otherwise the GBA's IWRAM will be quickly filled; for example, an entity table defined with 256 entities and 32 components will completely fill the IWRAM, without even creating any _actual_ component.

It is generally useful to define an alias for a table like this:

```cpp
using Table = ecsa::EntityTable<100, 2, 4>;
```

This alias will be used throughout the tutorial for simplicity.

## Entities and components

A new entity can be created inside a table like this: 

```cpp
ecsa::Entity e = table.create();
```

Here, `e` is an ID that identifies the entity uniquely (`ecsa::Entity` is just an alias for `unsigned int`).

Then, we can define some components:

```cpp
#define POSITION 0
#define VELOCITY 1

struct Vector2 : public ecsa:Component 
{
    int x, y;

    Vector2() : x(0), y(0) { }

    Vector2(int x, int y) : x(x), y(y) { }
};
```

`POSITION` and `VELOCITY` are just two integer indexes that are used to identify the components in the table. Components are identified both by their _type_ and by their _index_, which allows to have multiple components of the same type. In any case, they need to inherit from the struct `ecsa::Component`. 

We can add the components to the entity created above like this:

```cpp
table.add<POSITION>(e, new Vector2());
table.add<VELOCITY>(e, new Vector2(1, 1));
```

It is important that components are created using `new`, since ECSA will `delete` them when the table is destroyed. Finally, we need to `subscribe` the entity to all relevant systems in the table:

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

Systems are the objects used to process (update) entities at each frame. A system will process only entities that satisfy a certain condition.

Each system has to inherit from `ecsa::System`, a template class. For example...

```cpp
class MySystem : public ecsa::System<100, 20>
{

};
```

... Defines a system that can process up to 20 entities, and that belongs to a table that can hold up to 100 entities.

A system can implement one or more of the following functions (none is mandatory):
* `void init()`: initialization logic (ran only once)
* `void update()`: update logic (ran every frame)
* `bool select(ecsa::Entity e)`: allows to determine whether each newly created entity should be processed by the system; it is triggered automatically every time a new entity is _subscribed_ to a table

If no `select` function is defined, the system will not process _any_ entity (it can still be used to handle generic game logic, or it can work on entities obtained through [queries](#queries)).

Let's make a practical example and implement an update logic for the `POISITION` and `VELOCITY` components (basically, the system will change the position of each entity based on its velocity):

```cpp
class SysMovement : public ecsa::System<100, 100> // the table can hold 100 entities and the system can process any of those
{
    // a reference to the entity table
    Table & table;

    public:

    // constructor - takes a reference to the entity table
    SysMovement(Table & table) : ecsa::System<100, 100>(), table(table)
    {

    }

    // the system will process only entities that have both a POSITION and VELOCITY component
    bool select(ecsa::Entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    // update logic for each frame
    void update() override
    {
        // loop on all entities subscribed to this system and update their components
        for (ecsa::Entity e : this->subscribed())
        {
            Vector2 & p = table.get<Vector2, POSITION>(e);
            Vector2 & v = table.get<Vector2, VELOCITY>(e);

            p.x += v.x;
            p.y += v.y;
        }
    }
};
```

Then, the system can be added to the table like this:

```cpp
#define SYSMOVEMENT 0

table.add<SYSMOVEMENT>(new SysMovement(table));
```

`SYSMOVEMENT` is an integer index which has to be defined for each system (called system ID). As for components, this index is used to retrieve systems from the table:

```cpp
SysMovement * sys = table.get<SYSMOVEMENT>();
```

IMPORTANT NOTE: systems are processed in the order defined by their system IDs, therefore the system with ID `0` will be the first one to be processed, followed by the next ones, in order.

It is possible at run time to activate or deactivate systems, for example...

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

This makes it easy to block certain functionalities during the game, such as when implementing pause menus, etc.

## Example of main program

Here is an example of how everything explained in the previous sections is put together in the main program:

```cpp
#include "ecsa.h"

#define POSITION 0
#define VELOCITY 1

#define SYSMOVEMENT 0

int main()
{
    // 1. define an entity table
    Table table;

    // 2. add all the systems
    table.add<SYSMOVEMENT>(new SysMovement(table));

    // 3. call init() for every system
    table.init();

    // 4. add some entities with their components at this point...
    ecsa::Entity e = table.create();
    table.add<POSITION>(e, new Vector2());
    table.add<VELOCITY>(e, new Vector2(1, 1));
    table.subscribe(e);

    // main loop
    while (true)
    {
        // 5. call update() for every system previously added
        table.update();
    }
}

```

## Queries

Often it is useful, at any point in the program, to retrieve the IDs of entites that satisfy a certain condition: queries allow to do exactly this. ECSA offers several types of queries, listed in the next sub-sections. In general, the result of a query is an `ecsa::EntityBag`, which is a vector-like data structure with maximum capacity defined at compile time (however, remember that by design it does NOT preserve the order of the elements when an element is removed).

### 1. Queries based on a system

We can obtain all the IDs of the entities currently subscribed to a certain system (for example, the `SysMovement` defined above) by using:

```cpp
ecsa::EntityBag<100> ids = table.query<100, SYSMOVEMENT>();
```

The bag size should be the same as the size defined for the system (in the case above, `100`). Now we can loop on these entities, for example using a range-based for loop:

```cpp
for (ecsa::Entity e : ids)
{
    // do something...
}
```

### 2. Queries based on a function

Arbitrarily complex queries can be performed through simple functions. The function has to be defined as follows:

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
ecsa::EntityBag<100> ids = table.query<100>(&find_entities_with_positive_x);
```

The query above will run on _every_ entity in the table. However, to make it faster, we may decide to run it only on the entities processed by `SysMovement`:

```cpp
ecsa::EntityBag<100> ids = table.query<100, SYSMOVEMENT>(&find_entities_with_positive_x);
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

Now, the function can be used to obtain a bag of entity IDs:

```cpp
int x_limit = 200;

ecsa::EntityBag<100> ids = table.query<100, int>(&find_entities_with_positive_x, x_limit);
```

Here, the parameter we are passing is an `int`, but it could be of any type. Clearly, its value may change dynamically during the game, which makes this kind of queries very powerful. We can also run this query only on the entities processed by `SysMovement`:

```cpp
int x_limit = 200;

ecsa::EntityBag<100> ids = table.query<100, SYSMOVEMENT, int>(&find_entities_with_positive_x, x_limit);
```

## Optimized queries

The queries based on user-defined functions from the previous section are convenient to use, but they can be limiting in terms of performance. The main issue is related to the fact that the query function is called once per entity, which can be quite taxing performance-wise (especially on constrained hardware like the GBA). To help improve this situation, ECSA also has an optimized implementation of each type of query shown previously; the drawback is that optimized queries require a little bit more work from the programmer's side, since they require to manually iterate all the relevant entities and select the ones that satisfy the desired condition.

Let's redefine for example the query function above called `find_entities_with_positive_x`. The optimized version of this function will look like this:

```cpp
template<int Size>
ecsa::EntityBag<Size> find_entities_with_positive_x(Table & table)
{
    EntityBag<Size> result;

    for (Entity e = 0; e < table.capacity(); e++) // iterate on whole table
    {
        if (!table.contains(e)) // check if the entity is in the table
            continue;

        // Add the entity to the query result if it satisfies the condition
        Vecotr2 & p = table.get<Vector2, POSITION>(e);
        if (p.x > 0)
            result.push_back(e);
    }
    
    return result;
}
```

In this case, we are iterating on the whole table to find relevant entities, and we should make sure that entities are actually in the table. The function returns an EntityBag, which needs to be filled manually by the programmer, unlike the queries from the previous section. We can now run this query exactly as it was shown previously:

```cpp
ecsa::EntityBag<100> ids = table.query<100>(&find_entities_with_positive_x);
```

Because the iteration on the entities was moved inside the user-defined function itself, ECSA will actually only call the function once.

If we wanted to run the query only on the entities processed by the system `SysMovement` instead of the entire table, the function has to be changed a bit:

```cpp
template<int Size>
ecsa::EntityBag<Size> find_entities_with_positive_x(Table & table, ecsa::EntityBag<Size> & entities)
{
    EntityBag<Size> result;

    for (Entity e : entities) // iterate on entities processed by SysMovement
    {
        Vecotr2 & p = table.get<Vector2, POSITION>(e);
        if (p.x > 0)
            result.push_back(e);
    }
    
    return result;
}
```

In this case, the function needs to take as an input also an additional `EntityBag` argument (reference), which corresponds to the entities subscribed to the `SysMovement`. The body of the function is somewhat simplified because there is no need anymore to check that entities are contained in the table - if they are processed by a system, they are surely in the table. We can execute it with the usual syntax:

```cpp
ecsa::EntityBag<100> ids = table.query<100, SYSMOVEMENT>(&find_entities_with_positive_x);
```

Finally, we can add a parameter also to this kind of query:

```cpp
template<int Size>
ecsa::EntityBag<Size> find_entities_with_positive_x(Table & table, ecsa::EntityBag<Size> & entities, int & x_limit)
{
    EntityBag<Size> result;

    for (Entity e : entities)
    {
        Vecotr2 & p = table.get<Vector2, POSITION>(e);
        if (p.x > x_limit)
            result.push_back(e);
    }
    
    return result;
}
```

And call it like this:

```cpp
int x_limit = 200;

ecsa::EntityBag<100> ids = table.query<100, SYSMOVEMENT, int>(&find_entities_with_positive_x, x_limit);
```


## IWRAM components

The GBA has two main working memories: IWRAM, small (32 kbytes) but fast, and EWRAM, bigger (256 kbytes) but slower to access. When compiling a game with the devkit arm toolchain, normally objects declared on the stack are allocated in IWRAM, while objects declared on the heap (using `new` or `malloc`) are allocated in EWRAM. 

From what was shown previously, ECSA components are generally allocated using `new` and therefore end up in the GBA's EWRAM: this is fine for many cases, but it can limit performance for components used frequently and by many entities. Also in a modern system this will not be very efficient, since components will be scattered across the heap. 

ECSA offers the possibility to allocate components in IWRAM too. When working with IWRAM components, we work directly with an _arrray_ of components instead of individual components. We can declare an array of components on the stack along with the entity table (the size of the arrays must be the same as the maximum number of entities in the table):

```cpp
// table with 100 entities
ecsa::EntityTable<100, 32, 32> table;

ecsa::Array<Vector2, 100> positions; // array of POSITION components declared on the stack (IWRAM)
ecsa::Array<Vector2, 100> velocities; // array of VELOCITY components declared on the stack (IWRAM)

// add the arrays to the table and register them as component arrays
table.add<POSITION>(&positions); 
table.add<VELOCITY>(&velocities);
```

It is now possible to retrieve a reference to the array like this:

```cpp
ecsa::Array<Vector2, 100> & positions = table.get<Vecotr2, POSITION>();
```

Then, the individual components can be accessed by a simple index - the index being the entity ID. For example, we can re-write the movement system described in previous sections like this:

```cpp
class SysMovement : public ecsa::System<100>
{
    // a reference to the individual component arrays
    ecsa::Array<Vector2, 100> & positions;
    ecsa::Array<Vector2, 100> & velocities;

    public:

    // constructor - takes a reference to the entity table
    SysMovement(Table & table) : 
        ecsa::System<100, 100>(), 
        positions(table.get<Vector2, POSITION>()), // extract component arrays from the table
        velocities(table.get<Vector2, VELOCITY>()),
    {

    }

    // the system will process only entities that have both a POSITION and VELOCITY component
    bool select(ecsa::Entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    // update logic for each frame
    void update() override
    {
        // loop on all entities subscribed to this system
        for (ecsa::Entity e : this->subscribed())
        {
            Vector2 & p = positions[e]; // components accessed through indexes
            Vector2 & v = velocities[e];

            p.x += v.x;
            p.y += v.y;
        }
    }
};
```

IWRAM components can be particularly useful when coupled with [compiling code as ARM instructions](#boosting-performance-with-arm-code). The drawback of using IWRAM components is that you are allocating a whole array instead of individual components, which can fill up IWRAM quickly: for this reason, the case where this type of component is most useful is when most of the entities in the table use a specific component.

IWRAM components are interesting also for modern platforms, as arrays are cache-friendly data structures that can be iterated quickly. However, in the case of a modern system you should not allocate the array on the stack, but on the heap, like this:

```cpp
Array<Vector2, 100> * positions = new Array<Vector2, 100>();
table.add<POSITION>(positions); 
```

And remember to `delete` the array manually once it is not needed anymore.

## Boosting performance with ARM code

In GBA development, when you need some extra performance it is often a good idea to compile critical parts of your program as ARM instructions, which are then loaded in IWRAM (by default, code is compiled as thumb instructions and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but similar macros exist for other libraries, like libtonc. 

In ECSA, systems generally implement small `update` functions that take care of very specialized tasks; this makes it easy to identify performance-critical parts of your program to compile as ARM code in IWRAM. For example, we can modify the query `SysMovement` from above like this - first, the `sys_movement.h` file:

```cpp
class SysMovement : public ecsa::System<100, 100>;
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
    : ecsa::System<100, 100>()
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
        Vecotr2 & p = table.get<Vecotr2, POSITION>(e);
        Vecotr2 & v = table.get<Vecotr2, VELOCITY>(e);

        p.x += v.x;
        p.y += v.y;
    }
}
```

In this example, regular components were used, but when this approach is coupled with IWRAM components it allows to obtain an even higher performance boost.