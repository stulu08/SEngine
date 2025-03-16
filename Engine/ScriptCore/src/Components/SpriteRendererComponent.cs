#pragma warning disable 1591
using System;

namespace Stulu {
	public class SpriteRendererComponent : GameObjectAttached {
		public Texture2D texture { get => new Texture2D(InternalCalls.spriteRenComp_getTexture(id)); set => InternalCalls.spriteRenComp_setTexture(id, value.ID); }
		public Vector2 Tiling { get { InternalCalls.spriteRenComp_getTiling(id, out Vector2 val); return val; } set => InternalCalls.spriteRenComp_setTiling(id, ref value); }
		public Vector4 Color { get { InternalCalls.spriteRenComp_getColor(id, out Vector4 val); return val; } set => InternalCalls.spriteRenComp_setColor(id, ref value); }
	}
}
