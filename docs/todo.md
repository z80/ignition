

# ToDo list.

Here is the battle plan.
- [ ] Fix trajectory after deserialization. It is probably necessary to just call 'launch'.
- [ ] Make dynamic objects destroyed if going under the ground. It happens when flying too fast and the game cannot build collision volumes quick enough.
- [ ] Make an explosion object when dynamic body blows up.
- [ ] Make connection nodes for static bodies.
- [ ] Make it only possible to connect static bodies to "construction base" static node.
- [ ] Make it save/destroy/create/restore based on the bounding volume. May be introduce asynchronous database for this purpose.






