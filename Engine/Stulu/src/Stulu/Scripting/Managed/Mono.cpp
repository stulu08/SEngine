#include "st_pch.h"
#include "Mono.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/reflection.h>

namespace Stulu {
	namespace Mono {
		void Image::Close() {
			return mono_image_close(m_image);
		}
		std::string Image::GetStatusError(Status status) {
			return mono_image_strerror((MonoImageOpenStatus)status);
		}
		Mono::Image Image::Open(const std::string& fname, Status* status) {
			return mono_image_open(fname.c_str(), (MonoImageOpenStatus*)status);
		}
		Mono::Image Image::OpenFull(const std::string& fname, Status* status, bool refonly) {
			return mono_image_open_full(fname.c_str(), (MonoImageOpenStatus*)status, refonly);
		}
		Mono::Image Image::OpenFromData(char* data, uint32_t data_len, bool need_copy, Status* status) {
			return mono_image_open_from_data(data, data_len, need_copy, (MonoImageOpenStatus*)status);
		}
		Mono::Image Image::OpenFromDataFull(char* data, uint32_t data_len, bool need_copy, Status* status, bool refonly) {
			return mono_image_open_from_data_full(data, data_len, need_copy, (MonoImageOpenStatus*)status, refonly);
		}
		std::string Class::GetName() const {
			return mono_class_get_name(m_class);
		}
		std::string Class::GetNamespace() const {
			return mono_class_get_namespace(m_class);
		}
		bool Class::IsEnum() const {
			return mono_class_is_enum(m_class);
		}
		uint32_t Class::GetTypeToken() const {
			return mono_class_get_type_token(m_class);
		}
		Class Class::GetParent() const {
			return mono_class_get_parent(m_class);
		}
		ClassField Class::GetField(uint32_t field_token) const {
			return mono_class_get_field(m_class, field_token);
		}
		ClassField Class::GetFieldFromName(const std::string& name) const {
			return mono_class_get_field_from_name(m_class, name.c_str());
		}
		Mono::ClassField Class::GetFields(void** iter) const {
			return mono_class_get_fields(m_class, iter);
		}
		Method Class::GetMethodFromName(const std::string& name, int param_count)  const {
			return mono_class_get_method_from_name(m_class, name.c_str(), param_count);
		}
		Class Class::FromName(Image image, const std::string& nameSpace, const std::string& name) {
			return mono_class_from_name(image, nameSpace.c_str(), name.c_str());
		}
		bool Class::Compare(MonoClass* left, MonoClass* right) {
			return mono_class_get_type_token(left) == mono_class_get_type_token(right);
		}
		Image Assembly::GetImage() const {
			return mono_assembly_get_image(m_assembly);
		}
		Assembly Assembly::LoadFromFull(Mono::Image image, const std::string& fname, Image::Status* status, bool refonly) {
			return mono_assembly_load_from_full(image, fname.c_str(), (MonoImageOpenStatus*)status, refonly);
		}
		Domain::Domain(const std::string& name, const std::string& configFile) {
			if(configFile.empty())
				m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), NULL);
			else
				m_domain = mono_domain_create_appdomain(const_cast<char*>(name.c_str()), const_cast<char*>(configFile.c_str()));
		}
		void Domain::Unload() {
			mono_domain_unload(m_domain);
		}
		bool Domain::Set(bool force) {
			return mono_domain_set(m_domain, force);
		}
		Assembly Domain::OpenAssembly(const std::string& name) const {
			return mono_domain_assembly_open(m_domain, name.c_str());
		}
		MonoDomain* Domain::GetDomain() {
			return mono_domain_get();
		}
		std::string Type::GetName() const {
			return mono_type_get_name(m_type);
		}
		std::string Type::GetNameFull(TypeNameFormat format) const {
			return mono_type_get_name_full(m_type, (MonoTypeNameFormat)format);
		}
		int Type::GetType() const {
			return mono_type_get_type(m_type);
		}
		Type ReflectionType::GetType() const {
			return mono_reflection_type_get_type(m_type);
		}
		Class Type::GetClass() const {
			return mono_type_get_class(m_type);
		}
		MonoArrayType* Type::GetArrayType() const {
			return mono_type_get_array_type(m_type);
		}
		Type Type::GetPtrType() const {
			return mono_type_get_ptr_type(m_type);
		}
		Class Type::GetModifiers(bool* is_required, void** iter) const {
			return mono_type_get_modifiers(m_type, (mono_bool*)is_required, iter);;
		}
		bool Type::IsStruct() const {
			return mono_type_is_struct(m_type);
		}
		bool Type::IsVoid() const {
			return mono_type_is_void(m_type);
		}
		bool Type::IsPointer() const {
			return mono_type_is_pointer(m_type);
		}
		bool Type::IsRefrence() const {
			return mono_type_is_reference(m_type);
		}
		bool Type::IsGenericParameter() const {
			return mono_type_is_generic_parameter(m_type);
		}
		const std::string Method::GetFullName(bool signature) const {
			return mono_method_full_name(m_method, signature);
		}
		std::string String::ToUtf8() const {
			return mono_string_to_utf8(m_string);
		}
		String String::New(Domain main, const std::string& text) {
			return mono_string_new(main, text.c_str());
		}
		String Object::ToString(MonoObject** exc) const {
			return mono_object_to_string(m_object, exc);
		}
		Class Object::GetClass() const {
			return mono_object_get_class(m_object);
		}
		Method CustomAttrEntry::GetConstructor() const {
			return reinterpret_cast<MonoCustomAttrEntry*>(m_entry)->ctor;
		}
		size_t CustomAttrEntry::GetDataSize() const {
			return (size_t)reinterpret_cast<MonoCustomAttrEntry*>(m_entry)->data_size;
		}
		void* CustomAttrEntry::GetData() const{
			return (void*)reinterpret_cast<MonoCustomAttrEntry*>(m_entry)->data;
		}
		CustomAttrInfo CustomAttrInfo::FromField(Class clas, ClassField field) {
			return reinterpret_cast<_MonoCustomAttrInfo*>(mono_custom_attrs_from_field(clas, field));
		}
		CustomAttrInfo CustomAttrInfo::FromClass(Class clas) {
			return reinterpret_cast<_MonoCustomAttrInfo*>(mono_custom_attrs_from_class(clas));
		}
		CustomAttrInfo CustomAttrInfo::FromMethod(Method method) {
			return reinterpret_cast<_MonoCustomAttrInfo*>(mono_custom_attrs_from_method(method));
		}
		bool CustomAttrInfo::HasAttribute(Class attribute) const {
			return mono_custom_attrs_has_attr(reinterpret_cast<MonoCustomAttrInfo*>(m_infos), attribute);
		}
		Object CustomAttrInfo::GetAttribute(Class attribute) const {
			return mono_custom_attrs_get_attr(reinterpret_cast<MonoCustomAttrInfo*>(m_infos), attribute);
		}
		void CustomAttrInfo::Free() {
			mono_custom_attrs_free(reinterpret_cast<MonoCustomAttrInfo*>(m_infos));
		}
		Object Object::New(Domain domain, Class klass) {
			return mono_object_new(domain, klass);
		}
		std::string ClassField::GetName() const {
			return mono_field_get_name(m_field);
		}
		Type ClassField::GetType() const {
			return mono_field_get_type(m_field);
		}
		Class ClassField::GetParent() const {
			return mono_field_get_parent(m_field);
		}
		uint32_t ClassField::GetFlags() const {
			return mono_field_get_flags(m_field);
		}
		uint32_t ClassField::GetOffset() const {
			return mono_field_get_offset(m_field);
		}
		const char* ClassField::GetData() const {
			return nullptr;
		}
		void ClassField::s_SetValue(Object obj, ClassField field, void* value) {
			return mono_field_set_value(obj, field, value);
		}
		void ClassField::s_SetValueStatic(MonoVTable* vt, ClassField field, void* value) {
			return mono_field_static_set_value(vt, field, value);
		}
		void ClassField::s_GetValue(Object obj, ClassField field, void* value) {
			return mono_field_get_value(obj, field, value);
		}
		void ClassField::s_GetValueStatic(MonoVTable* vt, ClassField field, void* value) {
			return mono_field_static_get_value(vt, field, value);
		}
		Object ClassField::s_GetValueObject(Domain domain, ClassField field, Object obj) {
			return mono_field_get_value_object(domain, field, obj);
		}
		void MethodDesc::Free() {
			mono_method_desc_free(m_methodDesc);
		}
		bool MethodDesc::Match(Method method) const {
			return mono_method_desc_match(m_methodDesc, method);
		}
		bool MethodDesc::IsFull() const {
			return mono_method_desc_is_full(m_methodDesc);
		}
		bool MethodDesc::FullMatch(Method method) const {
			return mono_method_desc_full_match(m_methodDesc, method);
		}
		Method MethodDesc::SearchInClass(Class klass) const {
			return mono_method_desc_search_in_class(m_methodDesc, klass);
		}
		Method MethodDesc::SearchInImage(Image image) const {
			return mono_method_desc_search_in_image(m_methodDesc, image);
		}
		MethodDesc MethodDesc::New(const std::string& name, bool include_namespace) {
			return mono_method_desc_new(name.c_str(), include_namespace);
		}
		MethodDesc MethodDesc::FromMethod(Method method) {
			return mono_method_desc_from_method(method);
		}
		Thread Thread::GetCurrent() {
			return mono_thread_current();
		}
		Thread Thread::GetMain() {
			return mono_thread_get_main();
		}
		void Thread::SetMain(Thread thread) {
			mono_thread_set_main(thread);
		}
		void GCHandle::Free() {
			mono_gchandle_free(m_handle);
			m_handle = 0;
		}
		Mono::Object GCHandle::GetTarget() {
			return mono_gchandle_get_target(m_handle);
		}
		GCHandle GCHandle::New(Mono::Object obj, bool pinned) {
			return mono_gchandle_new(obj, pinned);
		}
		GCHandle GCHandle::NewWeakRef(Mono::Object obj, bool track_resurrection) {
			return mono_gchandle_new_weakref(obj, track_resurrection);
		}
		Array Array::New(Domain domain, Class clas, size_t size) {
			return mono_array_new(domain, clas, size);
		}
		void Array::SetRef(size_t index, Object value){
			mono_array_setref(m_array, index, value);
		}
		ST_MONO_API void SetDirs(const std::string& assembly_dir, const std::string& config_dir) {
			return mono_set_dirs(assembly_dir.c_str(), config_dir.c_str());
		}
		ST_MONO_API Domain GetRootDomain() {
			return mono_get_root_domain();
		}
		ST_MONO_API Class GetObjectClass() {
			return mono_get_object_class();
		}
		ST_MONO_API void AddInternallCall(const std::string& name, const void* method) {
			return mono_add_internal_call(name.c_str(), method);
		}
		ST_MONO_API Object RuntimeInvoke(Method method, void* obj, void** params, MonoObject** exception) {
			return mono_runtime_invoke(method, obj, params, exception);
		}
		ST_MONO_API void RuntimeObjectInit(Mono::Object object) {
			mono_runtime_object_init(object);
		}

		ST_MONO_API Domain JIT::Init(const std::string& file) {
			return mono_jit_init(file.c_str());
		}
		ST_MONO_API Domain JIT::InitVersion(const std::string& root_domain_name, const std::string& runtime_version) {
			return mono_jit_init_version(root_domain_name.c_str(), runtime_version.c_str());
		}
		ST_MONO_API int JIT::Exec(Domain domain, Assembly assembly, int argc, char* argv[]) {
			return mono_jit_exec(domain, assembly, argc, argv);
		}
		ST_MONO_API void JIT::Cleanup(Domain domain) {
			return mono_jit_cleanup(domain);
		}
		ST_MONO_API void JIT::set_crash_chaining(bool chain_signals) {
			return mono_set_crash_chaining(chain_signals);
		}
		ST_MONO_API void JIT::SetAotMode(AotMode mode) {
			return mono_jit_set_aot_mode((MonoAotMode)mode);
		}
		ST_MONO_API bool JIT::AotCompiling() {
			return  mono_jit_aot_compiling();
		}
		ST_MONO_API void JIT::SetBreakPolicy(BreakPolicyFunc policy_callback) {
			static BreakPolicyFunc pc = policy_callback;
			static MonoBreakPolicy(*wrapper) (MonoMethod* method) = [](MonoMethod* method) -> MonoBreakPolicy {
				return (MonoBreakPolicy)pc(method);
			};
			mono_set_break_policy(wrapper);
		}
		void JIT::ParseOptions(int argc, char* argv[]) {
			return mono_jit_parse_options(argc, argv);
		}
		ST_MONO_API char* JIT::GetRuntimeBuildInfo() {
			return mono_get_runtime_build_info();
		}
		ST_MONO_API void JIT::SetUseLLVM(bool use_llvm) {
			return mono_set_use_llvm(use_llvm);
		}
		ST_MONO_API void JIT::AotRegisterModule(void** aot_info) {
			return mono_aot_register_module(aot_info);
		}
		ST_MONO_API Domain JIT::JitThreadAttach(Domain domain) {
			return mono_jit_thread_attach(domain);
		}
		ST_MONO_API bool JIT::set_trace_options(const char* options) {
			return mono_jit_set_trace_options(options);
		}
		ST_MONO_API void JIT::set_signal_chaining(bool chain_signals) {
			return mono_set_signal_chaining(chain_signals);
		}
}
}