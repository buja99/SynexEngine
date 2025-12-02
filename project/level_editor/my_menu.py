import bpy

from .spawn import (
    MYADDON_OT_spawn_create_player_symbol,
    MYADDON_OT_spawn_create_enemy_symbol,
)

from .stretch_vertex import MYADDON_OT_stretch_vertex
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene


class TOPBAR_MT_my_menu(bpy.types.Menu):
    bl_idname = "TOPBAR_MT_my_menu"
    bl_label = "MyMenu"
    bl_description = "My custom menu by Park Yijun"  

    def draw(self, context):
        self.layout.operator("wm.url_open_preset", text="Manual", icon='HELP')
        self.layout.operator(MYADDON_OT_stretch_vertex.bl_idname, text=MYADDON_OT_stretch_vertex.bl_label)
        self.layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, text=MYADDON_OT_create_ico_sphere.bl_label)
        self.layout.operator(MYADDON_OT_export_scene.bl_idname, text=MYADDON_OT_export_scene.bl_label)
        self.layout.separator()
        self.layout.operator(MYADDON_OT_spawn_create_player_symbol.bl_idname)
        self.layout.operator(MYADDON_OT_spawn_create_enemy_symbol.bl_idname)
        
def draw_my_menu(self, context):
    self.layout.menu(TOPBAR_MT_my_menu.bl_idname)

__all__ = ['TOPBAR_MT_my_menu', 'draw_my_menu']