﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public class GameObject {
		//store components already got from InternalCalls
		private Dictionary<Type, GameObjectAttached> m_componentCache = new Dictionary<Type, GameObjectAttached>();
		internal GameObject(uint id) { Init(id); }
		public GameObject(String name) => Init(Create(name, "default", Vector3.Zero, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag) => Init(Create(name, tag, Vector3.Zero, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag, Vector3 position) => Init(Create(name, tag, position, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation) => Init(Create(name, tag, position, rotation, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation) => Init(Create(name, tag, position, rotation, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation, Vector3 scale) => Init(Create(name, tag, position, rotation, scale));
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation, Vector3 scale) => Init(Create(name, tag, position, rotation, scale));
		public GameObject(String name, Vector3 position) => Init(Create(name, "default", position, Quaternion.Identy, Vector3.One));
		public GameObject(String name, Vector3 position, Quaternion rotation) => Init(Create(name, "default", position, rotation, Vector3.One));
		public GameObject(String name, Vector3 position, Vector3 rotation) => Init(Create(name, "default", position, rotation, Vector3.One));
		public GameObject(String name, Vector3 position, Quaternion rotation, Vector3 scale) => Init(Create(name, "default", position, rotation, scale));
		public GameObject(String name, Vector3 position, Vector3 rotation, Vector3 scale) => Init(Create(name, "default", position, rotation, scale));

		public static GameObject Create(String name, String tag, Vector3 postion, Vector3 rotation, Vector3 Scale) => Create(name, tag, postion, new Quaternion(rotation,1.0f), Scale);
		public static GameObject Create(String name, String tag, Vector3 postion, Quaternion rotation, Vector3 Scale) {
			return new GameObject(InternalCalls.gameObject_create(name, tag, postion, rotation, Scale));
		}
		public static GameObject CreateSphere(String name, Vector3 postion) => CreateSphere(name, "default", postion);
		public static GameObject CreateSphere(String name, String tag, Vector3 postion) {
			return new GameObject(InternalCalls.gameObject_createSphere(name, tag, postion));
		}
		public static GameObject CreateCube(String name, Vector3 postion) => CreateCube(name, "default", postion);
		public static GameObject CreateCube(String name, String tag, Vector3 postion) {
			return new GameObject(InternalCalls.gameObject_createCube(name, tag, postion));
		}
		public static GameObject CreateCapsule(String name, Vector3 postion) => CreateCapsule(name, "default", postion);
		public static GameObject CreateCapsule(String name, String tag, Vector3 postion) {
			return new GameObject(InternalCalls.gameObject_createCapsule(name, tag, postion));
		}
		public static GameObject CreatePlane(String name, Vector3 postion) => CreatePlane(name, "default", postion);
		public static GameObject CreatePlane(String name, String tag, Vector3 postion) {
			return new GameObject(InternalCalls.gameObject_createPlane(name, tag, postion));
		}

		public static void Destroy(GameObject go) => InternalCalls.gameObject_destroy(go.ID);

		public String Name { get => InternalCalls.gameObject_getName(ID); set => InternalCalls.gameObject_setName(ID, value); }
		public String Tag { get => InternalCalls.gameObject_getTag(ID); set => InternalCalls.gameObject_setTag(ID, value); }
		public TransformComponent transform { get => this.getComponent<TransformComponent>(); }


		public T addComponent<T>() where T : GameObjectAttached, new() {
			if (hasComponent<T>()) {
				Log.EngineError("Gameobject already has component: " + typeof(T).ToString());
				return getComponent<T>();
			}
			Type type = typeof(T);
			if (m_componentCache.ContainsKey(type)) {
				return m_componentCache[type] as T;
			}
			InternalCalls.gameObject_add_component(ID, type);
			T comp = new T();
			comp.initilize(ID);
			m_componentCache.Add(type, (GameObjectAttached)comp);
			return comp;
		}
		public T getComponent<T>() where T : GameObjectAttached, new() {
			Type type = typeof(T);
			if (!hasComponent<T>()) {
				if (m_componentCache.ContainsKey(type))
					m_componentCache.Remove(type);
				return null;
			}
			if (this.m_componentCache.ContainsKey(type))
				return this.m_componentCache[type] as T;
			T comp = new T();
			comp.initilize(ID);
			m_componentCache.Add(type, (GameObjectAttached)comp);
			return comp;
		}
		public bool hasComponent<T>() where T : GameObjectAttached => InternalCalls.gameObject_has_component(ID, typeof(T));
		public bool hasComponent(Type type) => InternalCalls.gameObject_has_component(ID, type);
		public bool removeComponent<T>() where T : GameObjectAttached {
			Type type = typeof(T);
			bool rmoved = InternalCalls.gameObject_remove_component(ID, type);
			if (rmoved && m_componentCache.ContainsKey(type))
				m_componentCache.Remove(type);
			return rmoved;
		}
		internal void Init(uint id) {
			this.ID = id;
		}
		internal void Init(GameObject other) {
			this.ID = other.ID;
		}
		internal uint ID;
	}
}
