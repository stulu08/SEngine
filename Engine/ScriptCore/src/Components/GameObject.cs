using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#pragma warning disable 1591

namespace Stulu {
	public class GameObject {

		//store components already got from InternalCalls
		private Dictionary<Type, GameObjectAttached> m_componentCache = new Dictionary<Type, GameObjectAttached>();
		public GameObject(String name) => InitFromOther(Create(name, "default", Vector3.Zero, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag) => InitFromOther(Create(name, tag, Vector3.Zero, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag, Vector3 position) => InitFromOther(Create(name, tag, position, Quaternion.Identy, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation) => InitFromOther(Create(name, tag, position, rotation, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation) => InitFromOther(Create(name, tag, position, rotation, Vector3.One));
		public GameObject(String name, String tag, Vector3 position, Quaternion rotation, Vector3 scale) => InitFromOther(Create(name, tag, position, rotation, scale));
		public GameObject(String name, String tag, Vector3 position, Vector3 rotation, Vector3 scale) => InitFromOther(Create(name, tag, position, rotation, scale));
		public GameObject(String name, Vector3 position) => InitFromOther(Create(name, "default", position, Quaternion.Identy, Vector3.One));
		public GameObject(String name, Vector3 position, Quaternion rotation) => InitFromOther(Create(name, "default", position, rotation, Vector3.One));
		public GameObject(String name, Vector3 position, Vector3 rotation) => InitFromOther(Create(name, "default", position, rotation, Vector3.One));
		public GameObject(String name, Vector3 position, Quaternion rotation, Vector3 scale) => InitFromOther(Create(name, "default", position, rotation, scale));
		public GameObject(String name, Vector3 position, Vector3 rotation, Vector3 scale) => InitFromOther(Create(name, "default", position, rotation, scale));


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

		public T addComponent<T>() where T : GameObjectAttached, new()
		{
			if (hasComponent<T>())
			{
				Log.EngineWarn("Gameobject already has component: " + typeof(T).ToString());
			}
			else
			{
				Type type = typeof(T);
				InternalCalls.gameObject_add_component(ID, type);
			}
			return getComponent<T>();
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

			if (type.IsSubclassOf(typeof(Component))) {
				Object compObj = InternalCalls.gameObject_get_component(ID, type) as T;
				if (compObj == null) {
					Log.EngineCritical("Internal Error while adding component " + type.Name + " -> get_component returned null");
					return null;
				}
				Component comp = compObj as Component;
				comp.Initilize(ID);
				m_componentCache.Add(type, comp);
				return (T)compObj;
			}
			else {
				T comp = new T();
				comp.Initilize(ID);
				m_componentCache.Add(type, (GameObjectAttached)comp);
				return (T)comp;
			}
		}
		public bool hasComponent<T>() where T : GameObjectAttached => InternalCalls.gameObject_has_component(ID, typeof(T));
		public bool hasComponent(Type type) => InternalCalls.gameObject_has_component(ID, type);
		public bool removeComponent<T>() where T : GameObjectAttached => removeComponent(typeof(T));
		public bool removeComponent(Type type)
		{
			bool rmoved = InternalCalls.gameObject_remove_component(ID, type);
			if (rmoved && m_componentCache.ContainsKey(type))
				m_componentCache.Remove(type);
			return rmoved;
		}


		internal static ulong NULL_ID = System.UInt64.MaxValue;
		internal ulong ID;
		private GameObject(ulong id) { Init(id); }

		internal static GameObject CreateInternal(UUID id)
		{
			if (id == NULL_ID)
				return null;
			return new GameObject(id);
		}

		internal void InitFromOther(GameObject other) => Init(other.ID);
		internal void Init(ulong id = UInt64.MaxValue) {
			if(id == NULL_ID)
			{
				Log.EngineWarn("Using invalid handle 'null placeholder' for gameobject");
			}
			if(m_componentCache == null)
			{
				m_componentCache = new Dictionary<Type, GameObjectAttached>();
			}
			else
			{
				m_componentCache.Clear();
			}
			this.ID = id;
		}


		public override string ToString() {
			return $"GameObject({ID})";
		}
		
	}
}
