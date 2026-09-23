# This is an example of code interacting with the Godot Editor
@tool
extends EditorScript

# The _run() function serves as the entry point when executed
# To run the script, right click on the script in the FileSystem panel
# and select "Run"
func _run() -> void:
	print("Hello from the Godot Editor!")
	var current_scene = EditorInterface.get_edited_scene_root()
	if current_scene:
		print("Active scene name: ", current_scene.name)
	var result = custom_function(5)
	print(result)

func custom_function(n: int) -> int:
	for i in range(n):
		print("%d %f" % [i, pow(2, i)])
	return factorial(n)

func factorial(n: int) -> int:
	if n < 0:
		return 0
	if n < 2:
		return 1
	return n * factorial(n - 1)
