import bpy

class MYADDON_OT_stretch_vertex(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_stretch_vertex"
    bl_label = "Stretch Vertex"
    bl_description = "Pull and stretch the vertex"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        obj = bpy.data.objects.get("Cube")
        if obj is None:
            self.report({'WARNING'}, "Object 'Cube' was not found.")
            return {'CANCELLED'}
        if obj.type != 'MESH':
            self.report({'WARNING'}, "'Cube' is not a mesh object.")
            return {'CANCELLED'}
        if len(obj.data.vertices) == 0:
            self.report({'WARNING'}, "'Cube' has no vertices.")
            return {'CANCELLED'}

        obj.data.vertices[0].co.x += 1.0
        self.report({'INFO'}, "Moved the first vertex in X direction.")
        return {'FINISHED'}

__all__ = ['MYADDON_OT_stretch_vertex']