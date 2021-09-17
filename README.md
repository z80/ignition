

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
- [ ] A BUG: When switches to orbiting ref. frame engine thrust seems to get rest to 0. And have to reastart the engine in order to make it non zero again.
- [ ] A BUG: "A body with joints is destroyed" warning message is displayed sometimes.
- [x] Fix picking control group from the object when initilizing control group menu.
- [ ] IN PROCESS: Make zooming by attaching camera to a separate ref. frame which is a child of a physics ref. frame in which simulation happens.
- [ ] Port over the air friction model using mesh traingles.
- [ ] Make atmosphere mesh per planet.
- [ ] Vegetation and other static assets placement based on surface quad-tree node Ids.






