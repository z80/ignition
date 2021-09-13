

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
- [ ] A BUG: The issue is with split/merge. Shortest distance after split is smaller than the clustering distance just before split. It shouldn't be the case !!!!!
- [ ] Port over the air friction model using mesh traingles.
- [ ] Vegetation and other static assets placement based on surface quad-tree node Ids.






