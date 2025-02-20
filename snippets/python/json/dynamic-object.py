import json

class Object:
    pass

def main():
    obj = Object()
    obj.xyz = 3.41
    obj.name = "Onur"
    obj.age = 35
    obj.dog = Object()
    obj.dog.name = "Apollo"
    obj.dog.cat = Object()
    obj.dog.cat = [1, 2, 3, 4, 5, 6, 7]
    obj.yawn = { "rpy": 240, "hello": "world" }
    setattr(obj, "lfo", { "K": "P" })
    setattr(obj, "XYZ", "KMU")
    print(json.dumps(obj, default=lambda o: o.__dict__, indent=4))

main()
