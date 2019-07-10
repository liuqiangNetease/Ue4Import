#pragma once
#include "JsonTypes.h"
#include "JsonLight.h"
#include "JsonRenderer.h"
#include "JsonSkinRenderer.h"
#include "JsonReflectionProbe.h"
#include "JsonTerrain.h"
#include "JsonAnimation.h"
#include "JsonCollider.h"
#include "JsonRigidbody.h"
#include "JsonPhysics.h"

class JsonGameObject{
public:
	FString name;
	int32 id;
	int32 instanceId;

	FString scenePath;

	FVector localPosition;
	FQuat localRotation;
	FVector localScale;
	FMatrix worldMatrix = FMatrix::Identity;
	FMatrix localMatrix = FMatrix::Identity;
	int32 parentId;
	FString parentName;
	//int32 meshId;
	ResId meshId;

	bool activeSelf = true;
	bool activeInHierarchy = true;

	bool isStatic;
	bool lightMapStatic;
	bool navigationStatic;
	bool occluderStatic;
	bool occludeeStatic;

	bool nameClash;
	FString uniqueName;

	int32 prefabRootId;
	int32 prefabObjectId;
	bool prefabInstance;
	bool prefabModelInstance;
	FString prefabType;

	TArray<JsonLight> lights;
	TArray<JsonReflectionProbe> probes;
	TArray<JsonRenderer> renderers;
	TArray<JsonSkinRenderer> skinRenderers;
	TArray<JsonTerrain> terrains;
	TArray<JsonAnimator> animators;

	TArray<JsonCollider> colliders;
	TArray<JsonRigidbody> rigidbodies;

	TArray<JsonPhysicsJoint> joints;

	FVector unityLocalVectorToUnrealWorld(const FVector &arg) const;
	FVector unityLocalPosToUnrealWorld(const FVector &arg) const;

	bool hasParent() const{
		return parentId >= 0;
	}

	int findSuitableRootColliderIndex() const{
		for(int i = 0; i < colliders.Num(); i++){
			const auto& cur = colliders[i];
			if (!cur.isMeshCollider() && (cur.center == FVector::ZeroVector))
				return i;
		}
		return -1;
	}

	/*
	Returns index of collider that is pointing at the main mesh.

	By convention, there is only ONE mesh per object. For now.
	Of course, unity allows multiple mesh rendere components, despite usingo nly one mesh filter,
	but I'm not quite sure in which scenario this would ever be used, and how to transfer it to unreal properly.
	*/
	int findMainMeshColliderIndex() const{
		//if (meshId < 0)
		if (!meshId.isValid())
			return -1;//magic number...
		for (int i = 0; i < colliders.Num(); i++){
			const auto &cur = colliders[i];
			if (cur.isMeshCollider() && (cur.meshId == meshId))
				return i;
		}

		return -1;
	}

	const JsonCollider* getMainMeshCollider() const{
		return getColliderByIndex(findMainMeshColliderIndex());
	}

	bool hasMainMeshCollider() const{
		return getMainMeshCollider() != nullptr;
	}

	const JsonCollider* getColliderByIndex(int index) const{
		if (index < 0)
			return nullptr;
		return &colliders[index];
	}

	const JsonRenderer* getFirstRenderer() const{
		if (renderers.Num() > 0)
			return &renderers[0];
		return nullptr;
	}

	int getNumSpawnComponents() const{
		return lights.Num()
			+ probes.Num()
			+ renderers.Num()
			+ skinRenderers.Num()
			+ terrains.Num()
			//+ animators.Num() animators do not spawn components
			+ colliders.Num()
			+ rigidbodies.Num();
	}

	int getNumComponents() const{
		return lights.Num() 
			+ probes.Num()
			+ renderers.Num() 
			+ skinRenderers.Num()
			+ terrains.Num() 
			+ animators.Num()
			+ colliders.Num() 
			+ rigidbodies.Num();
	}

	IntArray getFirstMaterials() const;

	bool hasTerrain() const{
		return terrains.Num() > 0;
	}

	bool hasSkinMeshes() const{
		return skinRenderers.Num() > 0;
	}

	bool hasMesh() const{
		return meshId.isValid();
		//return meshId >= 0;
	}

	bool hasJoints() const{
		return joints.Num() > 0;
	}

	bool hasColliders() const{
		return colliders.Num() > 0;
	}

	bool hasRigidbody() const{
		return rigidbodies.Num() > 0;
	}

	bool hasLights() const{
		return lights.Num() > 0;
	}

	bool hasProbes() const{
		return probes.Num() > 0;
	}

	bool hasRenderers() const{
		return renderers.Num() > 0;
	}

	bool hasAnimators() const{
		return animators.Num() > 0;
	}

	EComponentMobility::Type getUnrealMobility() const{
		if (isStatic)
			return EComponentMobility::Static;
		return EComponentMobility::Movable;
	}

	FTransform getUnrealTransform(const FVector& localUnityOffset) const;
	FTransform getUnrealTransform() const;

	FMatrix ueWorldMatrix = FMatrix::Identity;
	FString ueName;

	void load(JsonObjPtr jsonData);
	JsonGameObject() = default;
	JsonGameObject(JsonObjPtr jsonData);
};

using JsonGameObjectArray = TArray<JsonGameObject>;