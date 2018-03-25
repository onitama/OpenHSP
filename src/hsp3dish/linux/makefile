CC = gcc
CXX = g++
AR = ar
CFLAGS = -Wall --exec-charset=UTF-8 -DHSPDISH -DHSPLINUX -DHSPDEBUG -DUSE_OBAQ
CFLAGS_GP = -Wall --exec-charset=UTF-8 -DHSPDISH -DHSPDISHGP -DHSPLINUX -DHSPDEBUG -I ../extlib/src -I ../extlib/src/glew -I ../gameplay/src -std=c++11

OBJS = \
	../../hsp3/dpmread.o \
	../geometry.o \
	../hgdx.o \
	../hgemitter.o \
	../hgevent.o \
	../hgmodel.o \
	../hgobj.o \
	../../hsp3/hsp3.o \
	../../hsp3/hsp3code.o \
	../../hsp3/hsp3debug.o \
	../hsp3gr_dish.o \
	../../hsp3/hsp3int.o \
	../../hsp3/hspvar_core.o \
	../../hsp3/hspvar_double.o \
	../../hsp3/hspvar_int.o \
	../../hsp3/hspvar_label.o \
	../../hsp3/hspvar_str.o \
	../../hsp3/hspvar_struct.o \
	../hspwnd_dish.o \
	../hspwnd_obj.o \
	../random.o \
	../../hsp3/stack.o \
	../../hsp3/strbuf.o \
	../../hsp3/strnote.o \
	../sysreq.o \
	../emscripten/hgtex.o \
	../emscripten/hgiox.o \
	../emscripten/mmman.o \
	../emscripten/main.o \
	../emscripten/stb_image.o \
	hsp3dish.o \
	webtask_linux.o \
	supio_linux.o

OBJS_GP = \
	../../hsp3/dpmread.gpo \
	../geometry.gpo \
	../hgdx.gpo \
	../hgemitter.gpo \
	../hgevent.gpo \
	../hgmodel.gpo \
	../hgobj.gpo \
	../../hsp3/hsp3.gpo \
	../../hsp3/hsp3code.gpo \
	../../hsp3/hsp3debug.gpo \
	../hsp3gr_dish.gpo \
	../../hsp3/hsp3int.gpo \
	../../hsp3/hspvar_core.gpo \
	../../hsp3/hspvar_double.gpo \
	../../hsp3/hspvar_int.gpo \
	../../hsp3/hspvar_label.gpo \
	../../hsp3/hspvar_str.gpo \
	../../hsp3/hspvar_struct.gpo \
	../hspwnd_dish.gpo \
	../hspwnd_obj.gpo \
	../random.gpo \
	../../hsp3/stack.gpo \
	../../hsp3/strbuf.gpo \
	../../hsp3/strnote.gpo \
	../sysreq.gpo \
	../emscripten/hgtex.gpo \
	../emscripten/mmman.gpo \
	../emscripten/main.gpo \
	../emscripten/stb_image.gpo \
	../win32gp/hgiox.gpo \
	../win32gp/gamehsp.gpo \
	../win32gp/gpcam.gpo \
	../win32gp/gplgt.gpo \
	../win32gp/gpmat.gpo \
	../win32gp/gpphy.gpo \
	hsp3dish.gpo \
	webtask_linux.gpo \
	supio_linux.gpo

OBJS_GAMEPLAY = \
	../gameplay/src/AbsoluteLayout.gpo \
	../gameplay/src/AIAgent.gpo \
	../gameplay/src/AIController.gpo \
	../gameplay/src/AIMessage.gpo \
	../gameplay/src/AIState.gpo \
	../gameplay/src/AIStateMachine.gpo \
	../gameplay/src/AnimationClip.gpo \
	../gameplay/src/AnimationController.gpo \
	../gameplay/src/Animation.gpo \
	../gameplay/src/AnimationTarget.gpo \
	../gameplay/src/AnimationValue.gpo \
	../gameplay/src/AudioController.gpo \
	../gameplay/src/AudioListener.gpo \
	../gameplay/src/BoundingBox.gpo \
	../gameplay/src/BoundingSphere.gpo \
	../gameplay/src/Bundle.gpo \
	../gameplay/src/Button.gpo \
	../gameplay/src/Camera.gpo \
	../gameplay/src/CheckBox.gpo \
	../gameplay/src/Container.gpo \
	../gameplay/src/Control.gpo \
	../gameplay/src/ControlFactory.gpo \
	../gameplay/src/Curve.gpo \
	../gameplay/src/DebugNew.gpo \
	../gameplay/src/DepthStencilTarget.gpo \
	../gameplay/src/Drawable.gpo \
	../gameplay/src/Effect.gpo \
	../gameplay/src/FileSystem.gpo \
	../gameplay/src/FlowLayout.gpo \
	../gameplay/src/Font.gpo \
	../gameplay/src/Form.gpo \
	../gameplay/src/FrameBuffer.gpo \
	../gameplay/src/Frustum.gpo \
	../gameplay/src/Game.gpo \
	../gameplay/src/Gamepad.gpo \
	../gameplay/src/HeightField.gpo \
	../gameplay/src/ImageControl.gpo \
	../gameplay/src/Image.gpo \
	../gameplay/src/Joint.gpo \
	../gameplay/src/JoystickControl.gpo \
	../gameplay/src/Label.gpo \
	../gameplay/src/Layout.gpo \
	../gameplay/src/Light.gpo \
	../gameplay/src/Logger.gpo \
	../gameplay/src/Material.gpo \
	../gameplay/src/MaterialParameter.gpo \
	../gameplay/src/MathUtil.gpo \
	../gameplay/src/Matrix.gpo \
	../gameplay/src/MeshBatch.gpo \
	../gameplay/src/Mesh.gpo \
	../gameplay/src/MeshPart.gpo \
	../gameplay/src/MeshSkin.gpo \
	../gameplay/src/Model.gpo \
	../gameplay/src/Node.gpo \
	../gameplay/src/ParticleEmitter.gpo \
	../gameplay/src/Pass.gpo \
	../gameplay/src/PhysicsCharacter.gpo \
	../gameplay/src/PhysicsCollisionObject.gpo \
	../gameplay/src/PhysicsCollisionShape.gpo \
	../gameplay/src/PhysicsConstraint.gpo \
	../gameplay/src/PhysicsController.gpo \
	../gameplay/src/PhysicsFixedConstraint.gpo \
	../gameplay/src/PhysicsGenericConstraint.gpo \
	../gameplay/src/PhysicsGhostObject.gpo \
	../gameplay/src/PhysicsHingeConstraint.gpo \
	../gameplay/src/PhysicsRigidBody.gpo \
	../gameplay/src/PhysicsSocketConstraint.gpo \
	../gameplay/src/PhysicsSpringConstraint.gpo \
	../gameplay/src/PhysicsVehicle.gpo \
	../gameplay/src/PhysicsVehicleWheel.gpo \
	../gameplay/src/Plane.gpo \
	../gameplay/src/Platform.gpo \
	../gameplay/src/PlatformEmscripten.gpo \
	../gameplay/src/Properties.gpo \
	../gameplay/src/Quaternion.gpo \
	../gameplay/src/RadioButton.gpo \
	../gameplay/src/Ray.gpo \
	../gameplay/src/Rectangle.gpo \
	../gameplay/src/Ref.gpo \
	../gameplay/src/RenderState.gpo \
	../gameplay/src/RenderTarget.gpo \
	../gameplay/src/Scene.gpo \
	../gameplay/src/SceneLoader.gpo \
	../gameplay/src/ScreenDisplayer.gpo \
	../gameplay/src/ScriptController.gpo \
	../gameplay/src/ScriptTarget.gpo \
	../gameplay/src/Slider.gpo \
	../gameplay/src/SpriteBatch.gpo \
	../gameplay/src/Sprite.gpo \
	../gameplay/src/Technique.gpo \
	../gameplay/src/Terrain.gpo \
	../gameplay/src/TerrainPatch.gpo \
	../gameplay/src/TextBox.gpo \
	../gameplay/src/Text.gpo \
	../gameplay/src/Texture.gpo \
	../gameplay/src/Theme.gpo \
	../gameplay/src/ThemeStyle.gpo \
	../gameplay/src/TileSet.gpo \
	../gameplay/src/Transform.gpo \
	../gameplay/src/Vector2.gpo \
	../gameplay/src/Vector3.gpo \
	../gameplay/src/Vector4.gpo \
	../gameplay/src/VertexAttributeBinding.gpo \
	../gameplay/src/VertexFormat.gpo \
	../gameplay/src/VerticalLayout.gpo \
	../extlib/src/glew/GL/glew.gpo \
	../extlib/src/libpng/png.gpo \
	../extlib/src/libpng/pngerror.gpo \
	../extlib/src/libpng/pngget.gpo \
	../extlib/src/libpng/pngmem.gpo \
	../extlib/src/libpng/pngpread.gpo \
	../extlib/src/libpng/pngread.gpo \
	../extlib/src/libpng/pngrio.gpo \
	../extlib/src/libpng/pngrtran.gpo \
	../extlib/src/libpng/pngrutil.gpo \
	../extlib/src/libpng/pngset.gpo \
	../extlib/src/libpng/pngtrans.gpo \
	../extlib/src/libpng/pngwio.gpo \
	../extlib/src/libpng/pngwrite.gpo \
	../extlib/src/libpng/pngwtran.gpo \
	../extlib/src/libpng/pngwutil.gpo \
	../extlib/src/zlib/adler32.gpo \
	../extlib/src/zlib/compress.gpo \
	../extlib/src/zlib/crc32.gpo \
	../extlib/src/zlib/deflate.gpo \
	../extlib/src/zlib/gzclose.gpo \
	../extlib/src/zlib/gzlib.gpo \
	../extlib/src/zlib/gzread.gpo \
	../extlib/src/zlib/gzwrite.gpo \
	../extlib/src/zlib/infback.gpo \
	../extlib/src/zlib/inffast.gpo \
	../extlib/src/zlib/inflate.gpo \
	../extlib/src/zlib/inftrees.gpo \
	../extlib/src/zlib/trees.gpo \
	../extlib/src/zlib/uncompr.gpo \
	../extlib/src/zlib/zutil.gpo
OBJS_BULLET_COLLISION = \
	../extlib/src/BulletCollision/BroadphaseCollision/btAxisSweep3.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btDbvt.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btDispatcher.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btMultiSapBroadphase.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.gpo \
	../extlib/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btCollisionObject.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btCollisionWorld.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btGhostObject.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btManifoldResult.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/btUnionFind.gpo \
	../extlib/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btBox2dShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btBoxShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btCapsuleShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btCollisionShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btCompoundShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConcaveShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConeShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvex2dShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexHullShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexInternalShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexPolyhedron.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btCylinderShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btEmptyShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btMultiSphereShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btOptimizedBvh.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btShapeHull.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btSphereShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btStaticPlaneShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btStridingMeshInterface.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTetrahedronShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleBuffer.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleCallback.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleMesh.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btTriangleMeshShape.gpo \
	../extlib/src/BulletCollision/CollisionShapes/btUniformScalingShape.gpo \
	../extlib/src/BulletCollision/Gimpact/btContactProcessing.gpo \
	../extlib/src/BulletCollision/Gimpact/btGenericPoolAllocator.gpo \
	../extlib/src/BulletCollision/Gimpact/btGImpactBvh.gpo \
	../extlib/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.gpo \
	../extlib/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.gpo \
	../extlib/src/BulletCollision/Gimpact/btGImpactShape.gpo \
	../extlib/src/BulletCollision/Gimpact/btTriangleShapeEx.gpo \
	../extlib/src/BulletCollision/Gimpact/gim_box_set.gpo \
	../extlib/src/BulletCollision/Gimpact/gim_contact.gpo \
	../extlib/src/BulletCollision/Gimpact/gim_memory.gpo \
	../extlib/src/BulletCollision/Gimpact/gim_tri_collision.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btConvexCast.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.gpo \
	../extlib/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.gpo
OBJS_BULLET_DYNAMICS = \
	../extlib/src/BulletDynamics/Character/btKinematicCharacterController.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btContactConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btFixedConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btGearConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btHingeConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btSliderConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btTypedConstraint.gpo \
	../extlib/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.gpo \
	../extlib/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.gpo \
	../extlib/src/BulletDynamics/Dynamics/btRigidBody.gpo \
	../extlib/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.gpo \
	../extlib/src/BulletDynamics/Dynamics/Bullet-C-API.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBody.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyConstraint.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyConstraintSolver.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyDynamicsWorld.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyJointMotor.gpo \
	../extlib/src/BulletDynamics/Featherstone/btMultiBodyPoint2Point.gpo \
	../extlib/src/BulletDynamics/MLCPSolvers/btDantzigLCP.gpo \
	../extlib/src/BulletDynamics/MLCPSolvers/btMLCPSolver.gpo \
	../extlib/src/BulletDynamics/Vehicle/btRaycastVehicle.gpo \
	../extlib/src/BulletDynamics/Vehicle/btWheelInfo.gpo

OBJS_LINEAR_MATH = \
	../extlib/src/LinearMath/btAlignedAllocator.gpo \
	../extlib/src/LinearMath/btConvexHull.gpo \
	../extlib/src/LinearMath/btConvexHullComputer.gpo \
	../extlib/src/LinearMath/btGeometryUtil.gpo \
	../extlib/src/LinearMath/btPolarDecomposition.gpo \
	../extlib/src/LinearMath/btQuickprof.gpo \
	../extlib/src/LinearMath/btSerializer.gpo \
	../extlib/src/LinearMath/btVector3.gpo

TARGETS = hsp3dish hsp3gp
LIBS = -lm -lGL -lEGL -lSDL -lSDL_image -lSDL_mixer -lstdc++
LIBS2 = -lm -lGL -lEGL -lSDL -lSDL_image -lSDL_mixer -lstdc++
LIBS_GP = \
	libgameplay.a \
	libBulletDynamics.a \
	libBulletCollision.a \
	libLinearMath.a

all: $(TARGETS)

.SUFFIXES: .cpp
hsp3dish: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) -s -o $@ $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $*.o
%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $*.o

hsp3gp: $(OBJS_GP) $(LIBS_GP)
	$(CXX) $(CFLAGS_GP) $(OBJS_GP) -s -o $@ $(LIBS2) $(LIBS_GP)
%.gpo: %.c
	$(CC) $(CFLAGS_GP) -c $< -o $*.gpo
%.gpo: %.cpp
	$(CXX) $(CFLAGS_GP) -c $< -o $*.gpo

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
	rm -f $(OBJS) $(OBJS_GP) $(OBJS_GAMEPLAY) $(TARGETS) $(LIBS_GP)

release:
	make clean
	make "CFLAGS = -O3 -Wall --exec-charset=UTF-8 -DHSPDISH -DHSPLINUX"
