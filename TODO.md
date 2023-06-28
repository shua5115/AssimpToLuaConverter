- Instead of storing a node tree, store a list of nodes and a tree of indices.
    - Then, for all data structures which reference a node, use a node index. These data structures are impacted:
        - aiNode (parent, children)
        - aiBone (node, armature)
        - aiSkeletonBone (node, armature)
        - aiScene (unrecognizable)
    - This will also require not using the regular convert() functions for aiNode, aiBone, or aiSkeletonBone.
      They must be converted in the convert(scene) function.
    - OR, what we could do is provide an optional argument for the aiNode, aiBone, and aiSkeleton functions
      which provides references to the rest of the nodes or meshes for proper referencing. This would allow
      referencing by index in these standalone functions.
- For all array values converted to lua, also provide the length of the array.
  Because of the size of arrays, the length operator will take a long time to compute if done repeatedly.
  We do not expect users to be editing this data, even though they can.