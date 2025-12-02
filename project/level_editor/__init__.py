bl_info = {
    "name": "Level Editor",
    "author": "Park Yijun",
    "version": (1, 0),
    "blender": (3, 3, 1),
    "location": "",
    "description": "Level Editor",
    "category": "Object",
}

from .spawn import (
    MYADDON_OT_spawn_create_symbol,
    MYADDON_OT_spawn_create_player_symbol,
    MYADDON_OT_spawn_create_enemy_symbol,
)

import bpy

from .collider import MYADDON_OT_add_collider, OBJECT_PT_collider, add_draw_handler, remove_draw_handler
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene
from .file_name_panel import MYADDON_OT_add_filename, OBJECT_PT_file_name
from .my_menu import TOPBAR_MT_my_menu, draw_my_menu
from .stretch_vertex import MYADDON_OT_stretch_vertex
from .disabled import MYADDON_OT_add_disabled_flag, OBJECT_PT_disabled_flag


classes = [
    MYADDON_OT_add_collider,
    OBJECT_PT_collider,
    MYADDON_OT_create_ico_sphere,
    MYADDON_OT_export_scene,
    MYADDON_OT_add_filename,
    OBJECT_PT_file_name,
    MYADDON_OT_stretch_vertex,
    TOPBAR_MT_my_menu,
    MYADDON_OT_add_disabled_flag,
    OBJECT_PT_disabled_flag,
    MYADDON_OT_spawn_create_symbol,
    MYADDON_OT_spawn_create_player_symbol,
    MYADDON_OT_spawn_create_enemy_symbol,
]

def register():
    for cls in classes:
        bpy.utils.register_class(cls)
    bpy.types.TOPBAR_MT_editor_menus.append(draw_my_menu)
    add_draw_handler()

def unregister():
    bpy.types.TOPBAR_MT_editor_menus.remove(draw_my_menu)
    remove_draw_handler()
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)