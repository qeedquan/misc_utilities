for i in *.obj; do
	obj2gltf -i $i $(basename $i.obj).gltf
done
