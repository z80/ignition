

# ToDo list.

Here is the battle plan.
- [x] Save.
- In process.
- [x] Save and Load.
- [x] Orbiting time acceleration.
- [x] Physics time acceleration Engine.time_scale = xxx.
- [x] Make player characters to get in/out a habitat module.
- [x] Trajectory visualization. In a separate viewport??? For planets and for the controlled vehicle.
- [x] May be rework a little bit how planet meshes are generated. When it is a distant mode, no need to apply heightmap, only apply color. And make sure it easily works for test projects, i.e. doesn't require complicated setup to actually see something.
        Somewhat done... Now generate in one and the same way... all the time.
- [x] Continuous scale application on every frame and recomputing the landscape on indication. Now it is both applied when needed and on every rf jump.
- [x] Apply scaler on every visual frame. It is especially needed for celestial bodies. Pitching up edges do look weird.
- [x] A BUG: The issue is with split/merge. Shortest distance after split is smaller than the clustering distance just before split. It shouldn't be the case !!!!!
- [x] A BUG: Fix the uncontrollable ref. frame axes spawning. Make sure it either doesn't spawn or gets destroyed.
- [x] A BUG: When switches to orbiting ref. frame engine thrust seems to get rest to 0. And have to reastart the engine in order to make it non zero again.
- [x] Not a thing anymore. A BUG: "A body with joints is destroyed" warning message is displayed sometimes.
- [x] Fix picking control group from the object when initilizing control group menu.
- [x] IN PROCESS: Make zooming by attaching camera to a separate ref. frame which is a child of a physics ref. frame in which simulation happens.
- [x] Make vehicle orbits visualization.
- [x] Adjust camera serialization/deserialization to match its modified scene.
- [x] A BUG: Assemblies are not destroyed on load. It causes a vehicle to have two assemblies.
- [x] Make control group #1 selected by default.
- [x] Make assemblies created on first request. I.e. implement setter ang getter which would do all the underground work when it is requested first.
- [x] Make dynamic splits and merges.
- [x] Currently working on.... Implement an ability to use multiple force sources like gravity, rotaional, atmosphere friction, etc...
- [x] Fix camera wobbling. Make camera up definition more clear and come from the nearest gravity center.
- [x] Fix the rotational part of the drag-rotate tool.
- [x] A BUG: Fix navball rotations. Icons seem to be correct but navball is not always.
- [x] Port over the air friction model using mesh traingles. May be also allow part connections by touching occupancy trees.
- [x] Make "esc" menu with save/load/quit options and sub menus.
- [ ] This might not be needed. Make atmosphere mesh per planet.
- [ ] Vegetation and other static assets placement based on surface quad-tree node Ids.

- [x] When moving a few connected parts in assembly and finish not connecting them to anything else, they fall through the ground. And the game fails shortly after that.
- [x] When using a decoupler, when thruster is directly below, the part above decoupler extends upwards compared to where it should be. With other nodes (other than thruster) it is ok.
- [x] With 3.3.4 Godot joint parameters changed. Don't know right now jow to make it very stiff.

- [x] Make a distance to the geoid indication.
- [x] Make atmospheric pressure indication.
- [x] Check default scaler distance. Make sure it is not too small.
- [x] Make sure thrusters do use atmospheric pressure.
- [x] Draw proper launch pad.
- [ ] Draw more realistic thrust.
- [ ] Improve sphere subdivision algorithm. Currently it doesn't look like a piece of art.
- [x] Fix the interaction menu window. Make content expand the window or slider show up. Add "x" icon as close button and "L" for resize place.
- [ ] Make static bodies to remember their initial parent planet and restore relative position on every ref. frame change.
- [ ] Camera shakiness issue. Probably something related the order in which object transforms are updated.







