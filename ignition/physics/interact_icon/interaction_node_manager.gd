
extends Node




# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta):
	var group: String = Constants.INTERACT_NODES_GROUP_NAME
	var nodes: Array = get_tree().get_nodes_in_group( group )
	
	var closest_node: Node = null
	var closest_dist: float = -1.0
	
	for node in nodes:
		var dist: float = node.distance_to_camera_ray()
		if dist < 0.0:
			continue
		
		if (dist < closest_dist) or (closest_dist <= 0.0):
			closest_dist = dist
			closest_node = node
	
	for node in nodes:
		var icon_visible: bool = (node == closest_node)
		node.icon_visible = icon_visible
