import bpy
import bpy_extras
import math
import json

class MYADDON_OT_export_scene(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
    bl_idname = "myaddon.myaddon_ot_export_scene"
    bl_label = "Export Scene"
    bl_description = "Export the current scene to a file"
    bl_options = {'REGISTER', 'UNDO'}
    filename_ext = ".json"

    def execute(self, context):
        print("Exporting scene information to file...")
        result = self.export_json()
        if result == {'FINISHED'}:
            self.report({'INFO'}, "Scene information exported to file")
        else:
            self.report({'ERROR'}, "Failed to export scene")
        return result

    def export(self):  
        print("Start writing scene data to:", self.filepath)

        try:
            with open(self.filepath, "wt") as file:
                self.write_and_print(file, "SCENE")

                for obj in bpy.context.scene.objects:
                    if obj.parent:
                        continue 
                    self.parse_scene_recursive(file, obj, 0)

        except Exception as e:
            print(f"Error: {e}")
            return {'CANCELLED'}

        print("Scene export complete.")
        return {'FINISHED'}
    
    def export_json(self):
        """Output to a file in JSON format"""
       
        json_object_root = dict()

        json_object_root["name"] = "scene"
        json_object_root["objects"] = list()

        for object in bpy.context.scene.objects:
            if(object.parent):
                continue

            self.parse_scene_recursive_json(json_object_root["objects"],object,0)

        json_text = json.dumps(json_object_root, ensure_ascii=False, cls=json.JSONEncoder, indent=4)

        print(json_text)

        with open(self.filepath,  "wt", encoding="utf-8") as file:

            file.write(json_text)

        return {'FINISHED'}
    
    def parse_scene_recursive_json(self, data_parent, object, level):
        
        json_object = dict()
        json_object["type"] = object.type
        json_object["name"] = object.name

        trans,rot,scale = object.matrix_local.decompose()
        rot = rot.to_euler()

        rot.x = math.degrees(rot.x)
        rot.y = math.degrees(rot.y)
        rot.z = math.degrees(rot.z)

        transform = dict()
        transform["translation"] = (trans.x,trans.y,trans.z)
        transform["rotation"] = (rot.x,rot.y,rot.z)
        transform["scaling"] = (scale.x,scale.y,scale.z)

        json_object["transform"] = transform

        if "file_name" in object:
            json_object["file_name"] = object["file_name"]

        if "collider" in object:
            collider = dict()
            collider["type"] = object["collider"]
            collider["center"] = object["collider_center"].to_list()
            collider["size"] = object["collider_size"].to_list()
            json_object["collider"] = collider
        
        if "Disabled option" in object:
            json_object["Disabled option"] = object["Disabled option"]


        data_parent.append(json_object)

        if len(object.children) > 0:
            json_object["children"] = list()

            for child in object.children:
                self.parse_scene_recursive_json(json_object["children"], child, level + 1)

           


    def parse_json_recursive(self, obj):
        trans, rot, scale = obj.matrix_local.decompose()
        rot = rot.to_euler()
        rot_deg = [math.degrees(r) for r in (rot.x, rot.y, rot.z)]

        obj_dict = {
            "name": obj.name,
            "type": obj.type,
            "transform": {
                "position": [trans.x, trans.y, trans.z],
                "rotation": rot_deg,
                "scale": [scale.x, scale.y, scale.z]
            },
            "children": []
        }

        if "file_name" in obj:
            obj_dict["file_name"] = obj["file_name"]

        if "collider" in obj:
            obj_dict["collider"] = {
                "type": obj["collider"],
                "center": list(obj["collider_center"]),
                "size": list(obj["collider_size"])
            }

        for child in obj.children:
            obj_dict["children"].append(self.parse_json_recursive(child))

        return obj_dict

    def parse_scene_recursive(self, file, obj, level):
        indent = "\t" * level
        self.write_and_print(file, indent + obj.type + " - " + obj.name)

        trans, rot, scale = obj.matrix_local.decompose()
        rot = rot.to_euler()
        rot = (math.degrees(rot.x), math.degrees(rot.y), math.degrees(rot.z))

        self.write_and_print(file, indent + "  Trans(%f,%f,%f)" % (trans.x, trans.y, trans.z))
        self.write_and_print(file, indent + "  Rot(%f,%f,%f)" % (rot[0], rot[1], rot[2]))
        self.write_and_print(file, indent + "  Scale(%f,%f,%f)" % (scale.x, scale.y, scale.z))
        self.write_and_print(file, "")
        if "file_name" in obj:
            self.write_and_print(file, indent + '  N "%s"' % obj["file_name"])

        if "collider" in obj:
            self.write_and_print(file, indent + '  C %s' % obj["collider"])

            temp_str = indent + '  CC %f %f %f' % (
                obj["collider_center"][0],
                obj["collider_center"][1],
                obj["collider_center"][2]
            )
            self.write_and_print(file, temp_str)

            temp_str = indent + '  CS %f %f %f' % (
                obj["collider_size"][0],
                obj["collider_size"][1],
                obj["collider_size"][2]
            )
            self.write_and_print(file, temp_str)

        self.write_and_print(file, indent + "  END")
        self.write_and_print(file, "")
        
        for child in obj.children:
            self.parse_scene_recursive(file, child, level + 1)

    def write_and_print(self, file, text):  
        print(text)
        file.write(text + "\n")

__all__ = ['MYADDON_OT_export_scene']