import bpy

class MYADDON_OT_add_disabled_flag(bpy.types.Operator):
    bl_idname = "myaddon.add_disabled_flag"
    bl_label = "Added disable option"
    bl_description = "Adds a disabled flag (bool type) to an object."
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        context.object["Disabled option"] = True
        return {'FINISHED'}


class OBJECT_PT_disabled_flag(bpy.types.Panel):
    bl_label = "Disabled Options Panel"
    bl_idname = "OBJECT_PT_disabled_flag"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout
        obj = context.object

        layout.label(text="Invalid Flag (Disabled Option)")

        if "Disabled option" in obj:
            layout.prop(obj, '["Disabled option"]', text="Disabled")
        else:
            layout.operator(MYADDON_OT_add_disabled_flag.bl_idname, text="Add a disabled flag")

__all__ = ['MYADDON_OT_add_disabled_flag', 'OBJECT_PT_disabled_flag']
