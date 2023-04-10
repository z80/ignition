

# ToDo list.

Here is the battle plan.
- [ ] Fix trajectory after deserialization. It is probably necessary to just call 'launch'.
- [ ] Make dynamic objects destroyed if going under the ground. It happens when flying too fast and the game cannot build collision volumes quick enough.
- [ ] Make an explosion object when dynamic body blows up.
- [ ] Make connection nodes for static bodies.
- [ ] Make it possible for the player to create a "construction base" used for attaching static bodies to it.
- [ ] Make it only possible to connect static bodies to "construction base" static node.
- [ ] Make it save/destroy/create/restore based on the bounding volume. May be introduce asynchronous database for this purpose.
- [ ] Resource system. May be "material" and "electricity"...
- [ ] Make "levels" purchasable for "material".
- [ ] May be port to Godot 4.
- [ ] If port to Godot 4 is successful, try implement multiplayer.
- [ ] Make it possible to pick a character.
- [ ] Implement space suit on/off capability.

- [ ] Make volumes editable. Implement nodes hierarchy. Make it possible to drag/rotate the nodes and update the resulting landscape. May be implement a mode when it is possible to add/remove volumes and design landscape this way.

- [ ] must needed: "mechjeb" type instruments/measures (one of the best KSP mod, it has a part and mostly many many helpers like rdv planner, trajectory predictions, etc)
- [ ] for more fun/missions given to player: resources scanning & mining, there was also a KSP mod for scanning from orbit
- [ ] fuel storage/renewable auto (in testing mode), because doing routine refuelling mission is just boring except for insane players ??
- [ ] important: precise control gizmos must be kept even with drag & drop, to allow precise placement of parts (for the most tight and crazy players)


# Another feedback as-is.

Eeally, this music is painful, menu is awful and we can't go back to menu, it's annoying to quit & restart the game !
I don't know why, after adding some (too much) command pods, all further items (decoupler, fuel tank) are put in game horizontally ! Very annoying, again, to build anything with this.
There is not enough feedback when we put something in game, so I add about 6 pods at the same location, no physics issue by the way !
UI is not very practical to use.
I tried TPS cam on the astronaut, bas idea, camera was completely messes up, no way to change it, no available key yet.
Building system need to be improved FIRST, it should be the top priority IMHO.

I watch a bit of the first video on the itch.io page, and wonder why not provide the savegame you used in it ? It would allow to test the core of the game without being (almost) killed first by the wall of building a rocket with boring building process.

I guess you misunderstood me, I click on add command pod button, but I've got pretty much no visual feedback and sound, so I click more time until I figured out command pod is out of view, but there is not one, instead, many command pods in one single space (objects overlapping them all as they have no physical mesh at this stage yet maybe), so another boring step to delete all those unneeded pods.

I really would like to test how rocket behave in atmosphere and space, staying on the ground is boring for a space exploration game




