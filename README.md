# ECSA (Entity System Advance)

ECSA (Entity-Component-System Advance) is an Entity-Component-System (ECS) framework written in C++. It is mostly targetting Game Boy Advance (GBA) game development, so it does not make any use of C++ features like RTTI or exceptions.

Below is a basic introduction to Entity Systems, as well as a comprehensive tutorial about ESA and its features. You can use it to learn and also as a reference to come back to from time to time. It covers all aaspects from basic usage to common optimization techniques. In this repo you'll also find a few examples that cover all the main feature of ESA. All examples are based on butano for now. For additional questions or help, you can reach out through the [GBADev discord](https://github.com/gbadev-org/awesome-gbadev?tab=readme-ov-file#community).

## Table of contents

- [An introduction to Entity Systems](#an-introduction-to-entity-systems)

- [An introduction to ESA](#an-introduction-to-esa)

- [Using ESA in your project](#using-esa-in-your-project)

- [Tutorial - Part 1: basic building blocks](#tutorial---part-1-basic-building-blocks)

    - [A simple example](#a-simple-example)

    - [Entity tables](#entity-tables)

    - [Components](#components)

    - [Entity lifecycle](#entity-lifecycle)

    - [Updaters](#updaters)

    - [Queries](#queries)

    - [Apply](#apply)

- [Tutorial - Part 2: advanced aspects](#tutorial---part-2-advanced-aspects)

    - [Storing components in IWRAM](#storing-components-in-iwram)

    - [Accessing components with the `[]` operator](#accessing-components-using-the--operator)

    - [Indexed components and index updaters](#indexed-components-and-index-updaters)

- [Appendix A: boosting performance with ARM code](#appendix-a-boosting-performance-with-arm-code)

- [Appendix B: ESA helper components](#appendix-b-esa-helper-components)

## An introduction to Entity Systems

Entity Systems have been a hot topic in game development for several years, although there are different visions about how to implement one. ESA is inspired by the model discussed in [this series of articles from 2007](https://t-machine.org/index.php/2007/09/03/entity-systems-are-the-future-of-mmog-development-part-1/) (always a great read for anyone interested in the topic). It implements some of these ideas, although with a focus on the GBA hardware.

The TL;DR version would be that an Entity System organizes the game logic in a way that is fundamentally different from a typical OOP approach, where game objects are generally represented by the instances of some classes and contain both the _data_ and the _logic_ of the objects. On the contrary, an Entity System is supposed to separate data from logic, implementing something more similar to a database: game objects (Entities) are organized into tables, where the row index is the ID of an Entity, the columns are the available Components for the entities, and Systems are routines that process all the entities that share a common set of Components. The IDs of groups of entities that share common aspects are obtained through queries on the database.

## An introduction to ESA

ESA is a compact, header-only framework. It aims to implement the vision described in the articles linked above, allowing you to organize your game in a way that resembles a _relational database_ (although here the _relational_ aspect is arguably not that important). It provides a [clear framework](#tutorial---part-1-basic-building-blocks) to organize the code of your games, it makes it easy to [distribute the data of your game between IWRAM and EWRAM](#storing-components-in-iwram), it allows to easily [optimize memory consumption](#indexed-components-and-index-updaters) where necessary and [makes it simple to identify performance-critical parts of your code to compile as ARM instructions](#appendix-a-boosting-performance-with-arm-code). 

Its main building blocks are the following:
* The [_Entity table_](#entity-tables) is the main data structure of ESA: it contains all the data about game objects, organized in a tabular format
* [_Entities_](#entity-lifecycle) are IDs that identify game objects in a unique way; they are the _row index_ of the entity table
* [_Components_](#entity-lifecycle) are the _column indices_ of the table, and they implement the data for each entity (each entity can own (or not) each component)
* [_Queries_](#queries) replace what in an ECS is normally called _system_; they are objects that are used for multiple purposes:
    * They allow to retrieve the IDs of entities that satisfy a certain condition
    * They allow to process (update every frame) all the entities that satisfy a certain condition

## Using ESA in your project

You can use ECSA with any GBA game engine out there. For example, [Butano]() is an awesome game engine for GBA, which I highly recommend. If you are planning to use ESA with the butano game engine, you can find excellent information about how to set up DevkitARM and butano [here](https://gvaliente.github.io/butano/getting_started.html). After that, just clone or download this repo and set up the butano porject Makefile to include ESA's `include` path. 

You can also use ESA with other libraries (like libtonc, libgba, ...), and for general information about the available options and how to set up your development environment check out the [getting started](https://gbadev.net/getting-started.html) page of the awesome GBAdev community website. Again, you will need to add ESA's `include` path to your project.

ESA comes with a few example projects, all of which are based on butano for the moment. Examples are always a good way to learn since you can try to tweak things and see what happens. If you want to run these projects, make sure that the relative path of butano defined in their Makefile matches its location on your computer.

## A simple example

Let's start with a simple example about using ESA. Below you can see how to define some components, allocate an entity table and create some entities.

```cpp
#include "ecsa.h"

// define tags for the components
#define POSITION 0
#define VELOCITY 1

// define two components
struct Position : public ecsa::Component
{
    int x, y;

    Position(int x, int y) : x(x), y(y) { }
};

struct Vecloity : public ecsa::Component
{
    int x, y;

    Velocity(int x, int y) : x(x), y(y) { }
};

int main()
{
    // define an entity table with 100 entities, 2 components and 0 queries
    ecsa::EntityTable<100, 2, 0> table;

    // create 100 entities and give them components
    for (int i = 0; i < 100; i++)
    {
        ecsa::Entity e = table.create();
        table.add<POSITION>(e, new Position(0, 0));
        table.add<VELOCITY>(e, new Velocity(1, 1));
        table.subscribe(e);
    }

    // udpate entities
    for (ecsa::Entity e = 0; e < 100; e++)
    {
        Position & p = table.get<Position, POSITION>(e);
        Velocity & v = table.get<Velocity, VELOCITY>(e);

        p.x += v.x;
        p.y += v.y;
    }
}
```

## Entity tables

In order to optimize memory consumption, ESA allows to parameterize entity tables with these template parmaeters:

* `Entities`: the maximum number of entities available (the number of rows in the table)

* `Components`: the maximum number of [components](#components) available (the number of columns in the table)

* `Queries`: the maximum number of [queries](#queries) supported by the table

In the [example](#a-simple-example) above, the entity table was parameterized like this:

```cpp
ecsa::EntityTable<100, 2, 0> table;
```

This means that the `EntityTable` can contain at most `100` entities and `2` components. No queries are allowed here (although this is not a typical scenario).

## Entity lifecycle

An entity can be added to a table as shown in the example above: 

```cpp
ecsa::Entity e = table.create();
```

Here, `e` is an ID that identifies the entity uniquely. `ecsa::Entity` is just an alias for `unsigned short`.

Then, we can add some components to it:

```cpp
table.add<POSITION>(e, new Position(0, 0));
table.add<VELOCITY>(e, new Velocity(1, 1));
```

Here, `POSITION` and `VELOCITY` are just two integer indexes that are used to identify the components. They are used also to retrieve the components later on. 

After initializing the components, we can _subscribe_ the entity to all relevant queries:

```cpp
table.subscribe(e);
```

A reference to a component can be retrieved from an entity like this:

```cpp
Position & p = table.get<Position, POSITION>(e);
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

## Queries

Queries are objects can be used for several purposes:
1. To obtain a list of IDs of entities that satisfy a certain condition
2. To process (update) all the entities that satisfy a certain condition

Queries are used to implement most of the game logic. Each query has to inherit from `ecsa::Query`, a template class. A query can implement one or more of the following functions (none is mandatory):
* `void init()`: initialization logic (ran only once)
* `void update()`: update logic (ran every frame)
* `bool select(ecsa::Entity e)`: allows to determine whether each newly created entity should be processed by the query; it is triggered automatically every time a new entity is created
* `bool where(ecsa::Entity e)`: allows to filter the entities subscribed to the query according to some condition

The naming of `select` and `where` refers to the corresponing `SQL` clauses - `select` allows to select columns from a table (in `ECSA`, this corresponds to filtering entities based on their components) while `where` allows to filter based on the value contained in each element of the column (that is, the component's content).

Let's make a practical example and implement an update logic for the `Position` and `Velocity` components (basically, the query will change the position of each entity based on its valocity):

```cpp
using Table = ecsa::EntityTable<100, 2, 2>;

class QMovement : public ecsa::Query<100> // query expected to work on up to 100 entities
{
    // a reference to the entity table
    Table & table;

    public:

    // constructor - pass a reference to the entity table
    QMovement(Table & table) : ecsa::Query<100>(), table(table)
    {

    }

    // select clause of the query
    bool select(ecsa::entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    // where clause of the query - provides additional customized filtering
    bool where(ecsa::Entity e)
    {
        Position & p = table.get<Position, POSITION>(e);

        if (p.x > 100)
            return true;
    }

    // update logic for each frame
    void update() override
    {
        // loop on all entities currently subscribed to this query
        // they are the entities that satisfy the `select` clause
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

Then, the query can be added to the table. An integer index should also be defined, which will allow to retrieve the query later on:

```cpp
#define QMOVEMENT 0

table.add<QMOVEMENT>(new QMovement(table));
```

We can run this query in two ways. In the first case, we only take into account the `select` clause - that is, we only get a list of the IDs of all the entities processed by the query:

```cpp
ecsa::Vector<ecsa::Entity, 100> ids = table.query<QMOVEMENT, 100>();
```

In the second case, we can also take into account the `where` clause, which provides additional filtering:

```cpp
ecsa::Vector<ecsa::Entity, 100> ids = table.query<QMOVEMENT, 100>(true);
```

The argument `true` specifies that we want to use the `where` clause to filter the entities.

## Dynamic queries

Dynamic queries are queries that can take a parameter of any type in their `where` clause. In fact, a dynamic query _should always_ implement a `where` clause. Here is an example of definition of a dynamic query:

```cpp
using Table = ecsa::EntityTable<100, 2, 2>;

class QMovement : public ecsa::DynamicQuery<100, int> // query works on max 100 entities, and can take an int parameter
{
    // a reference to the entity table
    Table & table;

    public:

    // constructor - pass a reference to the entity table
    QMovement(Table & table) : ecsa::DynamicQuery<100, int>(), table(table)
    {

    }

    // select clause of the query
    bool select(ecsa::Entity e) override
    {
        return table.has<POSITION>(e) && table.has<VELOCITY>(e);
    }

    // where clause of the query - provides additional customized filtering
    // an int parameter is taken, which allows to filter dynamically
    bool where(ecsa::Entity e, int x_max)
    {
        Position & p = table.get<Position, POSITION>(e);

        if (p.x > x_max)
            return true;
    }
};
```

The query can be added to an entity table as shown previously:

```cpp
#define QMOVEMENT 0

table.add<QMOVEMENT>(new QMovement(table));
```

Then, the query can be executed like this:

```cpp
int x_max = 10;
ecsa::Vector<ecsa::Entity, 100> ids = table.query<QMOVEMENT, 100, int>(x_max);
```

This will return a vector of entities for which `x > x_max`. Note the additional template argument `int` when we call `table.query`, which was not there for regular queries.

## Game loop example

In conclusion, when using ECSA, the overall look of the main program will be something like this:

```cpp
// define IDs for queries and components
#define QUERY1 0
#define QUERY2 1

#define POSIITON 0
#define VELOCITY 1

// create an entity table with 100 entities, 2 components and 2 queries
ecsa::EntityTable<100, 2, 2> table;

// add queries (implemented elsewhere) to the table and assign them an ID
table.add<QUERY1>(new Query1(table));
table.add<QUERY2>(new Query2(table));

// call "init" for every query
table.init();

// create an entity and subscribe it to the relevant queries
ecsa::Entity e = table.create();
table.add<POSITION>(e, new Position(0, 0));
table.add<VELOCITY>(e, new Velocity(0, 0));
table.subscribe(e);

// main loop
while (1)
{
    // call "update" for every query
    table.update();
}
```

This keeps the main game loop very clean and tidy.

## Appendix: boosting performance with ARM code

In GBA development, if you feel like you need some performance boost it is often a good idea to compile some of your code in ARM instructions and store it in IWRAM (by default, code is compiled as Thumb and stored in ROM). The butano engine allows to generate ARM code in IWRAM by using the macro `BN_CODE_IWRAM` (check [this](https://gvaliente.github.io/butano/faq.html#faq_memory_arm_iwram) out in the butano FAQ), but you can do the same with other libraries too like libtonc. We can apply this principle to updaters, queries and apply objects.

In ECSA, queries generally implement small `update` functions that take care of very specialized tasks. This makes it easy to identify performance-critical parts of your program to compile as ARM code in IWRAM. For example, we can modify the query `QMovement` from above like this - first, the `q_movement.h` file:

```cpp
class QMovement : public ecsa::Query<100>;
{
    Table & table;

    public:

    QMovement(Table & table);
    bool select(ecsa::Entity e) override;
    BN_CODE_IWRAM void update() override; // marked as IWRAM (ARM) code
};
```

Then, we will implement in `q_movement.cpp` all functions apart from the `update` function:

```cpp
#include "udpater.h"

QMovement::QMovement(Table & table)
    : Qeury<100>()
{

}

bool QMovement::select(ecsa::Entity e) override
{
    return table.has<POSITION>(e) && table.has<VELOCITY>(e);
}
```

And finally we can implement the `update` function in a file named `q_movement.iwram.cpp`:

```cpp
#include "udpater.h"

void QMovement::update()
{
    for (ecsa::Entity e : this->subscribed())
    {
        Position & p = table.get<Position, POSITION>;
        Velocity & v = table.get<Veloicty, VELOCITY>;

        p.x += v.x;
        p.y += v.y;
    }
}
```