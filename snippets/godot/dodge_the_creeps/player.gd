extends Area2D

signal hit

@export var speed = 400 # How fast the player will move (pixels/sec).
var screen_size # Size of the game window.
var invincible # Don't die on hit.

func _ready():
	screen_size = get_viewport_rect().size
	set_invincible(false)
	hide()

func _process(delta):
	var velocity = Vector2.ZERO # The player's movement vector.
	if Input.is_action_pressed(&"move_right"):
		velocity.x += 1
	if Input.is_action_pressed(&"move_left"):
		velocity.x -= 1
	if Input.is_action_pressed(&"move_down"):
		velocity.y += 1
	if Input.is_action_pressed(&"move_up"):
		velocity.y -= 1
	if Input.is_action_just_pressed(&"toggle_invincibility"):
		set_invincible(!invincible)

	if velocity.length() > 0:
		velocity = velocity.normalized() * speed
		$AnimatedSprite2D.play()
	else:
		$AnimatedSprite2D.stop()

	position += velocity * delta
	position = position.clamp(Vector2.ZERO, screen_size)

	if velocity.x != 0:
		$AnimatedSprite2D.animation = &"right"
		$AnimatedSprite2D.flip_v = false
		$Trail.rotation = 0
		$AnimatedSprite2D.flip_h = velocity.x < 0
	elif velocity.y != 0:
		$AnimatedSprite2D.animation = &"up"
		rotation = PI if velocity.y > 0 else 0

func set_invincible(state):
	invincible = state
	$Invincible.visible = invincible

func start(pos):
	position = pos
	rotation = 0
	show()
	$CollisionShape2D.disabled = false

func _on_body_entered(_body):
	if invincible:
		return
	hide() # Player disappears after being hit.
	hit.emit()
	# Must be deferred as we can't change physics properties on a physics callback.
	$CollisionShape2D.set_deferred(&"disabled", true)
