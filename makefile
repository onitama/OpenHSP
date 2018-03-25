CC = gcc
CXX = g++
AR = ar
CFLAGS_DISH = -Wall --exec-charset=UTF-8 -DHSPDISH -DHSPLINUX -DHSPDEBUG -DUSE_OBAQ
CFLAGS_GP = -Wall --exec-charset=UTF-8 -DHSPDISH -DHSPDISHGP -DHSPLINUX -DHSPDEBUG -I src/hsp3dish/extlib/src -I src/hsp3dish/extlib/src/glew -I src/hsp3dish/gameplay/src -std=c++11
CFLAGS_CL = -Wall -std=c++11 --exec-charset=UTF-8 -DHSPLINUX -DHSPDEBUG
CFLAGS_CMP = -Wall -std=c++11 --exec-charset=UTF-8 -DHSPLINUX -DHSPDEBUG

OBJS = \
	src/hsp3/dpmread.do \
	src/hsp3dish/geometry.do \
	src/hsp3dish/hgdx.do \
	src/hsp3dish/hgemitter.do \
	src/hsp3dish/hgevent.do \
	src/hsp3dish/hgmodel.do \
	src/hsp3dish/hgobj.do \
	src/hsp3/hsp3.do \
	src/hsp3/hsp3code.do \
	src/hsp3/hsp3debug.do \
	src/hsp3dish/hsp3gr_dish.do \
	src/hsp3/hsp3int.do \
	src/hsp3/hspvar_core.do \
	src/hsp3/hspvar_double.do \
	src/hsp3/hspvar_int.do \
	src/hsp3/hspvar_label.do \
	src/hsp3/hspvar_str.do \
	src/hsp3/hspvar_struct.do \
	src/hsp3dish/hspwnd_dish.do \
	src/hsp3dish/hspwnd_obj.do \
	src/hsp3dish/random.do \
	src/hsp3/stack.do \
	src/hsp3/strbuf.do \
	src/hsp3/strnote.do \
	src/hsp3dish/sysreq.do \
	src/hsp3dish/emscripten/hgtex.do \
	src/hsp3dish/emscripten/hgiox.do \
	src/hsp3dish/emscripten/mmman.do \
	src/hsp3dish/emscripten/main.do \
	src/hsp3dish/emscripten/stb_image.do \
	src/hsp3dish/obaq/omkedraw.do \
	src/hsp3dish/obaq/hsp3dw.do \
	src/hsp3dish/obaq/game.do \
	src/obaq/physics/rock.do \
	src/obaq/physics/vessel.do \
	src/hsp3dish/linux/hsp3dish.do \
	src/hsp3dish/linux/webtask_linux.do \
	src/hsp3dish/linux/supio_linux.do

OBJS_CMP = \
	src/hspcmp/ahtmodel.o \
	src/hspcmp/ahtobj.o \
	src/hspcmp/codegen.o \
	src/hspcmp/comutil.o \
	src/hspcmp/errormsg.o \
	src/hspcmp/hsc3.o \
	src/hspcmp/hspcmd.o \
	src/hspcmp/label.o \
	src/hspcmp/localinfo.o \
	src/hspcmp/main.o \
	src/hspcmp/membuf.o \
	src/hspcmp/strnote.o \
	src/hspcmp/tagstack.o \
	src/hspcmp/token.o \
	src/hspcmp/linux/supio_linux.o

OBJS_CL = \
	src/hsp3/linux/main.o \
	src/hsp3/hsp3.o \
	src/hsp3/hsp3code.o \
	src/hsp3/hsp3debug.o \
	src/hsp3/hsp3int.o \
	src/hsp3/hspvar_core.o \
	src/hsp3/hspvar_double.o \
	src/hsp3/hspvar_int.o \
	src/hsp3/hspvar_label.o \
	src/hsp3/hspvar_str.o \
	src/hsp3/hspvar_struct.o \
	src/hsp3/stack.o \
	src/hsp3/strbuf.o \
	src/hsp3/strnote.o \
	src/hsp3/dpmread.o \
	src/hsp3/linux/supio_linux.o \
	src/hsp3/linux/hsp3cl.o \
	src/hsp3/linux/hsp3ext_linux.o \
	src/hsp3/linux/hsp3gr_linux.o

OBJS_GP = \
	src/hsp3/dpmread.gpo \
	src/hsp3dish/geometry.gpo \
	src/hsp3dish/hgdx.gpo \
	src/hsp3dish/hgemitter.gpo \
	src/hsp3dish/hgevent.gpo \
	src/hsp3dish/hgmodel.gpo \
	src/hsp3dish/hgobj.gpo \
	src/hsp3/hsp3.gpo \
	src/hsp3/hsp3code.gpo \
	src/hsp3/hsp3debug.gpo \
	src/hsp3dish/hsp3gr_dish.gpo \
	src/hsp3/hsp3int.gpo \
	src/hsp3/hspvar_core.gpo \
	src/hsp3/hspvar_double.gpo \
	src/hsp3/hspvar_int.gpo \
	src/hsp3/hspvar_label.gpo \
	src/hsp3/hspvar_str.gpo \
	src/hsp3/hspvar_struct.gpo \
	src/hsp3dish/hspwnd_dish.gpo \
	src/hsp3dish/hspwnd_obj.gpo \
	src/hsp3dish/random.gpo \
	src/hsp3/stack.gpo \
	src/hsp3/strbuf.gpo \
	src/hsp3/strnote.gpo \
	src/hsp3dish/sysreq.gpo \
	src/hsp3dish/emscripten/hgtex.gpo \
	src/hsp3dish/emscripten/mmman.gpo \
	src/hsp3dish/emscripten/main.gpo \
	src/hsp3dish/emscripten/stb_image.gpo \
	src/hsp3dish/win32gp/hgiox.gpo \
	src/hsp3dish/win32gp/gamehsp.gpo \
	src/hsp3dish/win32gp/gpevent.gpo \
	src/hsp3dish/win32gp/gpcam.gpo \
	src/hsp3dish/win32gp/gplgt.gpo \
	src/hsp3dish/win32gp/gpmat.gpo \
	src/hsp3dish/win32gp/gpphy.gpo \
	src/hsp3dish/linux/hsp3dish.gpo \
	src/hsp3dish/linux/webtask_linux.gpo \
	src/hsp3dish/linux/supio_linux.gpo

OBJS_GAMEPLAY = \
	src/hsp3dish/gameplay/src/AbsoluteLayout.gpo \
	src/hsp3dish/gameplay/src/AIAgent.gpo \
	src/hsp3dish/gameplay/src/AIController.gpo \
	src/hsp3dish/gameplay/src/AIMessage.gpo \
	src/hsp3dish/gameplay/src/AIState.gpo \
	src/hsp3dish/gameplay/src/AIStateMachine.gpo \
	src/hsp3dish/gameplay/src/AnimationClip.gpo \
	src/hsp3dish/gameplay/src/AnimationController.gpo \
	src/hsp3dish/gameplay/src/Animation.gpo \
	src/hsp3dish/gameplay/src/AnimationTarget.gpo \
	src/hsp3dish/gameplay/src/AnimationValue.gpo \
	src/hsp3dish/gameplay/src/AudioController.gpo \
	src/hsp3dish/gameplay/src/AudioListener.gpo \
	src/hsp3dish/gameplay/src/BoundingBox.gpo \
	src/hsp3dish/gameplay/src/BoundingSphere.gpo \
	src/hsp3dish/gameplay/src/Bundle.gpo \
	src/hsp3dish/gameplay/src/Button.gpo \
	src/hsp3dish/gameplay/src/Camera.gpo \
	src/hsp3dish/gameplay/src/CheckBox.gpo \
	src/hsp3dish/gameplay/src/Container.gpo \
	src/hsp3dish/gameplay/src/Control.gpo \
	src/hsp3dish/gameplay/src/ControlFactory.gpo \
	src/hsp3dish/gameplay/src/Curve.gpo \
	src/hsp3dish/gameplay/src/DebugNew.gpo \
	src/hsp3dish/gameplay/src/DepthStencilTarget.gpo \
	src/hsp3dish/gameplay/src/Drawable.gpo \
	src/hsp3dish/gameplay/src/Effect.gpo \
	src/hsp3dish/gameplay/src/FileSystem.gpo \
	src/hsp3dish/gameplay/src/FlowLayout.gpo \
	src/hsp3dish/gameplay/src/Font.gpo \
	src/hsp3dish/gameplay/src/Form.gpo \
	src/hsp3dish/gameplay/src/FrameBuffer.gpo \
	src/hsp3dish/gameplay/src/Frustum.gpo \
	src/hsp3dish/gameplay/src/Game.gpo \
	src/hsp3dish/gameplay/src/Gamepad.gpo \
	src/hsp3dish/gameplay/src/HeightField.gpo \
	src/hsp3dish/gameplay/src/ImageControl.gpo \
	src/hsp3dish/gameplay/src/Image.gpo \
	src/hsp3dish/gameplay/src/Joint.gpo \
	src/hsp3dish/gameplay/src/JoystickControl.gpo \
	src/hsp3dish/gameplay/src/Label.gpo \
	src/hsp3dish/gameplay/src/Layout.gpo \
	src/hsp3dish/gameplay/src/Light.gpo \
	src/hsp3dish/gameplay/src/Logger.gpo \
	src/hsp3dish/gameplay/src/Material.gpo \
	src/hsp3dish/gameplay/src/MaterialParameter.gpo \
	src/hsp3dish/gameplay/src/MathUtil.gpo \
	src/hsp3dish/gameplay/src/Matrix.gpo \
	src/hsp3dish/gameplay/src/MeshBatch.gpo \
	src/hsp3dish/gameplay/src/Mesh.gpo \
	src/hsp3dish/gameplay/src/MeshPart.gpo \
	src/hsp3dish/gameplay/src/MeshSkin.gpo \
	src/hsp3dish/gameplay/src/Model.gpo \
	src/hsp3dish/gameplay/src/Node.gpo \
	src/hsp3dish/gameplay/src/ParticleEmitter.gpo \
	src/hsp3dish/gameplay/src/Pass.gpo \
	src/hsp3dish/gameplay/src/PhysicsCharacter.gpo \
	src/hsp3dish/gameplay/src/PhysicsCollisionObject.gpo \
	src/hsp3dish/gameplay/src/PhysicsCollisionShape.gpo \
	src/hsp3dish/gameplay/src/PhysicsConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsController.gpo \
	src/hsp3dish/gameplay/src/PhysicsFixedConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsGenericConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsGhostObject.gpo \
	src/hsp3dish/gameplay/src/PhysicsHingeConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsRigidBody.gpo \
	src/hsp3dish/gameplay/src/PhysicsSocketConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsSpringConstraint.gpo \
	src/hsp3dish/gameplay/src/PhysicsVehicle.gpo \
	src/hsp3dish/gameplay/src/PhysicsVehicleWheel.gpo \
	src/hsp3dish/gameplay/src/Plane.gpo \
	src/hsp3dish/gameplay/src/Platform.gpo \
	src/hsp3dish/gameplay/src/PlatformEmscripten.gpo \
	src/hsp3dish/gameplay/src/Properties.gpo \
	src/hsp3dish/gameplay/src/Quaternion.gpo \
	src/hsp3dish/gameplay/src/RadioButton.gpo \
	src/hsp3dish/gameplay/src/Ray.gpo \
	src/hsp3dish/gameplay/src/Rectangle.gpo \
	src/hsp3dish/gameplay/src/Ref.gpo \
	src/hsp3dish/gameplay/src/RenderState.gpo \
	src/hsp3dish/gameplay/src/RenderTarget.gpo \
	src/hsp3dish/gameplay/src/Scene.gpo \
	src/hsp3dish/gameplay/src/SceneLoader.gpo \
	src/hsp3dish/gameplay/src/ScreenDisplayer.gpo \
	src/hsp3dish/gameplay/src/ScriptController.gpo \
	src/hsp3dish/gameplay/src/ScriptTarget.gpo \
	src/hsp3dish/gameplay/src/Slider.gpo \
	src/hsp3dish/gameplay/src/SpriteBatch.gpo \
	src/hsp3dish/gameplay/src/Sprite.gpo \
	src/hsp3dish/gameplay/src/Technique.gpo \
	src/hsp3dish/gameplay/src/Terrain.gpo \
	src/hsp3dish/gameplay/src/TerrainPatch.gpo \
	src/hsp3dish/gameplay/src/TextBox.gpo \
	src/hsp3dish/gameplay/src/Text.gpo \
	src/hsp3dish/gameplay/src/Texture.gpo \
	src/hsp3dish/gameplay/src/Theme.gpo \
	src/hsp3dish/gameplay/src/ThemeStyle.gpo \
	src/hsp3dish/gameplay/src/TileSet.gpo \
	src/hsp3dish/gameplay/src/Transform.gpo \
	src/hsp3dish/gameplay/src/Vector2.gpo \
	src/hsp3dish/gameplay/src/Vector3.gpo \
	src/hsp3dish/gameplay/src/Vector4.gpo \
	src/hsp3dish/gameplay/src/VertexAttributeBinding.gpo \
	src/hsp3dish/gameplay/src/VertexFormat.gpo \
	src/hsp3dish/gameplay/src/VerticalLayout.gpo \
	src/hsp3dish/extlib/src/glew/GL/glew.gpo \
	src/hsp3dish/extlib/src/libpng/png.gpo \
	src/hsp3dish/extlib/src/libpng/pngerror.gpo \
	src/hsp3dish/extlib/src/libpng/pngget.gpo \
	src/hsp3dish/extlib/src/libpng/pngmem.gpo \
	src/hsp3dish/extlib/src/libpng/pngpread.gpo \
	src/hsp3dish/extlib/src/libpng/pngread.gpo \
	src/hsp3dish/extlib/src/libpng/pngrio.gpo \
	src/hsp3dish/extlib/src/libpng/pngrtran.gpo \
	src/hsp3dish/extlib/src/libpng/pngrutil.gpo \
	src/hsp3dish/extlib/src/libpng/pngset.gpo \
	src/hsp3dish/extlib/src/libpng/pngtrans.gpo \
	src/hsp3dish/extlib/src/libpng/pngwio.gpo \
	src/hsp3dish/extlib/src/libpng/pngwrite.gpo \
	src/hsp3dish/extlib/src/libpng/pngwtran.gpo \
	src/hsp3dish/extlib/src/libpng/pngwutil.gpo \
	src/hsp3dish/extlib/src/zlib/adler32.gpo \
	src/hsp3dish/extlib/src/zlib/compress.gpo \
	src/hsp3dish/extlib/src/zlib/crc32.gpo \
	src/hsp3dish/extlib/src/zlib/deflate.gpo \
	src/hsp3dish/extlib/src/zlib/gzclose.gpo \
	src/hsp3dish/extlib/src/zlib/gzlib.gpo \
	src/hsp3dish/extlib/src/zlib/gzread.gpo \
	src/hsp3dish/extlib/src/zlib/gzwrite.gpo \
	src/hsp3dish/extlib/src/zlib/infback.gpo \
	src/hsp3dish/extlib/src/zlib/inffast.gpo \
	src/hsp3dish/extlib/src/zlib/inflate.gpo \
	src/hsp3dish/extlib/src/zlib/inftrees.gpo \
	src/hsp3dish/extlib/src/zlib/trees.gpo \
	src/hsp3dish/extlib/src/zlib/uncompr.gpo \
	src/hsp3dish/extlib/src/zlib/zutil.gpo

OBJS_BULLET_COLLISION = \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btAxisSweep3.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btDbvt.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btDispatcher.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.gpo \
	src/hsp3dish/extlib/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btCollisionObject.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btCollisionWorld.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btGhostObject.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btManifoldResult.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/btUnionFind.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btBox2dShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btBoxShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btCapsuleShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btCollisionShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btCompoundShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConcaveShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConeShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvex2dShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexHullShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexInternalShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexPolyhedron.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btCylinderShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btEmptyShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btMultiSphereShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btOptimizedBvh.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btShapeHull.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btSphereShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btStaticPlaneShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btStridingMeshInterface.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTetrahedronShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleBuffer.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleCallback.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleMesh.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btTriangleMeshShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/CollisionShapes/btUniformScalingShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btContactProcessing.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btGenericPoolAllocator.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btGImpactBvh.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btGImpactShape.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/btTriangleShapeEx.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/gim_box_set.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/gim_contact.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/gim_memory.gpo \
	src/hsp3dish/extlib/src/BulletCollision/Gimpact/gim_tri_collision.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btConvexCast.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.gpo \
	src/hsp3dish/extlib/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.gpo

OBJS_BULLET_DYNAMICS = \
	src/hsp3dish/extlib/src/BulletDynamics/Character/btKinematicCharacterController.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btContactConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btFixedConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btGearConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btHingeConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btSliderConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btTypedConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Dynamics/btRigidBody.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Dynamics/Bullet-C-API.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBody.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyJointMotor.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Featherstone/btMultiBodyPoint2Point.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/MLCPSolvers/btDantzigLCP.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/MLCPSolvers/btMLCPSolver.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Vehicle/btRaycastVehicle.gpo \
	src/hsp3dish/extlib/src/BulletDynamics/Vehicle/btWheelInfo.gpo

OBJS_LINEAR_MATH = \
	src/hsp3dish/extlib/src/LinearMath/btAlignedAllocator.gpo \
	src/hsp3dish/extlib/src/LinearMath/btConvexHull.gpo \
	src/hsp3dish/extlib/src/LinearMath/btConvexHullComputer.gpo \
	src/hsp3dish/extlib/src/LinearMath/btGeometryUtil.gpo \
	src/hsp3dish/extlib/src/LinearMath/btPolarDecomposition.gpo \
	src/hsp3dish/extlib/src/LinearMath/btQuickprof.gpo \
	src/hsp3dish/extlib/src/LinearMath/btSerializer.gpo \
	src/hsp3dish/extlib/src/LinearMath/btVector3.gpo

TARGETS = hsp3dish hsp3gp hsp3cl hspcmp hsed
LIBS1 = -lm -lGL -lEGL -lSDL -lSDL_image -lSDL_mixer -lstdc++
LIBS2 = -lm -lGL -lEGL -lSDL -lSDL_image -lSDL_mixer -lstdc++
LIBS_GP = \
	libgameplay.a \
	libBulletDynamics.a \
	libBulletCollision.a \
	libLinearMath.a

all: $(TARGETS)

.SUFFIXES: .cpp
hsp3dish: $(OBJS)
	$(CXX) $(CFLAGS_DISH) $(OBJS) -s -o $@ $(LIBS1)
%.do: %.c
	$(CC) $(CFLAGS_DISH) -c $< -o $*.do
%.do: %.cpp
	$(CXX) $(CFLAGS_DISH) -c $< -o $*.do

hsp3gp: $(OBJS_GP) $(LIBS_GP)
	$(CXX) $(CFLAGS_GP) $(OBJS_GP) -s -o $@ $(LIBS2) $(LIBS_GP)
%.gpo: %.c
	$(CC) $(CFLAGS_GP) -c $< -o $*.gpo
%.gpo: %.cpp
	$(CXX) $(CFLAGS_GP) -c $< -o $*.gpo

hspcmp: $(OBJS_CMP)
	$(CXX) $(CFLAGS_CMP) $(OBJS_CMP) -s -o $@
%.o: %.c
	$(CC) $(CFLAGS_CMP) -c $< -o $*.o
%.o: %.cpp
	$(CXX) $(CFLAGS_CMP) -c $< -o $*.o

hsp3cl: $(OBJS_CL)
	$(CXX) $(CFLAGS_CL) $(OBJS_CL) -lm -lstdc++ -s -o $@
%.o: %.c
	$(CC) $(CFLAGS_CL) -c $< -o $*.o
%.o: %.cpp
	$(CXX) $(CFLAGS_CL) -c $< -o $*.o

hsed: src/tools/hsed_gtk2.cpp
	$(CXX) -O2 -Wall -o hsed src/tools/hsed_gtk2.cpp `pkg-config --cflags --libs gtk+-2.0`

libgameplay.a: $(OBJS_GAMEPLAY)
	rm -f $@
	$(AR) rcs $@ $(OBJS_GAMEPLAY)

libBulletCollision.a: $(OBJS_BULLET_COLLISION)
	rm -f $@
	$(AR) rcs $@ $(OBJS_BULLET_COLLISION)

libBulletDynamics.a: $(OBJS_BULLET_DYNAMICS)
	rm -f $@
	$(AR) rcs $@ $(OBJS_BULLET_DYNAMICS)

libLinearMath.a: $(OBJS_LINEAR_MATH)
	rm -f $@
	$(AR) rcs $@ $(OBJS_LINEAR_MATH)

clean:
	rm -f $(OBJS) $(OBJS_GP) $(OBJS_CMP) $(OBJS_CL) $(OBJS_GAMEPLAY) $(TARGETS) $(LIBS_GP)

