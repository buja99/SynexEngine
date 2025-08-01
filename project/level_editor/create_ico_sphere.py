import bpy

class MYADDON_OT_create_ico_sphere(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_create_object"
    bl_label = "Create Ico Sphere"
    bl_description = "Create an Ico Sphere"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        bpy.ops.mesh.primitive_ico_sphere_add()
        return {'FINISHED'}

__all__ = ['MYADDON_OT_create_ico_sphere']