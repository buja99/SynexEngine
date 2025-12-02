import bpy
import mathutils
import gpu
import gpu_extras.batch
import copy

class MYADDON_OT_add_collider(bpy.types.Operator):
    bl_idname = "myaddon.myaddon_ot_add_collider"
    bl_label = "Add Collider"
    bl_description = "['collider']Add a custom property"
    bl_options = {'REGISTER', 'UNDO'}

    def execute(self, context):
        context.object["collider"] = "BOX"
        context.object["collider_center"] = mathutils.Vector((0, 0, 0))
        context.object["collider_size"] = mathutils.Vector((2, 2, 2))
        return {'FINISHED'}
        
class OBJECT_PT_collider(bpy.types.Panel):
    bl_idname = "OBJECT_PT_collider"
    bl_label = "Collider"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "object"

    def draw(self, context):
        layout = self.layout
        obj = context.object

        layout.label(text="Collider Settings")

        if "collider" in obj:
            
            layout.prop(obj, '["collider"]', text="Type")
            layout.prop(obj, '["collider_center"]', text="Center")
            layout.prop(obj, '["collider_size"]', text="Size")
        else:
            
            layout.operator(MYADDON_OT_add_collider.bl_idname, text="Add Collider")

class DrawCollider:
    handle = None

    @staticmethod
    def draw_collider():

        vertices = {"pos": []}
        indices = []
        offsets = [
        [-0.5, -0.5, -0.5], [0.5, -0.5, -0.5],
        [-0.5, 0.5, -0.5],  [0.5, 0.5, -0.5],
        [-0.5, -0.5, 0.5],  [0.5, -0.5, 0.5],
        [-0.5, 0.5, 0.5],   [0.5, 0.5, 0.5]
        ]
        size = [2, 2, 2]

        for object in bpy.context.scene.objects:

            if "collider" not in object:
                continue

            center = mathutils.Vector((0, 0, 0))
            size = mathutils.Vector((2, 2, 2))
            
            center[0] = object["collider_center"][0]
            center[1] = object["collider_center"][1]
            center[2] = object["collider_center"][2]
            
            size[0] = object["collider_size"][0]
            size[1] = object["collider_size"][1]
            size[2] = object["collider_size"][2]

            start = len(vertices["pos"])
            for offset in offsets:
                pos = copy.copy(center)
                pos[0] += offset[0] * size[0]
                pos[1] += offset[1] * size[1] 
                pos[2] += offset[2] * size[2]

                pos = object.matrix_world @ pos

                vertices["pos"].append(pos)

                indices.append([start + 0,start+1])
                indices.append([start + 2,start+3])
                indices.append([start + 0,start+2])
                indices.append([start + 1,start+3])

                indices.append([start + 4,start+5])
                indices.append([start + 6,start+7])
                indices.append([start + 4,start+6])
                indices.append([start + 5,start+7])

                indices.append([start + 0,start+4])
                indices.append([start + 1,start+5])
                indices.append([start + 2,start+6])
                indices.append([start + 3,start+7])


        shader = gpu.shader.from_builtin("UNIFORM_COLOR")
        batch = gpu_extras.batch.batch_for_shader(shader, "LINES", vertices, indices = indices)
        color = [0.5, 1.0, 1.0, 1.0]
        shader.bind()
        shader.uniform_float("color", color)
        batch.draw(shader)



def add_draw_handler():
    DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(
        DrawCollider.draw_collider, (), "WINDOW", "POST_VIEW"
    )

def remove_draw_handler():
    if DrawCollider.handle:
        bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle, "WINDOW")
        DrawCollider.handle = None

__all__ = ['MYADDON_OT_add_collider', 'OBJECT_PT_collider', 'DrawCollider', 'add_draw_handler', 'remove_draw_handler']