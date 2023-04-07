

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




