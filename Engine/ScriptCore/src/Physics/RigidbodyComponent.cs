#pragma warning disable 1591
namespace Stulu {
	public enum ForceMode {
		Force,
		Impulse,
		VelocityChange,
		Acceleration
	};
    public class RigidbodyComponent : GameObjectAttached {
        public void addForce(Vector3 force, ForceMode mode) => InternalCalls.rigidbody_addForce(id, ref force, (uint)mode);
        public bool useGravity { get => InternalCalls.rigidbody_useGravity(id); set => InternalCalls.rigidbody_useGravity(id, value); }
        public bool kinematic { get => InternalCalls.rigidbody_kinematic(id); set => InternalCalls.rigidbody_kinematic(id, value); }
        public bool retainAccelaration { get => InternalCalls.rigidbody_retainaccel(id); set => InternalCalls.rigidbody_retainaccel(id, value); }
        public float mass { get => InternalCalls.rigidbody_mass(id); set => InternalCalls.rigidbody_mass(id, value); }
        public Vector3 massCenterPos { get { InternalCalls.rigidbody_massCenterGet(id, out Vector3 val); return val; } set => InternalCalls.rigidbody_massCenterSet(id, ref value); }
        
        public bool LockAngularVelocityX { get => InternalCalls.rigidbody_lockAngularVelocity(id, 0); set => InternalCalls.rigidbody_lockAngularVelocity(id, 0, value); }
        public bool LockAngularVelocityY { get => InternalCalls.rigidbody_lockAngularVelocity(id, 1); set => InternalCalls.rigidbody_lockAngularVelocity(id, 1, value); }
        public bool LockAngularVelocityZ { get => InternalCalls.rigidbody_lockAngularVelocity(id, 2); set => InternalCalls.rigidbody_lockAngularVelocity(id, 2, value); }

		public bool LockLinearVelocityX { get => InternalCalls.rigidbody_lockLinearVelocity(id, 0); set => InternalCalls.rigidbody_lockLinearVelocity(id, 0, value); }
		public bool LockLinearVelocityY { get => InternalCalls.rigidbody_lockLinearVelocity(id, 1); set => InternalCalls.rigidbody_lockLinearVelocity(id, 1, value); }
		public bool LockLinearVelocityZ { get => InternalCalls.rigidbody_lockLinearVelocity(id, 2); set => InternalCalls.rigidbody_lockLinearVelocity(id, 2, value); }
	}
}
