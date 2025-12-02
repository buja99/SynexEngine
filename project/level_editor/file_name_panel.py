import bpy

from .stretch_vertex import MYADDON_OT_stretch_vertex
from .create_ico_sphere import MYADDON_OT_create_ico_sphere
from .export_scene import MYADDON_OT_export_scene
from .collider import MYADDON_OT_add_collider

class MYADDON_OT_add_filename(bpy.types.Operator):

    bl_idname = "myaddon.myaddon_ot_add_filename"
    bl_label = "Add FileName"
    bl_description = "Add a custom property 'file_name' to the object"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        context.object["file_name"] = ""
        return {'FINISHED'}

class OBJECT_PT_file_name(bpy.types.Panel):
    """Object Filename Panel"""
    bl_label = "OBJECT_PT_file_name"
    bl_idname = "FileName"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Export Tools")

        layout.operator(MYADDON_OT_stretch_vertex.bl_idname, text=MYADDON_OT_stretch_vertex.bl_label)
        layout.operator(MYADDON_OT_create_ico_sphere.bl_idname, text=MYADDON_OT_create_ico_sphere.bl_label)
        layout.operator(MYADDON_OT_export_scene.bl_idname, text=MYADDON_OT_export_scene.bl_label)

        layout.separator()
        layout.label(text="Custom Properties")

        if "file_name" in context.object:
            layout.prop(context.object, '["file_name"]', text="File Name")
        else:
            layout.operator(MYADDON_OT_add_filename.bl_idname)
            layout.operator(MYADDON_OT_add_collider.bl_idname)

__all__ = ['MYADDON_OT_add_filename', 'OBJECT_PT_file_name']