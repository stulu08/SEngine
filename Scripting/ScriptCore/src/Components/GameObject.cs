using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public class GameObject {
		//store components already got from InternalCalls
		private Dictionary<Type, GameObjectAttached> m_componentCache = new Dictionary<Type, GameObjectAttached>();

		internal GameObject(uint id) { this.id = id; }
		public GameObject() => Create("New GameObject", "default", Vector3.Zero, Quaternion.Identy, Vector3.Zero);
		public GameObject(String name) => Create(name, "default", Vector3.Zero, Quaternion.Identy, Vector3.Zero);
		public GameObject(String name, String tag) => Create(name, tag, Vector3.Zero, Quaternion.Identy, Vector3.Zero);
		public GameObject(String name, String tag, Vector3 position) => Create(name, tag, position, Quaternion.Identy, Vector3.Zero);
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation) => Create(name, tag, position, rotation, Vector3.Zero);
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation) => Create(name, tag, position, rotation, Vector3.Zero);
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation, Vector3 scale) => Create(name, tag, position, rotation, scale);
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation, Vector3 scale) => Create(name, tag, position, rotation, scale);
		public GameObject(String name, Vector3 position) => Create(name, "default", position, Quaternion.Identy, Vector3.Zero);
		public GameObject(String name, Vector3 position, Quaternion rotation) => Create(name, "default", position, rotation, Vector3.Zero);
		public GameObject(String name, Vector3 position, Vector3 rotation) => Create(name, "default", position, rotation, Vector3.Zero);
		public GameObject(String name, Vector3 position, Quaternion rotation, Vector3 scale) => Create(name, "default", position, rotation, scale);
		public GameObject(String name, Vector3 position, Vector3 rotation, Vector3 scale) => Create(name, "default", position, rotation, scale);

		public GameObject Create(String name, String tag, Vector3 postion, Quaternion rotation, Vector3 Scale) {
			return new GameObject(InternalCalls.gameObject_create(name, tag, postion, rotation, Scale));
		}
		public GameObject Create(String name, String tag, Vector3 postion, Vector3 rotation, Vector3 Scale) {
			return Create(Name, tag, postion, new Quaternion(rotation,1.0f), Scale);
		}

		public String Name { get => InternalCalls.gameObject_getName(id); set => InternalCalls.gameObject_setName(id, value); }
		public String Tag { get => InternalCalls.gameObject_getTag(id); set => InternalCalls.gameObject_setTag(id, value); }
		public TransformComponent transform { get => this.getComponent<TransformComponent>(); }


		public T addComponent<T>() where T : GameObjectAttached, new() {
			if(hasComponent<T>())
				return getComponent<T>();
			Type type = typeof(T);
			if (m_componentCache.ContainsKey(type)) {
				return m_componentCache[type] as T;
			}
			InternalCalls.gameObject_add_component(id, type);
			T comp = new T();
			comp.initilize(id);
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
			comp.initilize(id);
			m_componentCache.Add(type, (GameObjectAttached)comp);
			return comp;
		}
		public bool hasComponent<T>() where T : GameObjectAttached => InternalCalls.gameObject_has_component(id, typeof(T));
		public bool hasComponent(Type type) => InternalCalls.gameObject_has_component(id, type);
		public bool removeComponent<T>() where T : GameObjectAttached {
			Type type = typeof(T);
			bool rmoved = InternalCalls.gameObject_remove_component(id, type);
			if (rmoved && m_componentCache.ContainsKey(type))
				m_componentCache.Remove(type);
			return rmoved;
		}

		internal readonly uint id;
	}
}
