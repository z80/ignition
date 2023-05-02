
extends StaticPhysicsBody





func create_physical():
	var ph = super.create_physical()
	if ph != null:
		ph.mode = ph.FREEZE_MODE_STATIC


