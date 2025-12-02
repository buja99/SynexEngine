import bpy
import os

# Define the spawn type (Prototype name, name to be placed in the scene, obj file path)
class SpawnNames():
    PROTOTYPE = 0
    INSTANCE = 1
    FILENAME = 2

    names = {}
    names["Enemy"]  = ("PrototypeEnemySpawn", "EnemySpawn",  "enemy/enemy.obj")
    names["Player"] = ("PrototypePlayerSpawn","PlayerSpawn", "player/player.obj")


# Load obj (avoid duplicate loading)
def load_obj(type: str):
    spawn_object = bpy.data.objects.get(SpawnNames.names[type][SpawnNames.PROTOTYPE])
    if spawn_object is not None:
        return {'CANCELLED'}

    if type == "Player":
        full_path = "C:/Users/PARK YIJUN/Desktop/Engine/project/resources/player/player.obj"
    elif type == "Enemy":
        full_path = "C:/Users/PARK YIJUN/Desktop/Engine/project/resources/enemy/enemy.obj"
    else:
        return {'CANCELLED'}

    bpy.ops.wm.obj_import(
        'EXEC_DEFAULT',
        filepath=full_path,
        forward_axis='Z',
        up_axis='Y'
    )

    obj = bpy.context.active_object
    obj.name = SpawnNames.names[type][SpawnNames.PROTOTYPE]
    obj["type"] = SpawnNames.names[type][SpawnNames.INSTANCE]

   # Remove from scene (leaving only the original in the database)
    bpy.context.collection.objects.unlink(obj)

    return {'FINISHED'}


# Common creation operators
class MYADDON_OT_spawn_create_symbol(bpy.types.Operator):
    bl_idname = "myaddon.spawn_create_symbol"
    bl_label = "Spawn Symbol Create"
    bl_description = "Spawn Point Symbol 생성"
    bl_options = {'REGISTER', 'UNDO'}

    type: bpy.props.StringProperty(name="Type", default="Player")

    def execute(self, context):
        spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])
        if spawn_object is None:
            load_obj(self.type)
            spawn_object = bpy.data.objects.get(SpawnNames.names[self.type][SpawnNames.PROTOTYPE])

        new_obj = spawn_object.copy()
        bpy.context.collection.objects.link(new_obj)
        new_obj.name = SpawnNames.names[self.type][SpawnNames.INSTANCE]
        new_obj["type"] = SpawnNames.names[self.type][SpawnNames.INSTANCE]
        return {'FINISHED'}


# Dedicated Operator (Player)
class MYADDON_OT_spawn_create_player_symbol(bpy.types.Operator):
    bl_idname = "myaddon.spawn_create_player_symbol"
    bl_label = "Creating a player spawn point symbol"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.ops.myaddon.spawn_create_symbol('EXEC_DEFAULT', type="Player")
        return {'FINISHED'}


# Dedicated Operator (Enemy)
class MYADDON_OT_spawn_create_enemy_symbol(bpy.types.Operator):
    bl_idname = "myaddon.spawn_create_enemy_symbol"
    bl_label = "Creating enemy spawn point symbols"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.ops.myaddon.spawn_create_symbol('EXEC_DEFAULT', type="Enemy")
        return {'FINISHED'}


__all__ = [
    'MYADDON_OT_spawn_create_symbol',
    'MYADDON_OT_spawn_create_player_symbol',
    'MYADDON_OT_spawn_create_enemy_symbol',
    'SpawnNames',
    'load_obj'
]