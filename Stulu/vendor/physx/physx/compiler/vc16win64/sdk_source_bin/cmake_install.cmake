# Install script for directory: C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PhysX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include/windows" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsAoS.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsFPU.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsInclude.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsInlineAoS.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsIntrinsics.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/windows/PsWindowsTrigConstants.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/windows/PxWindowsIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/windows" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/windows/PxWindowsIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/unix/PxUnixIntrinsics.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/unix" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/unix/PxUnixIntrinsics.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXFoundation_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXFoundation_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXFoundation_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxFoundation.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/foundation/PxAssert.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/foundation/PxFoundationConfig.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/foundation/PxMathUtils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/foundation/include" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/Ps.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsAlignedMalloc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsAlloca.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsAllocator.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsAoS.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsArray.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsAtomic.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsBasicTemplates.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsBitUtils.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsBroadcast.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsCpu.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsFoundation.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsFPU.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsHash.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsHashInternals.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsHashMap.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsHashSet.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsInlineAllocator.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsInlineAoS.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsInlineArray.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsIntrinsics.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsMathUtils.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsMutex.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsPool.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsSList.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsSocket.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsSort.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsSortInternals.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsString.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsSync.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsTempAllocator.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsThread.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsTime.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsUserAllocated.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsUtilities.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecMath.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecMathAoSScalar.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecMathAoSScalarInline.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecMathSSE.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecMathUtilities.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecQuat.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/foundation/include/PsVecTransform.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/Px.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxAllocatorCallback.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxProfiler.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxSharedAssert.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxBitAndData.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxBounds3.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxErrorCallback.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxErrors.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxFlags.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxIntrinsics.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxIO.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxMat33.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxMat44.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxMath.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxMemory.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxPlane.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxPreprocessor.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxQuat.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxSimpleTypes.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxStrideIterator.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxTransform.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxUnionCast.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxVec2.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxVec3.h;C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation/PxVec4.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/install/vc15win64/PxShared/include/foundation" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/Px.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxAllocatorCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxProfiler.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxSharedAssert.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxBitAndData.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxBounds3.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxErrorCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxErrors.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxFlags.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxIntrinsics.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxIO.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxMat33.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxMat44.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxMath.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxMemory.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxPlane.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxPreprocessor.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxQuat.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxSimpleTypes.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxStrideIterator.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxTransform.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxUnionCast.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxVec2.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxVec3.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/../pxshared/include/foundation/PxVec4.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/gpu/PxGpu.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cudamanager/PxCudaContextManager.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cudamanager/PxCudaMemoryManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common/windows" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/windows/PxWindowsDelayLoadHook.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysX_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysX_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysX_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysX_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxActor.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxAggregate.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulationReducedCoordinate.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulationBase.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulation.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulationJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulationJointReducedCoordinate.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxArticulationLink.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxBatchQuery.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxBatchQueryDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxBroadPhase.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxClient.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxConstraint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxConstraintDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxContact.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxContactModifyCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxDeletionListener.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxFiltering.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxForceMode.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxImmediateMode.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxLockedData.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxMaterial.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPhysics.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPhysicsAPI.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPhysicsSerialization.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPhysicsVersion.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPhysXConfig.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxPruningStructure.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxQueryFiltering.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxQueryReport.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxRigidActor.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxRigidBody.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxRigidDynamic.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxRigidStatic.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxScene.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxSceneDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxSceneLock.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxShape.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxSimulationEventCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxSimulationStatistics.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxVisualizationParameter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxBase.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxCollection.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxCoreUtilityTypes.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxMetaData.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxMetaDataFlags.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxPhysicsInsertionCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxPhysXCommonConfig.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxRenderBuffer.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxSerialFramework.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxSerializer.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxStringTable.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxTolerancesScale.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxTypeInfo.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/common/PxProfileZone.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/pvd/PxPvdSceneClient.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/pvd/PxPvd.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/pvd/PxPvdTransport.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/collision/PxCollisionDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/solver/PxSolverDefs.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/PxConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCharacterKinematic_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCharacterKinematic_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCharacterKinematic_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCharacterKinematic_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxBoxController.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxCapsuleController.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxController.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxControllerBehavior.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxControllerManager.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxControllerObstacles.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/characterkinematic/PxExtended.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCommon_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCommon_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCommon_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCommon_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxBoxGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxCapsuleGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxConvexMesh.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxConvexMeshGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxGeometryHelpers.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxGeometryQuery.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxHeightField.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxHeightFieldDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxHeightFieldFlag.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxHeightFieldGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxHeightFieldSample.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxMeshQuery.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxMeshScale.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxPlaneGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxSimpleTriangleMesh.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxSphereGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxTriangle.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxTriangleMesh.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxTriangleMeshGeometry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geometry/PxBVHStructure.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geomutils/GuContactBuffer.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/geomutils/GuContactPoint.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCooking_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCooking_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCooking_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCooking_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxBVH33MidphaseDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxBVH34MidphaseDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/Pxc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxConvexMeshDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxCooking.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxMidphaseDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxTriangleMeshDesc.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/cooking/PxBVHStructureDesc.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXExtensions_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXExtensions_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXExtensions_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXExtensions_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxBinaryConverter.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxBroadPhaseExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxCollectionExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxConstraintExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxContactJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxConvexMeshExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxD6Joint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxD6JointCreate.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDefaultAllocator.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDefaultCpuDispatcher.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDefaultErrorCallback.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDefaultStreams.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxDistanceJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxContactJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxExtensionsAPI.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxFixedJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxJointLimit.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxMassProperties.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxPrismaticJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRaycastCCD.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRepXSerializer.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRepXSimpleType.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRevoluteJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRigidActorExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxRigidBodyExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxSceneQueryExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxSerialization.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxShapeExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxSimpleFactory.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxSmoothNormals.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxSphericalJoint.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxStringTableExt.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/extensions/PxTriangleMeshExt.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/filebuf/PxFileBuf.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXVehicle_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXVehicle_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXVehicle_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXVehicle_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleComponents.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleDrive.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleDrive4W.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleDriveNW.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleDriveTank.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleNoDrive.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleSDK.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleShaders.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleTireFriction.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleUpdate.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleUtil.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleUtilControl.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleUtilSetup.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/source/fastxml/include" TYPE FILE FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/source/fastxml/include/PsFastXml.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXPvdSDK_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXPvdSDK_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXPvdSDK_static_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXPvdSDK_static_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXTask_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXTask_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXTask_64.pdb")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE FILE OPTIONAL FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXTask_64.pdb")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/task/PxCpuDispatcher.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/task/PxTask.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/task/PxTaskDefine.h"
    "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/include/task/PxTaskManager.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXFoundation_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXFoundation_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXFoundation_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXFoundation_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysX_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysX_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysX_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysX_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCharacterKinematic_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCharacterKinematic_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCharacterKinematic_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCharacterKinematic_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXPvdSDK_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXPvdSDK_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXPvdSDK_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXPvdSDK_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCommon_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCommon_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCommon_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCommon_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXCooking_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXCooking_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXCooking_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXCooking_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXExtensions_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXExtensions_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXExtensions_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXExtensions_static_64.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/debug" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/debug/PhysXVehicle_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/checked" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/checked/PhysXVehicle_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/profile" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/profile/PhysXVehicle_static_64.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc142.mt/release" TYPE STATIC_LIBRARY FILES "C:/stulu/dev/SEngine/Stulu/vendor/physx/physx/bin/win.x86_64.vc142.mt/release/PhysXVehicle_static_64.lib")
  endif()
endif()

