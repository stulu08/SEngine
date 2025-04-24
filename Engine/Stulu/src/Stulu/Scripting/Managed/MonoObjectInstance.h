#pragma once
#include "Stulu/Math/Math.h"

#include "Property/ManagedProperty.h"
#include "ScriptAssembly.h"

namespace Stulu {
	class STULU_API MonoObjectInstance {
	public:
		MonoObjectInstance(Mono::Class clasz, ScriptAssembly* assembly);
		MonoObjectInstance(const MonoObjectInstance&);
		MonoObjectInstance(MonoObjectInstance&&) = default;
		~MonoObjectInstance();

		void CallConstructor(uint32_t paramCount, void** args = nullptr);
		Mono::Object CallMethod(Mono::Method method, void** args = NULL, bool isStatic = false);

		static bool FieldHasAttribute(Ref<ManagedProperty> field, Mono::Class attribute);

		void LoadAllClassFields();

		void BackupFields(bool overwrite = false);
		void LoadFieldsBackup();

		Ref<ManagedProperty> FindField(const std::string& name) const;
		template<class T = ManagedProperty>
		Ref<T> FindFieldAs(const std::string& name) const {
			return std::dynamic_pointer_cast<T>(FindField(name));
		}

		std::unordered_map<std::string, void*> CreateFieldBackup();
		void LoadFieldsBackupFrom(std::unordered_map<std::string, void*>& map);
		
		inline bool IsInitilized() const { return m_initilized; }
		inline void SetInitilized() { m_initilized = true; }

		inline Mono::Object getObject() const { return m_object; }
		inline Mono::Class getClass() const { return m_class; }
		inline const std::vector<Ref<ManagedProperty>>& getFields() const { return m_fields; }
		inline std::vector<Ref<ManagedProperty>>& getFields() { return m_fields; }
	private:
		void CreateObject();
		void Reload();

		bool m_initilized = false;
		
		std::unordered_map<std::string, void*>* m_reloadFieldsChache = nullptr;
		std::string m_classID;

		UUID m_objectID = UUID();

		Mono::GCHandle m_garbageHandle = 0;
		Mono::Class m_class = nullptr;
		Mono::Object m_object = nullptr;
		ScriptAssembly* m_assembly;

		std::vector<Ref<ManagedProperty>> m_fields;

		friend class ScriptAssembly;
	};
}